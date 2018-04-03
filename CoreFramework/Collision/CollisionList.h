/* ===========================================================
	Collision List

	Romans 8:28 'More Than Conquerors'
	"And we know that in all things God works for the good of
	those who love him, who have been called according to his
	pupose."
	
	Romans 8:31
	"What, then, shall we say in response to this? If God is
	for us, who can be against us?"

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__COLLISIONLIST__)
#define __COLLISIONLIST__

#include "CollisionPrimitive.h"
#include "ICollisionCallback.h"
#include <CoreFramework/Core/GenericNode.h>
#include <vector>


namespace GODZ
{
	class PhysicsObject;
	class PhysicsWorld;

	class GODZ_API CollisionList : public ICollisionCallback
	{
	public:	
		CollisionList();

		virtual ~CollisionList();

		//returns true if the collision routine should abort search. This query is called
		//during collision searches (hit testing)
		virtual void					OnCollisionOccured(CollisionResult& result);

		//returns the result that was the closest
		CollisionResult*				GetClosestResult();

		size_t							GetNumResults();

		//bail when first poly is hit
		void							SetBailOnContact(bool bailOut) { m_bBailOnFirstContact = bailOut; }
		virtual bool getEarlyBail() { return m_bBailOnFirstContact; }

		void setPhysicsWorld(PhysicsWorld* world) { m_world = world; }
		PhysicsWorld* getPhysicsWorld() { return m_world; }

		/*
		virtual void setPhysicsObjects(PhysicsObject* p1, PhysicsObject* p2);
		virtual PhysicsObject* getPhysicsObject1(void);
		virtual PhysicsObject* getPhysicsObject2(void);
		*/

		//Accesses the member at the array index. 
		CollisionResult& operator[](size_t index);

	protected:
		static const int				MAX_COLL_POLYS = 12;
		std::vector<CollisionResult>	m_hitResults;
		bool							m_bBailOnFirstContact;
		PhysicsWorld*					m_world;
		//PhysicsObject*					m_objectA;
		//PhysicsObject*					m_objectB;
	};

}

#endif //__COLLISIONLIST__