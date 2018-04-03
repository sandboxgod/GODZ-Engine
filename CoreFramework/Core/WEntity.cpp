/* ===========================================================
	Game Entity implementation.

	Created Nov 8, 2003 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/


#include "WEntity.h"
#include "LevelNode.h"
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Animation/AnimationDesc.h>
#include <CoreFramework/Animation/AnimationTrigger.h>
#include "ResourceManager.h"
#include "SceneManager.h"
#include "GDZArchive.h"
#include "Mesh.h"
#include <CoreFramework/Collision/AABBTree.h>
#include "RenderQueue.h"
#include "Material.h"
#include "SceneData.h"
#include "EntityEvents.h"
#include "MActor.h"
#include "EntityComponent.h"
#include "EntityTemplate.h"
#include <CoreFramework/Math/WMatrixUtil.h>
#include <CoreFramework/Math/EulerAngleConverter.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>


namespace GODZ
{

class MotionTypeInspector : public SkeletonNodeInspector
{
public:
	MotionTypeInspector()
		: mMovementPrimitive(NULL)
	{
	}

	virtual Action inspect(SkeletonNode& node)
	{
		if (node.m_motionType == MotionType_Character || node.m_motionType == MotionType_MovementQuery)
		{
			mMovementPrimitive = node.m_primitive;
			motionType = node.m_motionType;

			//Tell the Skeleton to drop the node
			return Action_RemoveNode;
		}

		return Action_Continue;
	}

	MotionType motionType;
	GenericReference<CollisionPrimitive> mMovementPrimitive;
};

//////////////////////////////////////////////////////////////////////

ImplementGenericFlags(WEntity, ClassFlag_Placeable)

REGISTER_VECTOR(WEntity, DrawScale)
REGISTER_BOOL(WEntity, CanCollideWithWorld)
REGISTER_BOOL(WEntity, IsShadowCaster)
REGISTER_OBJECT(WEntity, Mesh, Mesh)
REGISTER_VECTOR(WEntity, Location)
REGISTER_MATRIX3(WEntity, Rotation)

WEntity::WEntity()
: m_pLevel(NULL)
, m_pMeshInstance(NULL)
, m_animController(NULL)
, m_bVisible(true)
, m_bCollideWithWorld(true)
, m_bIgnoreOutOfBounds(true)
, m_lastFrameNumber(0)
, m_layer(NULL)
, m_vel(Vector3::Zero())
, m_orientation(1)
, m_model(NULL)
, m_visualsHasChanged(false)
, m_pos(Vector3::Zero())
, m_vScale(Vector3(1,1,1))
, m_isShadowCaster(true)
, m_prevAnimFrame(-1)
, m_pSkeleton(NULL)
, m_angularVel(AxisAngle::Zero())
, m_physicsType(PhysicsUpdate_Position)
, m_renderMat(1)
{
	m_bEditable=true;

	m_id = SceneData::GetNextVisualID();

#ifdef _DEBUG
	//debugging on render side
	GenericClass* gc = GetRuntimeClass();
	SetName(gc->GetName());
#endif
}

WEntity::~WEntity()
{
	if (m_animController != NULL)
	{
		delete m_animController;
		m_animController = NULL;
	}

	m_model=NULL;

	if (m_pSkeleton != NULL)
	{
		delete m_pSkeleton;
		m_pSkeleton = NULL;
	}
}

void WEntity::OnAddToWorld()
{
	MaterialID mat;

	//Editor material
	Material* m = GetRuntimeClass()->GetMaterial();
	if (m != NULL)
	{
		mat = m->GetRuntimeID();
	}

	//tell render thread to create a renderable geominfo....
	GeometryInfo info;
	info.m_id = m_id;
	info.m_editorMaterial = mat;
	info.mIsShadowCaster = m_isShadowCaster;
	info.m_name = m_HashCode;
	info.m_transform = m_renderMat;

	//Give subclasses a chance to update the struct
	UpdateGeometryInfo(info);

	EntityCreationEvent* ev = new EntityCreationEvent(info);
	m_pLevel->AddObjectStateChangeEvent(ev);

	ResetCollision();
}

//invoked when this entity is removed from a layer
void WEntity::OnRemoved()
{
	//tell render thread to destroy our renderable data
	EntityDestroyEvent* ev = new EntityDestroyEvent(m_id);
	m_pLevel->AddObjectStateChangeEvent(ev);

	m_components.clear();

	//release refs
	m_pMeshInstance = NULL;
	mesh = NULL;
	m_template = NULL;
	m_weakReference.reset();
}

void WEntity::SetLayer(Layer* layer, const atomic_ptr<WEntity>& ptr, const EntityID& id)
{
	m_layer = layer;
	m_weakReference = ptr;
	mEntityID = id;
}

void WEntity::HideEditorMaterial()
{
	EntityStatusChangedEvent* ev = new EntityStatusChangedEvent(m_id, EntityStatusChangedEvent::kEntityChangeEvent_ClearEditorMaterial);
	m_pLevel->AddObjectStateChangeEvent(ev);
}

bool WEntity::GetCanCollideWithWorld() 
{
	return m_bCollideWithWorld; 
}

void WEntity::SetCanCollideWithWorld(bool collide)
{
	m_bCollideWithWorld = collide;
}

MotionType WEntity::CreateCollisionModel()
{
	MotionType mtype;
	m_model = NULL;

	if (mesh != NULL)
	{
		MeshInstance* mi = mesh->GetDefaultInstance();
		if (mi->GetCollisionType() == CT_Tree)
		{
			//TODO: Serialize the AABB Tree
			AABBTreeInfo treeInfo;
			AABBTreeBuilder builder(this, treeInfo);
			m_model = builder.GetRootNode();

			mtype = MotionType_Static;
		}
		else
		{
			//Check our skeleton and see if there's a movement phantom we can use....
			MotionTypeInspector inspector;
			m_pSkeleton->InspectNodes(inspector);

			//Setup debugging....
			m_pSkeleton->ComputePrimitives();

			if (inspector.mMovementPrimitive != NULL)
			{
				m_model = inspector.mMovementPrimitive;
				m_model->SetName( m_HashCode ); //debugging!
				mtype = inspector.motionType;
			}
			else
			{
				AABBCollisionPrimitive* aabb = new AABBCollisionPrimitive();
				aabb->SetName( m_HashCode ); //debugging!
				m_model = aabb;
				WBox box = mesh->GetBounds();

				WMatrix tm(1);
				tm.SetScale(m_vScale);
				tm.SetTranslation(m_pos);
				box.Transform(tm);
				aabb->SetBounds(box);

				mtype = MotionType_Character;
			}
		}
	}
	else
	{
		//Always setup the collision box
		WBox temp( Vector3(-10,-10,-10), Vector3(10,10,10) );
		GenericReference<AABBCollisionPrimitive> aabbPrim = new AABBCollisionPrimitive();
		aabbPrim->SetBounds(temp);
		m_model = aabbPrim;

		mtype = MotionType_Character;
	}

	return mtype;
}

void WEntity::RemoveFromWorld()
{
	//remove from the world
	if (m_pLevel != NULL)
	{
		m_pLevel->ReleaseEntity(this);
	}
}

AnimController* WEntity::GetAnimController()
{
	return this->m_animController;
}

CollisionPrimitive* WEntity::GetCollisionModel()
{
	return m_model;
}

const Vector3& WEntity::GetDrawScale() const
{
	return m_vScale;
}

LevelNode* WEntity::GetLevel()
{
	return m_pLevel;
}

void WEntity::SetLevel(LevelNode *level)
{
	m_pLevel=level;
}

void WEntity::UpdateRenderTransform()
{
	WMatrix16f scaleMat(1);
	scaleMat.SetScale(m_vScale);

	WMatrix16f tm(m_orientation);
	tm.SetTranslation(m_pos);

	//combine scale * offset * rotation
	WMatrixMul(scaleMat, tm, m_renderMat);


	godzassert(!IsNaN(m_renderMat._11));
	godzassert(!IsNaN(m_renderMat._12));
	godzassert(!IsNaN(m_renderMat._13));
	godzassert(!IsNaN(m_renderMat._14));

	godzassert(!IsNaN(m_renderMat._21));
	godzassert(!IsNaN(m_renderMat._22));
	godzassert(!IsNaN(m_renderMat._23));
	godzassert(!IsNaN(m_renderMat._24));

	godzassert(!IsNaN(m_renderMat._31));
	godzassert(!IsNaN(m_renderMat._32));
	godzassert(!IsNaN(m_renderMat._33));
	godzassert(!IsNaN(m_renderMat._34));

	godzassert(!IsNaN(m_renderMat._41));
	godzassert(!IsNaN(m_renderMat._42));
	godzassert(!IsNaN(m_renderMat._43));
	godzassert(!IsNaN(m_renderMat._44));

	if (m_pLevel!=NULL)
	{
		EntityTransformUpdateEvent* etu = new EntityTransformUpdateEvent(m_id, m_renderMat);
		m_pLevel->AddObjectStateChangeEvent(etu);
	}
}

// Obsolete ~ need to remove
void WEntity::SetOrientationMatrix(const WMatrix3& mat)
{
	SetRotation(mat);
}

// Obsolete ~ need to remove
const WMatrix3&	WEntity::GetOrientationMatrix()
{
	return m_orientation;
}

const WMatrix3&	WEntity::GetRotation()
{
	return m_orientation;
}

//Note: orientation matrix should always be a pure 3x3 rotation matrix. No scale or translation!
void WEntity::SetRotation(const WMatrix3& mat)
{
	if (m_pSkeleton != NULL)
	{
		WMatrix tm(1);
		CopyMatrix(mat, tm);
		tm.SetTranslation(m_pos);
		m_pSkeleton->TransformBy( mat, tm );
	}

	m_orientation = mat;
	UpdateRenderTransform();
}

bool WEntity::IsVisible()
{
	return m_bVisible;
}

void WEntity::OnAnimEnd(AnimSequence* sequence)
{
}

void WEntity::OnPropertiesUpdated()
{
	//assuming the bounds may have been updated.... (editor mode)
	if (mesh != NULL)
	{
		SetMesh(mesh);
		m_pMeshInstance->OnPropertiesUpdated();
	}

}

void WEntity::OnTick(float dt)
{
	//Make copy, this way we can allow components to modify our components list--->
	ComponentMap map = m_components;

	ComponentMap::const_iterator iter = map.begin();
	while(iter != map.end())
	{
		atomic_ptr<EntityComponent> component = iter->second;
		if ( component.m_atom->GetPtr() != NULL )
		{
			iter->second->OnTick(dt);
		};
		iter++;
	}
}


void WEntity::NotifyOutsideOfScene()
{
	if (!m_bIgnoreOutOfBounds)
	{
		RemoveFromWorld();
	}
}

void WEntity::SetPhysicsObject(PhysicsObjectProxy& obj)
{
	m_physicsObject = obj;

	//Make copy, this way we can allow components to modify our components list--->
	ComponentMap map = m_components;

	ComponentMap::const_iterator iter = map.begin();
	while(iter != map.end())
	{
		iter->second->OnSetPhysicsObject();
		iter++;
	}
}

PhysicsObjectProxy*	WEntity::GetPhysicsObject()
{
	if (m_physicsObject.IsValid())
		return &m_physicsObject;

	return NULL;
}

void WEntity::UpdatePhysics(CollisionStatus physicsState)
{
	//Make copy, this way we can allow components to modify our components list--->
	ComponentMap map = m_components;

	ComponentMap::const_iterator iter = map.begin();
	while(iter != map.end())
	{
		iter->second->UpdatePhysics(physicsState);
		iter++;
	}
}

void WEntity::UpdateTransform(float dt)
{
	//Ask our physics object where we are in the world
	if (m_physicsObject.IsValid())
	{
		long newframe = m_physicsObject.GetFrameNumber();
		if (m_lastFrameNumber != newframe)
		{
			m_lastFrameNumber = newframe;

			Vector3 pos;
			m_physicsObject.GetPosition(pos);

			/*
			if (m_rotFrame > 0)
			{
				Log("pos entity %f %f %f \n",pos.x, pos.y, pos.z);
				Log("oldpos entity %f %f %f \n",m_pos.x, m_pos.y, m_pos.z);
			}
			*/

			//Compute velocity locally, we might have missed a physics frame...
			m_vel = (pos - m_pos);

			//float v = m_vel.GetLength();
			//Log("size %f --- %f Equal %d\n", v, m_vel.GetLength(), (v == m_vel.GetLength()) );
			SetLocation(pos);
			
			if( m_physicsType == PhysicsUpdate_PosRotation )
			{
				Matrix3 m;
				m_physicsObject.GetRotation(m);
				SetOrientationMatrix(m);
			}

			UpdatePhysics(m_physicsObject.GetCollisionState());

			//Log("vel %f %f %f  Y: %f id: %d mY %f\n", m_vel.x, m_vel.y, m_vel.z, pos.y, m_geomInfo.m_id, m_pos.y);
		}
	}

	if (m_pMeshInstance != NULL)
	{
		m_pMeshInstance->Transform(dt, m_pSkeleton, GetAnimController());
		OnUpdateTransform();
	}

	//update attachments....
	std::vector<AttachmentInfo>::iterator iter;
	for(iter = m_attachedActors.begin(); iter != m_attachedActors.end(); iter++)
	{
		AttachmentInfo& info = *iter;
		atomic_ptr<WEntity> entityPtr = info.entity.lock();
		if( entityPtr != NULL )
		{
			entityPtr->SetLocation(m_pos + info.m_rel);
		}

		//todo: handle rotation... 
	}
}

void WEntity::SetIsShadowCaster(bool flag)
{
	m_isShadowCaster = flag;

	if (m_pLevel != NULL)
	{
		EntityShadowCasterEvent* ev = new EntityShadowCasterEvent(m_id, flag);
		m_pLevel->AddObjectStateChangeEvent(ev);
	}
}
	

void WEntity::OnUpdateTransform()
{
	size_t numbones = m_pSkeleton->GetNumBones();

	if (numbones > 0)
	{
		EntityBonesUpdateEvent* event = new EntityBonesUpdateEvent(m_id);
		event->m_bones.reserve(numbones);

		Bone* bones = m_pSkeleton->GetBones();

		//Sets the bone matrices for this frame
		for(size_t i=0;i<numbones;i++)
		{
			event->m_bones.push_back(bones[i].m_offsetTM);
		}

		//bones
		m_pLevel->AddObjectStateChangeEvent(event);
	}


	//We can also update all of our animation triggers at this point
	if (m_template != NULL && m_animController != NULL)
	{
		AnimationInstance* ai = m_animController->GetAnimInstance();
		if (ai != NULL && m_animController->GetSequence() > -1)
		{
			AnimSequence* s = ai->GetSequence(m_animController->GetSequence());
			int currframe = m_animController->GetCurrentAnimFrame();

			// go through all triggers and fire any that should've fired between
			// the previous frame and now!
			GodzVector<atomic_ptr<AnimationDesc> >& descs = m_template->GetAnimationList();
			for (size_t i = 0; i < descs.size(); i++)
			{
				AnimationDesc* animDesc = descs[i];
				
				if (animDesc->GetAnimationName() == s->GetName())
				{
					GodzVector<atomic_ptr<AnimationTrigger> >& triggers = animDesc->GetAnimTriggers();
					for (size_t j = 0; j < triggers.size(); j++)
					{
						AnimationTrigger* trigger = triggers[j];

						//Log("pframe %d %d tframe %d\n",m_prevAnimFrame,currframe,trigger->GetTriggerFrame());
						if ( (trigger->GetTriggerFrame() > m_prevAnimFrame && trigger->GetTriggerFrame() <= currframe)
							|| (m_prevAnimFrame > currframe && trigger->GetTriggerFrame() <= currframe) //covers looping anims
							)
						{
							trigger->apply(this);
						}
					}
					break;
				}
			}

			m_prevAnimFrame = currframe;
		} //if (ai != NULL && m_animController->GetSequence() > -1)
	}
}

const Vector3& WEntity::GetLocation() const
{
	return m_pos;
}

void WEntity::SetLocation(const Vector3& newLocation)
{
	//update collision model
	if (m_model != NULL)
	{
		m_model->TransformBy(newLocation - m_pos);
	}

	if (m_pSkeleton != NULL)
	{
		m_pSkeleton->TransformBy( newLocation - m_pos );
	}

	m_pos = newLocation;

	UpdateRenderTransform();
}

void WEntity::SetDrawScale(const Vector3& newScale)
{
	m_vScale = newScale;
	UpdateRenderTransform();
}

void WEntity::ResetCollision()
{
	if ( m_pLevel != NULL && m_pMeshInstance != NULL )
	{
		//if we had a physics object, release it
		if (m_physicsObject.IsValid())
		{
			m_pLevel->ReleasePhysicsObject(m_physicsObject);
		}

		//Create a collision model
		MotionType motion = CreateCollisionModel();

		//Add to physics now that we have a new model, check for Level
		//editor might be instancing us so its valid not to have a Level
		if ( m_pLevel != NULL )
			m_pLevel->GetPhysicsObject(this, motion);

		//upload materials to render thread
		EntityMaterialsUpdateEvent* materialEvent = new EntityMaterialsUpdateEvent(m_id);
		RenderDeviceMap map;

		//Materials, atm we assume these don't change. So we just really only need
		//to fire these off when our meshinstance changes
		size_t numMats = m_pMeshInstance->GetNumMaterials();
		materialEvent->m_materials.reserve(numMats);

		for(size_t i = 0; i < numMats;i++)
		{
			Material* pMat = m_pMeshInstance->GetMaterial(i);
			if (pMat != NULL)
			{
				materialEvent->m_materials.push_back(pMat->GetRuntimeID());

				ModelContainer* mc = m_pMeshInstance->GetModelResources(pMat);
				size_t num = mc->size();
				for(size_t j=0; j<num; j++)
				{
					ModelResource* resource = mc->get(j);

					//add additional info bout this resource...
					RenderDeviceObjectInfo info;
					info.mIsVisible = resource->IsVisible();
					info.box = resource->GetBox();
					info.mFuture = resource->GetRenderDeviceObject();
					info.matID = pMat->GetRuntimeID();

					map[pMat->GetRuntimeID().GetUInt64()].add(info);
				}
			}
		}

		materialEvent->mMap = map;

		//fire off the event
		m_pLevel->AddObjectStateChangeEvent(materialEvent);
	}
}

atomic_ptr<Mesh> WEntity::GetMesh()
{
	return mesh;
}

void WEntity::SetMesh(atomic_ptr<Mesh>& NewMesh)
{
	mesh=NewMesh;

	//by default, we just grab the default model
	m_pMeshInstance = NewMesh->GetDefaultInstance();

	if (m_pSkeleton != NULL)
	{
		delete m_pSkeleton;
	}

	m_pSkeleton = m_pMeshInstance->GetSkeleton();
	m_pSkeleton->SetInitTransform(m_pos, m_orientation);

	AnimationInstance* animInstance = NewMesh->GetAnimationInstance();
	if (animInstance != NULL)
	{
		if (m_animController != NULL)
		{
			delete m_animController;
			m_animController=NULL;
		}

		//Setup an animation controller that allows this entity to specify
		//the animation it wishes to present.
		if (m_animController == NULL)
		{
			m_animController = new AnimController();
			m_animController->SetEntity(this);
		}

		godzassert(m_animController != NULL);
		m_animController->SetAnimationInstance(animInstance);

		//Play a default animation, this way the bones will be set to the default pose and
		//the mesh will render...
		m_animController->PlayAnim(0);
	}
	else
	{
		if (m_animController)
		{
			m_animController->SetAnimationInstance(NULL);
		}
	}

	ResetCollision();

	//now that our visuals have changed, we need to make sure we upload visibility information
	//on the next pass
	m_visualsHasChanged = true;

	OnMeshChanged();
}

void WEntity::OnMeshChanged()
{
}

void WEntity::SetVisible(bool isVisible)
{
	m_bVisible = isVisible;

	//now that our visuals have changed, we need to make sure we upload visibility information
	//on the next pass
	m_visualsHasChanged = true;
}

void WEntity::SetIgnoreOutOfWorld(bool bIgnoreOutOfWorld)
{
	this->m_bIgnoreOutOfBounds=bIgnoreOutOfWorld;
}

void WEntity::SetVelocity(const Vector3& vel)
{
	//just tell the proxy, we derive our actual velocity from it
	if (m_physicsObject.IsValid())
	{
		m_physicsObject.SetDesiredVelocity(vel);
	}
	else
	{
		m_vel = vel;
	}
}

const Vector3& WEntity::GetVelocity()
{
	return m_vel;
}

void WEntity::SetAngularVelocity(const AxisAngle& axisAng)
{
	if (m_physicsObject.IsValid())
	{
		m_physicsObject.SetAngularVelocity(axisAng);
	}
	else
	{
		m_angularVel = axisAng;
	}
}

void WEntity::Serialize(GDZArchive& ar)
{
	GenericObject::Serialize(ar);

	ArchiveVersion version(1,1);
	ar << version;
}

bool WEntity::UpdateSpatialLocation()
{
	return false;
}

void WEntity::AttachActor(WEntity* entity)
{
	AttachmentInfo info;
	info.entity = entity->GetWeakReference();

	//note: converting to a matrix3 since there is no Matrix3::Inverse() atm
	WMatrix orientationInverse(1);
	WMatrix orientation(GetOrientationMatrix());
	orientation.Inverse(orientationInverse);
	info.m_rot = orientationInverse * entity->GetOrientationMatrix();
	info.m_rel = entity->GetLocation() - m_pos;

	m_attachedActors.push_back(info);
}

WEntity* WEntity::GetAttachment(size_t index)
{ 
	godzassert(index < m_attachedActors.size());
	return m_attachedActors[index].entity.lock();
}

void WEntity::AddEntityComponent(atomic_ptr<EntityComponent> component)
{
	HashCodeID code = component->GetComponentName();
	m_components[code] = component;
}

bool WEntity::RemoveEntityComponent(EntityComponent* component)
{
	HashCodeID code = component->GetComponentName();
	ComponentMap::iterator iter = m_components.find(code);
	if (iter != m_components.end())
	{
		m_components.erase(iter);
		return true;
	}

	return false;
}

EntityComponent* WEntity::GetEntityComponent(HashCodeID hash)
{
	return m_components[hash];
}

void WEntity::SetTemplate(atomic_ptr<EntityTemplate> entitytemplate)
{
	m_template = entitytemplate;
	atomic_weak_ptr<Mesh> meshReference = entitytemplate->GetModel();
	atomic_ptr<Mesh> ptr = meshReference.lock();

	if (ptr != NULL)
	{
		SetMesh(ptr);
	}
}

EntityTemplate*	WEntity::GetTemplate()
{
	return m_template;
}

void WEntity::OnHit(float damage, WEntity* who)
{
}

} //namespace GODZ
