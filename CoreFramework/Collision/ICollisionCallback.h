/* ===========================================================
	Collision Callback Interface

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

#if !defined(__ICOLLISIONCALLBACK__)
#define __ICOLLISIONCALLBACK__

#include "CollisionPrimitive.h"

namespace GODZ
{
	class PhysicsObject;

	struct GODZ_API CollisionListData
	{
		float		nearestDistance;
		Vector3		intersectionPoint;
		Triangle	intersectionTriangle;
		bool		m_bFoundCollision;		//point in poly
	};



	class GODZ_API ICollisionCallback
	{
	public:
		//returns true if the collision routine should abort search. This query is called
		//during collision searches (hit testing)
		virtual void OnCollisionOccured(CollisionResult& result) = 0;
		virtual bool getEarlyBail() = 0;
		virtual PhysicsWorld* getPhysicsWorld() = 0;
		virtual ~ICollisionCallback() {}
	};

	class GODZ_API ICollisionTraceCallback
	{
	public:	
		//returns true if the ray trace routine should abort search. This query is called
		//during ray traces through the world
		virtual bool OnRayHit(TraceResult& result) = 0;
		virtual ~ICollisionTraceCallback() {}
	};
}

#endif //__ICOLLISIONCALLBACK__