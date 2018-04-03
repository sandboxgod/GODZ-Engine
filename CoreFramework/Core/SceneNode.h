/* ===========================================================
	Hierarchial Scene Graph Node

	"When a man's ways are pleasing to the Lord, he makes even
	his enemies live at peace with him." - Proverbs 16:7

	Created Jan 2, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_SCENENODE_H_)
#define _SCENENODE_H_

#include "Godz.h"
#include "SceneInfo.h"
#include "RenderQueue.h"
#include "GenericNode.h"

namespace GODZ
{
	//Forward Declarations
	class Brush;
	class WCamera;
	class ICollisionCallback;
	class PhysicsObject;
	class ModelResource;
	class CollisionPrimitive;

	//Scene Node Types
	enum ESceneNodeFlags
	{
		SNF_Spatial,	//Spatial Sorting Node
		SNF_Divider,	//Stores static geometry
		SNF_Root		//This is a special root node (not intended to partition the scene)
	};

	GODZ_API SceneNode* CreateScene(ESceneNodeFlags sceneFlags, const WBox& Box);

	class GODZ_API SceneNode
	{
	public:
		virtual ~SceneNode();

		//Returns true if the Collision Interface invoked an early exit from the collision query
		bool					SweepHit(CollisionPrimitive* cA, const Vector3& velocity, CollisionResult& result, ICollisionCallback* pCallback, PhysicsObject* physicsObject);
		
		//Adds the entity to the scene graph.
		virtual bool			AddEntity(PhysicsObject* entity);
		
		// [in] entity that needs to be released from this node.
		virtual bool			ReleaseEntity(PhysicsObject* entity);

		bool					OnEntityMoved(PhysicsObject* entity);

		//Adds a node to this scene node
		void					AddNode(SceneNode* node);

		//Adds a node to this scene node
		SceneNode*				AddScene(ESceneNodeFlags flags, const WBox& box);

		//drop all the contents
		virtual void			Clear();

		//Returns true if this scene contains the entity.
		bool					ContainsEntity(PhysicsObject* entity);

		virtual bool			IsEntityElgible(PhysicsObject* entity) = 0;

		//Queries the entire scene graph to determine the number of nodes storing
		//the entity.
		void					ContainsEntityWithinGraph(PhysicsObject* entity, udword& count);

		// Returns all actors contained within the region.
		// [out] RelevantActors stores all actors who are relevant to the camera scene.
		void					GetActors(std::vector<PhysicsObject*>& RelevantActors);

		//Returns the number of child scene nodes.
		size_t					GetChildCount();

		//returns child scene nodes
		std::vector<SceneNode*>& GetChildren() { return children; }

		//Returns the scene node type.
		ulong					GetFlags();

		//Returns the visual representation of this node.
		ModelResource*			GetModelResource();

		//Returns the parent of this node.
		SceneNode*				GetParent();

		//Returns the root scene node.
		SceneNode*				GetRoot();

		//Searches this node for the desired node type.
		SceneNode*				FindNode(ESceneNodeFlags NodeType);

		void					RenderChildren(SceneInfo& sceneInfo);

		//Serialize this scene
		virtual void			Serialize(GDZArchive& Ar);

		//Returns the scene element hit by the ray trace (if true)
		//[in] ray - trace data
		//[out] rayTrace - collision package containing trace result
		//[in] ignore - entity/object that needs to be ignored by the trace
		//[in] bIgnoreBlockTrace - ray test the actor even if it does not block ray traces
		void					Trace(const WRay &ray, TraceResults& rayTrace, PhysicsObject* ent, bool sortResults = false);

		void					SetParent(SceneNode* parent);

		//Tells the Octree to remain "Static". Do not expand if a new entity is added. This way we do not
		//have to make all access to child nodes thread safe (performance). This should always be called
		//after all the entities have been added to this octant
		void					SetExpansionMode(bool bExpand) { m_bCanExpand = bExpand; }

		const WBox&				GetBounds(void) { return m_box; }
		void					SetBounds(const WBox& box) { m_box = box; }

	protected:
		WBox m_box;
		std::vector<PhysicsObject*>	m_objects;
		SceneNode *					parent;					//parent node
		std::vector<SceneNode *>	children;				//child nodes
		ulong						NodeFlags;				//node type (see ESceneNodeFlags)
		GenericReference<ModelResource>	modelResource;			//Model associated with this node (for debugging)

		bool						m_bCanExpand;			//tells the octree to basically remain "static". Do not expand during gameplay

		//constructor
		SceneNode();

		void						StoreElement(PhysicsObject* pEnt);

		//shoots a ray through this scene node and all child nodes. When a hit is detected,
		//it's appended to the collision package array.
		void						TraceScene(const WRay &ray, TraceResults& hitResults, PhysicsObject* ignore);
	};
}

#endif