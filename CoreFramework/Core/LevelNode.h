/* ===========================================================
	LevelNode
	
	Encapsulates the Game World

	Created Jan 3, '04 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	========================================================== 
*/

#if !defined(_LEVEL_NODE_H_)
#define _LEVEL_NODE_H_

#include "GenericObject.h"
#include "GenericObjectList.h"
#include "Timer.h"
#include "PerformanceTracker.h"
#include <CoreFramework/Collision/PhysicsWorld.h>
#include "GizmoAxis.h"
#include "EntityPass.h"
#include <hash_map>
#include <queue>
#include "atomic_ref.h"
#include "SlotIndex.h"
#include "WEntity.h"
#include <CoreFramework/Math/WRay.h>
#include <CoreFramework/Math/Vector3.h>
#include <CoreFramework/Math/ViewFrustum.h>
#include <CoreFramework/Gameplay/WPlayerController.h>

namespace GODZ
{
	//Forward Declarations
	class Layer;
	class WDynamicLight;
	class WCamera;
	class ObjectStateEventCollection;
	class ObjectStateChangeEvent;

	// Stores all the entities within the Level.
	class GODZ_API LevelEntityData
	{
	public:
		EntityID AddActor(const atomic_ptr<WEntity>& entity_ref);
		atomic_ptr<WEntity>& FindActor(const EntityID& id);
		void FreeActor(const EntityID& id);

	private:
		struct EntityRef
		{
			atomic_ptr<WEntity> mPtr;
			UInt32 mInstanceCount;
		};

		typedef std::vector<EntityRef> EntityList;
		EntityList m_actors;
		std::queue<UInt32> mFreeList; 
	};

	class GODZ_API LevelNode
		: public GenericObject
	{
		DeclareGeneric(LevelNode, GenericObject)

	public:
		LevelNode();
		~LevelNode();

		Layer*					AddLayer(HString layerName);
		void					UnloadLayer(HashCodeID index);

		Layer*					GetLayer(HashCodeID index);
		size_t					GetNumLayers();
		Layer*					GetLayerAtIndex(size_t index);

		void					GetRenderElements(std::vector<EntityPassList>& elements, WCamera* camera, ViewFrustum& lightFrustum, VisibilityChangeInfo& status);

		//Clears the level (removes all actors, meshes, and resources from memory)
		void					Clear();
		void					Shutdown();

		//Returns true if Tick all entities on the level.
		//This function should be called once per frame.
		bool					DoTick(float dt, ObjectStateEventCollection* logicPass);

		//Returns the events that occured during this tick or NULL when outside of a game tick
		ObjectStateEventCollection* GetLogicPass() { return mGamePass; }

		void					AddController(atomic_ptr<WController> ptr);

		//Returns all actors that belong to the class type.
		void					GetActors(GenericClass* Class, GenericObjectList& list);
		atomic_weak_ptr<WEntity> FindActor(const EntityID& id);

		//Encapsulates data about this level
		const Vector3&			GetGravity() { return m_gravity; }
		void					SetGravity(const Vector3& v) { m_gravity = v; }

		//get time since level start
		ulong					GetLevelSeconds();

		virtual bool			ReleaseEntity(WEntity* entity);

		//Sets how often entities should be updated (game state update rate)
		void					SetTimeDilation(float newdt);

		void					SetCamera(WCamera* active);
		WCamera*				GetActiveCamera();

		void					SetClientPlayer(atomic_ptr<WPlayerController>& player);
		WPlayerController*		GetClient();

		//Editor stuff -------------------------->
		//Checks to see if this ray intersects with an entity... Returns closest hit
		WEntity*				PickEntity(const WRay& ray);
		GizmoAxis				PickAxis(const WRay& ray);
		void					DeselectEntity(WEntity* actor);
		void					SelectEntity(WEntity* actor);
		void					SelectAxis(GizmoAxis axis);

		//Physics -------------------------->
		void					BeginSimulation(const WBox& box);
		void					GetPhysicsObject(WEntity* entity, MotionType type);
		void					ReleasePhysicsObject(PhysicsObjectProxy& proxy);
		void					Sweep(Future<CollisionList> list, CollisionPrimitive* primitive, const Vector3& vel, PhysicsObjectProxy* ignore = NULL);
		void					Trace(Future<TraceResults> result, const WRay& ray, PhysicsObject* ignore, bool sortResults = false);
		

		//Adds this event so it can be batched together into the game logic pass. Events in which their order is 
		//important or depends upon visibility for this frame, etc should be added here during a game tick
		void AddObjectStateChangeEvent(ObjectStateChangeEvent* event);

	private:
		// All of the entities that exist within the world
		LevelEntityData			m_EntityData;

		//stores events that will be sent out during next game logic pass...
		std::vector<ObjectStateChangeEvent*> m_eventQueue;

		typedef stdext::hash_map<HashCodeID, Layer*> LayerMap;
		LayerMap m_regions;

		//kept around for tools / editor; LayerMap is really the main layer-collection
		std::vector<Layer*> mLayers;

		// List of Bots & Player client
		typedef std::vector<atomic_ptr<WController> > ControllerList;
		ControllerList m_controllers;

		float					m_counter;
		ulong					m_lifespan;					//duration this level has been loaded measured in time dilation (1 tick per cycle)
		float					fTimeDilation;				//when to update entities
		Timer					m_pTimer;
		float					m_fElapsedTime;
		TraceResult				m_TraceResult;
		atomic_weak_ptr<WPlayerController>		m_player;
		WCamera*				m_camera;					//camera

		Vector3					m_gravity;					//default gravity force

		//performance tracking
		PerformanceTracker		m_profileGameLoop;

		PhysicsWorld			m_world;					//physics simulation

		ObjectStateEventCollection* mGamePass;

		//current entity user has selected (editor stuff)
		WEntity* m_selectedEntity;
		Gizmo mGizmo;
		bool mIsShutdown;
	};
}

#endif
