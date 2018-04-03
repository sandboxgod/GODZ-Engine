
#include "LevelNode.h"
#include <float.h>
#include "Timer.h"
#include "WCamera.h"
#include "Layer.h"
#include "SceneData.h"
#include "EntityEvents.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Gameplay/WPlayerController.h>
#include <CoreFramework/Animation/Skeleton.h>
#include <CoreFramework/Core/Mesh.h>

namespace GODZ
{


ImplementGeneric(LevelNode)


////////////////////////////////////////////////////////////////////////////////////////

LevelNode::LevelNode()
: m_lifespan(0)
, fTimeDilation(0.01f) //be careful setting this attribute a lot of input system related stuff can get messed up
, m_fElapsedTime(0)
, m_gravity(0.0f,-900.0f,0.0f)
, m_counter(0)
, m_camera(NULL)
, mGamePass(NULL)
, m_selectedEntity(NULL)
, mIsShutdown(false)
{
	m_pTimer.PerformanceTimer(TIMER_RESET);

	if (fTimeDilation > 0)
	{
		Log("Warning: fTimeDilation has been set to %f\n", fTimeDilation);
	}
}

LevelNode::~LevelNode()
{
}

void LevelNode::SetCamera(WCamera* active)
{
	m_camera = active;

	CameraActivateEvent* event = new CameraActivateEvent(m_camera->GetCameraID());
	AddObjectStateChangeEvent(event);
}

void LevelNode::Shutdown()
{
	m_world.Exit();
	Clear();
}

void LevelNode::Clear()
{
	//drop all layers?
	LayerMap::iterator iter;
	for(iter = m_regions.begin(); iter != m_regions.end(); iter++)
	{
		Layer* p = iter->second;
		p->Exit();
		delete p;
	}

	m_regions.clear();
	mLayers.clear();
	m_player.reset();
	m_controllers.clear();
}

void LevelNode::AddObjectStateChangeEvent(ObjectStateChangeEvent* event)
{
	if (mGamePass != NULL)
	{
		//push events to the game logic pass, this way camera changes, etc can be kept in-sync
		//with the character it's trying to focus on....
		mGamePass->mList.Add(event);
	}
	else if (mIsShutdown)
	{
		// Free the event, the level is shutting down
		delete event;
	}
	else
	{
		//Sometimes there can actually be a valid reason for an entity to submit outside the pass,
		//so we go ahead and pass those events on....
		//SceneData::AddObjectStateChangeEvent(event);

		//Sometimes there can actually be a valid reason for an entity to submit outside the pass.
		//So what we do is queue these events and add those to the next pass....
		m_eventQueue.push_back(event);
	}
}

bool LevelNode::DoTick(float fElapsedTime, ObjectStateEventCollection* logicPass)
{
	//tracks time for ontick()
	m_counter += fElapsedTime;
	
	bool bDoTick = false;
		
	//if its time to tick actors....
	if (m_counter >= fTimeDilation || m_lifespan == 0)
	{
#ifdef _LEVELNODE_PROFILER_
		m_profileGameLoop.profilerBeginTime();
#endif
		mGamePass = logicPass;

		size_t numEvents = m_eventQueue.size();
		for(size_t i = 0; i < numEvents; i++)
		{
			logicPass->mList.Add(m_eventQueue[i]);
		}
		m_eventQueue.clear();

		bDoTick=true;
		m_fElapsedTime=m_counter;

		LayerMap::iterator iter;
		for(iter = m_regions.begin(); iter != m_regions.end(); iter++)
		{
			Layer* p = iter->second;
			p->DoTick(m_counter);
		}

		//tick the camera; we update him last just incase he needs to look at the Player, etc.
		//Otherwise we'll see some bad frame pops, etc
		if (m_camera != NULL)
		{
			m_camera->OnCameraTick(m_counter);
		}

		m_counter=0;
		m_lifespan++;

		mGamePass = NULL;

#ifdef _LEVELNODE_PROFILER_
		m_profileGameLoop.profilerEndTime("LevelNode::Tick()");
#endif _LEVELNODE_PROFILER_
	}

	return bDoTick;

}

WEntity* LevelNode::PickEntity(const WRay& ray)
{
	WEntity* best = NULL;
	float bestDist = -FLT_MAX;

	LayerMap::iterator iter;
	for(iter = m_regions.begin(); iter != m_regions.end(); iter++)
	{
		Layer* p = iter->second;
		best = p->PickEntity(ray, bestDist, best);
	}

	return best;
}

void LevelNode::GetActors(GenericClass* gc, GenericObjectList& list)
{
	LayerMap::iterator iter;
	for(iter = m_regions.begin(); iter != m_regions.end(); iter++)
	{
		Layer* p = iter->second;
		p->GetActors(gc, list);
	}
}

WCamera* LevelNode::GetActiveCamera()
{
	return m_camera;
}

ulong LevelNode::GetLevelSeconds()
{
	return m_lifespan;
}


bool LevelNode::ReleaseEntity(WEntity *entity)
{
	bool found = false;

	//remove the phys object from the simulation....
	PhysicsObjectProxy* proxy = entity->GetPhysicsObject();
	if (proxy != NULL)
	{
		m_world.RemovePhysicsObject(*proxy);
	}

	LayerMap::iterator iter;
	for(iter = m_regions.begin(); iter != m_regions.end(); iter++)
	{
		Layer* p = iter->second;
		if (p->ReleaseEntity(entity))
		{
			entity->OnRemoved();
			m_EntityData.FreeActor(entity->GetEntityID());
			found = true;
			break;
		}
	}

	return found;
}


void LevelNode::SetTimeDilation(float newdt)
{
	fTimeDilation=newdt;
}

//creates a phys object for the entity
void LevelNode::GetPhysicsObject(WEntity* entity, MotionType type)
{
	CollisionPrimitive* p = entity->GetCollisionModel();
	if (p != NULL && entity->GetCanCollideWithWorld())
	{
		PhysicsObjectCreationData data;
		data.m_id = entity->GetEntityID();
		data.m_motionType = type;
		data.m_primitive=p;

		//Compute origin location...
		data.m_origin = entity->GetMesh()->GetBounds().GetCenter();

		PhysicsObjectProxy rootProxy = m_world.CreatePhysicsObject(entity->GetLocation(), entity->GetVelocity(), data);
		entity->SetPhysicsObject(rootProxy);

		//add child nodes to the physics simulation...
		Skeleton* sk = entity->GetSkeleton();
		if (sk != NULL)
		{
			std::vector<SkeletonNode*> primitives;
			sk->GetSkeletonNodes( primitives );

			for (size_t i = 0; i < primitives.size(); i++)
			{
				data.m_motionType = primitives[i]->m_motionType;
				data.m_primitive = primitives[i]->m_primitive;

				//Provide the physics world with our pos / rot in object space
				//relative to our parent....
				primitives[i]->m_proxy = m_world.CreatePhysicsObject(primitives[i]->m_offset, ZERO_VECTOR, data);

				//Tell the parent proxy about his child
				rootProxy.AddChild(primitives[i]->m_proxy);
			}
		}

		m_world.AddPhysicsObject(rootProxy);
	}
}

void LevelNode::ReleasePhysicsObject(PhysicsObjectProxy& proxy)
{
	m_world.RemovePhysicsObject(proxy);
}

void LevelNode::Sweep(Future<CollisionList> list, CollisionPrimitive* primitive, const Vector3& vel, PhysicsObjectProxy* ignore)
{
	m_world.Sweep(list, primitive, vel, ignore);
}

void LevelNode::Trace(Future<TraceResults> result, const WRay& ray, PhysicsObject* ignore, bool sortResults)
{
	m_world.Trace(result, ray, ignore, sortResults);
}

//Starts the physics sim
void LevelNode::BeginSimulation(const WBox& box)
{
	m_world.Begin(box, m_gravity);
}

//Get all the elements within the view frustum
void LevelNode::GetRenderElements(std::vector<EntityPassList>& elements, WCamera* camera, ViewFrustum& lightFrustum, VisibilityChangeInfo& status)
{
	ViewFrustum& frustum = camera->GetViewFrustum();
	LayerMap::iterator iter;
	for(iter = m_regions.begin(); iter != m_regions.end(); iter++)
	{
		Layer* p = iter->second;
		p->GetRenderElements(elements, frustum, lightFrustum, status);
	}
}

//Adds a new, named layer
Layer* LevelNode::AddLayer(HString layerName)
{
	Layer* p = new Layer(layerName, this, &m_EntityData);
	m_regions[layerName] = p;
	mLayers.push_back(p);

	return p;
}

void LevelNode::UnloadLayer(HashCodeID index)
{
	//local scope
	{
		LayerMap::iterator iter = m_regions.find(index);
		if(iter != m_regions.end())
		{
			Layer* p = iter->second;
			m_regions.erase(iter);
		}
	}

	std::vector<Layer*>::iterator iter;
	for(iter=mLayers.begin();iter!=mLayers.end();iter++)
	{
		Layer* p = *iter;
		if (p->GetName() == index)
		{
			p->Exit();
			mLayers.erase(iter);
			delete p;
			return;
		}
	}
}


Layer* LevelNode::GetLayer(HashCodeID index)
{
	return m_regions[index];
}

size_t	LevelNode::GetNumLayers()
{
	return mLayers.size();
}

Layer* LevelNode::GetLayerAtIndex(size_t index)
{
	return mLayers[index];
}

void LevelNode::SetClientPlayer(atomic_ptr<WPlayerController>& player)
{
	m_player = player;
}

WPlayerController* LevelNode::GetClient()
{
	return m_player.lock();
}

void LevelNode::AddController(atomic_ptr<WController> controllerPtr)
{
	m_controllers.push_back(controllerPtr);
}

void LevelNode::SelectEntity(WEntity* actor)
{
	if (m_selectedEntity == actor)
	{
		return;
	}

	VisualID oldID;
	if (m_selectedEntity != NULL)
	{
		//publish a deselect event
		oldID = m_selectedEntity->GetRuntimeID();
	}

	m_selectedEntity = actor;

	//publish a selection event
	EntitySelectedEvent* selEvent = new EntitySelectedEvent(actor->GetRuntimeID(), oldID);
	AddObjectStateChangeEvent(selEvent);
}

void LevelNode::DeselectEntity(WEntity* actor)
{
	//publish a deselection event
	EntityDeselectedEvent* selEvent = new EntityDeselectedEvent(actor->GetRuntimeID());
	AddObjectStateChangeEvent(selEvent);

	if (m_selectedEntity == actor)
	{
		m_selectedEntity = NULL;
	}
}

void LevelNode::SelectAxis(GizmoAxis axis)
{
	//publish a selection event
	if (m_selectedEntity != NULL)
	{
		EntityAxisChangedEvent* axisChangedEvent = new EntityAxisChangedEvent(m_selectedEntity->GetRuntimeID(), axis);
		AddObjectStateChangeEvent(axisChangedEvent);
	}
}

GizmoAxis LevelNode::PickAxis(const WRay& ray)
{
	if (m_selectedEntity != NULL)
	{
		WMatrix tm(1);
		tm.SetTranslation(m_selectedEntity->GetLocation());

		GizmoAxis axis = mGizmo.GetAxis(ray, tm);

		EntityAxisChangedEvent* axisChangedEvent = new EntityAxisChangedEvent(m_selectedEntity->GetRuntimeID(), axis);
		AddObjectStateChangeEvent(axisChangedEvent);

		return axis;
	}

	return GizmoAxis_None;
}

atomic_weak_ptr<WEntity> LevelNode::FindActor(const EntityID& id)
{
	return m_EntityData.FindActor(id);
}


//////////////////////////////////////////////////////////////////////////////

EntityID LevelEntityData::AddActor(const atomic_ptr<WEntity>& entity)
{
	EntityID id;

	if (mFreeList.empty())
	{
		EntityRef ref;
		ref.mPtr = entity;
		ref.mInstanceCount = 1;
		m_actors.push_back(ref);

		id.mInstanceCount = ref.mInstanceCount;
		id.mSlotNumber = m_actors.size() - 1;
	}
	else
	{
		UInt32 slotNum = mFreeList.front();
		mFreeList.pop();
		m_actors[slotNum].mInstanceCount++;
		godzassert(m_actors[slotNum].mPtr == NULL);
		m_actors[slotNum].mPtr = entity;

		id.mInstanceCount = m_actors[slotNum].mInstanceCount;
		id.mSlotNumber = slotNum;
	}
	
	return id;
}

atomic_ptr<WEntity>& LevelEntityData::FindActor(const EntityID& id)
{
	godzassert(id.mSlotNumber < m_actors.size());

	// Make sure the instance number is still valid.
	if (id.mInstanceCount != m_actors[id.mSlotNumber].mInstanceCount)
	{
		// They are refering to an entity that no longer exists
		static atomic_ptr<WEntity> empty;
		return empty;
	}

	return m_actors[id.mSlotNumber].mPtr;
}

void LevelEntityData::FreeActor(const EntityID& id)
{
	godzassert(id.mSlotNumber < m_actors.size());

	// Make sure the instance number is still valid.
	if (id.mInstanceCount != m_actors[id.mSlotNumber].mInstanceCount)
	{
		return;
	}

	m_actors[id.mSlotNumber].mPtr = NULL;
}

} //namespace