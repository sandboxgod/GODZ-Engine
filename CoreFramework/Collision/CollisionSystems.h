/* ===================================================================
	Defines collision routines

	Sources Cited:

	[1] Elastic Collision in Three Dimensions by Thomas Simd
	http://www.plasmaphysics.org.uk/programs/coll3d_cpp.htm

	[2] Simple Intersection Tests for Games by Miguel Gomez
	http://www.gamasutra.com/features/19991018/Gomez_1.htm 

	[3] Real-Time Collision Detection and Response with AABB

	[4] Swept Sphere vs Triangles
	http://www.members.lycos.co.uk/olivierrenault/
	http://www.gamedev.net/community/forums/topic.asp?topic_id=192562

	Created Mar 25, 2003 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	==================================================================
*/

#if !defined(_COLLISION_SYSTEMS_H_)
#define _COLLISION_SYSTEMS_H_

#include "CollisionResult.h"
#include "RayCollider.h"
#include "PhysicsObjectProxy.h"
#include <CoreFramework/Core/GodzAtomics.h>
#include <CoreFramework/Core/SlotIndex.h>
#include <CoreFramework/Math/WPlane.h>
#include <CoreFramework/Math/WBox.h>
#include <vector>


namespace GODZ
{
	//forward declar
	class SynchronizedCollisionList;


	/*
	* Collision result
	*/
	struct GODZ_API PhysicsCollisionResult
	{
		PhysicsCollisionResult(const CollisionResult& r);

		Vector3 m_hitLocation;		//point of collision
		Vector3 m_normal;			//normal of the plane we collided with
		EntityID m_id;				//cached id of the entity we collided with
	};

	class GODZ_API PhysicsCollisionResults
	{
		friend class SynchronizedCollisionList;

	public:
		void AddCollision(const PhysicsCollisionResult& result);
		size_t Size();
		PhysicsCollisionResult& GetResult(size_t index);
		void Clear();

	private:
		std::vector<PhysicsCollisionResult> m_results;
	};

	/*
	* Thread safe container; collection of collision results
	*/
	class GODZ_API SynchronizedCollisionList
	{
	public:
		void AddCollision(const PhysicsCollisionResult& result);
		size_t Size();
		void GetResult(size_t i, PhysicsCollisionResult& result);
		void Clear();

		//returns a copy of the internal list
		void GetCopy(PhysicsCollisionResults& list);

	private:
		GodzAtomic m_collisionListLock;
		std::vector<PhysicsCollisionResult> m_results;
	};

	// collision result
	struct GODZ_API TraceResult
	{
		Vector3 hitLocation;		//point whereas penetration occured
		PhysicsObject* m_pHit;		//object that was hit
		Vector3 m_normal;			//normal of the face that was hit
		float m_distance;

		TraceResult()
			: m_pHit(NULL)
		{
		}

		//interface invoked by sort routines (see Sort.h)...
		float getValue()
		{
			return m_distance;
		}
	};

	class GODZ_API TraceResults
	{
	public:
		void AddResult(const TraceResult& result);

		//attempts to sort the result
		void InsertResult(const TraceResult& result);

		size_t Size() { return m_results.size(); }

		TraceResult& GetResult(size_t i);

		void Sort();


	private:
		std::vector<TraceResult> m_results;
	};
}

#endif