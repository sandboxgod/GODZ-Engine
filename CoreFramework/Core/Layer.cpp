
#include "Layer.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Math/WBox.h>
#include "WCamera.h"
#include "GenericPackage.h"
#include "GenericObjData.h"
#include "PackageWriter.h"
#include "PackageReader.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Material.h"
#include "SceneManager.h"
#include "WSunLight.h"


namespace GODZ
{

Layer::Layer(HString name, LevelNode* level, LevelEntityData* data)
: mName(name)
, mLevel(level)
, m_package(NULL)
, mEntityData(data)
{
}

Layer::~Layer()
{

}

void Layer::Exit()
{
	size_t num = m_actors.size();
	for(size_t i=0;i<num;i++)
	{
		WEntity* ptr = mEntityData->FindActor(m_actors[i]);
		if (ptr != NULL)
		{
			ptr->OnRemoved();
			mEntityData->FreeActor(m_actors[i]);
		}
	}
}

void Layer::AddActor(const atomic_ptr<WEntity>& entity)
{
	EntityID id = mEntityData->AddActor(entity);
	m_actors.push_back(id);
	entity->SetLevel(mLevel);
	entity->SetLayer(this, entity, id);

	entity->OnAddToWorld();
}

size_t Layer::GetNumActors()
{
	return m_actors.size();
}

WEntity* Layer::GetActor(size_t index)
{
	godzassert(index < m_actors.size());
	return mEntityData->FindActor(m_actors[index]);
}

//Get all the elements within the view frustum
void Layer::GetRenderElements(std::vector<EntityPassList>& elements, ViewFrustum& frustum, ViewFrustum& lightFrustum, VisibilityChangeInfo& status)
{
	SceneManager* smgr = SceneManager::GetInstance();
	bool isEditor = (smgr->IsEditorMode());

	size_t num = m_actors.size();
	for(size_t i=0;i<num;i++)
	{
		WEntity* ptr = mEntityData->FindActor(m_actors[i]);

		if (ptr == NULL)
			continue;

		if (ptr->HasVisualsChanged())
		{
			//new visibility information needs to be sent over...
			status.m_requiresPass = true;
			ptr->ResetVisualChangedState();
		}

		//don't submit invisible actors
		if (!ptr->IsVisible() && !isEditor)
		{
			continue;
		}

		//Primitives should already be scaled properly...
		CollisionPrimitive* primitive = ptr->GetCollisionModel();

		WBox box;
		
		if (primitive != NULL)
		{
			primitive->GetBox(box);
		}
		else
		{
			//give the primitive some default scale (for editor icons)....
			const float scale = 10;
			box.SetMinMax(ptr->GetLocation() + Vector3(-scale,-scale,-scale),
				ptr->GetLocation() + Vector3(scale,scale,scale));
		}

		WSphere modelSphere = box;

		if (frustum.SphereInFrustum(modelSphere) != ViewFrustum::FrustumResult_OUT || lightFrustum.SphereInFrustum(modelSphere) != ViewFrustum::FrustumResult_OUT)
		{
			EntityPassList list;
			list.m_id = ptr->GetRuntimeID();

			Mesh* mesh = ptr->GetMesh();
			if (mesh == NULL)
			{
				//TODO: ugh, a static function call for the editor....
				if (isEditor)
				{
					//send over entities that qualify so we can render their icons!
					RenderDeviceBox rbox;
					rbox.mObj = NULL;

					rbox.box = box;

					// get Editor icon for this class
					GenericClass* gc = ptr->GetClass();
					Material* gm = gc->GetMaterial();
					if (gm != NULL)
					{
						rbox.matID = gm->GetRuntimeID();
						rbox.matIndex = 0;
						list.m_visibleResources.push_back(rbox);
						elements.push_back(list);
					}
				}

				continue;
			}

			//Determine exactly which resources are visible or not....
			MeshInstance* mi = ptr->GetMeshInstance();

			size_t numMats = mi->GetNumMaterials();
			for(size_t matIndex=0; matIndex<numMats; matIndex++)
			{
				Material* m = mi->GetMaterial(matIndex);
				ModelContainer* mc = mi->GetModelResources(m);

				size_t len = mc->size();
				for(size_t j=0; j<len; j++)
				{
					ModelResource* mr = mc->get(j);
					if (mr->IsVisible())
					{
						WBox resourceBox = mr->GetBox();
						
						//SCALE the ModelResource AABB (from model space) otherwise frustum test FAILS!
						Matrix4 actormat(1);
						actormat.SetScale(ptr->GetDrawScale());
						actormat.SetTranslation(ptr->GetLocation());
						resourceBox.Transform(actormat);

						WSphere s = resourceBox;

						if (frustum.SphereInFrustum(s) != ViewFrustum::FrustumResult_OUT || lightFrustum.SphereInFrustum(s) != ViewFrustum::FrustumResult_OUT)
						{
							Future<GenericReference<RenderDeviceObject> > future = mr->GetRenderDeviceObject();
							if (future.isReady())
							{
								RenderDeviceBox rbox;

								rbox.mObj = future.getValue();
								rbox.box = resourceBox;
								rbox.matID = m->GetRuntimeID();
								rbox.matIndex = matIndex;
								list.m_visibleResources.push_back(rbox);
							}
						}

						/*
						else
						{
							_asm nop; //debug objects that not render
						}
						*/
					}
				}
			}

			elements.push_back(list);
		}
	}
}

void Layer::DoTick(float dt)
{
	size_t size = m_actors.size();
	for(size_t i =0;i<size;i++)
	{
		WEntity* entity = mEntityData->FindActor(m_actors[i]);
		if (entity!=NULL)
		{
			entity->OnTick(dt);
			entity->UpdateTransform(dt); //client update
		}
	}
}

void Layer::GetActors(GenericClass* Class, GenericObjectList& list)
{
	size_t size=m_actors.size();
	for (ulong i = 0; i < size; i++)
	{
		WEntity* e = mEntityData->FindActor(m_actors[i]);
		if (e!=NULL && e->IsA(Class))
		{
			list.Add(e);
		}
	}
}

// Removes the EntityID from our internal list. We do not free the object
// itself though in this instance just in case the Level is trying to move
// this entity to another layer.
bool Layer::ReleaseEntity(WEntity* entity)
{
	EntityID id = entity->GetEntityID();
	EntityList::iterator iter;
	for(iter = m_actors.begin();iter != m_actors.end(); iter++)
	{
		EntityID tempId = *iter;
		if (tempId == id)
		{
			m_actors.erase(iter);
			return true;
		}
	}

	return false;
}

WEntity* Layer::PickEntity(const WRay& ray, float& bestDist, WEntity* best)
{
	size_t size = m_actors.size();
	for(size_t i =0;i<size;i++)
	{
		WEntity* entity = mEntityData->FindActor(m_actors[i]);

		if (entity != NULL && entity->GetCanCollideWithWorld())
		{
			CollisionPrimitive* p = entity->GetCollisionModel();

			TraceResult ret;
			if (p!= NULL && p->Trace(ray, ret))
			{
				float d = ret.m_distance;
				if (d > bestDist)
				{
					bestDist = d;
					best = entity;
				}
			}
		}
	}

	return best;
}

NavigationMesh&	Layer::GetNavigationMesh()
{
	if( m_navMeshInfo == NULL )
	{
		m_navMeshInfo = new NavMeshInfo();
	}

	return m_navMeshInfo->GetNavigationMesh();
}

atomic_ptr<WEntity> Layer::SpawnActor(GenericClass* EntityClass)
{	
	godzassert(EntityClass->IsA(WEntity::GetClass()));
	GenericObject* pObj = EntityClass->GetInstance();
	WEntity* entity = SafeCast<WEntity>(pObj);

	atomic_ptr<WEntity> ptr(entity);

	AddActor(ptr);

	return ptr;
}

atomic_ptr<WEntity> Layer::SpawnActor(GenericClass* EntityClass, const Vector3& pos, const Quaternion& rot)
{
	atomic_ptr<WEntity> entityPtr = SpawnActor(EntityClass);

	entityPtr->SetLocation(pos);

	Matrix3 m;
	QuaternionToMatrix(rot, m);
	entityPtr->SetOrientationMatrix(m);

	return entityPtr;
}

void Layer::SaveToPackage(const char* file)
{
	PackageWriter writer;
	writer.SavePackage(m_package, file);
}

//single threaded; waits til package loads
void Layer::LoadFromPackage(const char* filename)
{
	Future<GenericPackage*> p;
	if (SceneManager::GetInstance()->StreamPackage(filename, p))
	{
		//wait until it's ready!
		while ( !p.isReady() );
		LoadFromPackage( p.getValue() );
	}
}

void Layer::LoadFromPackage(GenericPackage* p)
{
	m_navMeshInfo = NULL;

	size_t num = p->GetNumObjects();
	for(size_t i=0;i<num;i++)
	{
		atomic_ptr<GenericObject> obj_ptr = p->GetReferenceAt(i);
		WEntity* actor = SafeCast<WEntity>( obj_ptr );
		if (actor != NULL)
		{
			atomic_ptr<WEntity> entity_ref = atomic_ptr_cast<WEntity>( obj_ptr );
			AddActor( entity_ref );
		}
		else if ( obj_ptr->IsA(NavMeshInfo::GetClass() ) )
		{
			m_navMeshInfo = atomic_ptr_cast<NavMeshInfo>(obj_ptr);
		}
	}
}

GenericPackage* Layer::GetPackage() { return m_package; }
void Layer::SetPackage(GenericPackage* p) { m_package = p; }

}