/* ===========================================================

	Created Jan 16, '10 by Richard Osborne
	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__PHYSWORLD_H__)
#define __PHYSWORLD_H__

#include "Physics.h"
#include "CollisionSystems.h"
#include "CollisionList.h"
#include <vector>
#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/Job.h>
#include <CoreFramework/Core/Timer.h>
#include <CoreFramework/Core/GodzAtomics.h>
#include <CoreFramework/Math/WBox.h>


class btBroadphaseInterface;
class btCollisionWorld;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btVoronoiSimplexSolver;

namespace GODZ
{
	class AABBNode;
	class SceneNode;
	class CollisionPrimitive;
	class PhysicsObject;
	class PhysicsObjectProxy;

	struct GODZ_API CollisionPackage
	{
		Vector3				m_lastSafePosition;
		int					m_nQueries;				//num of queries ran
		CollisionStatus		m_collisionStatus;
		bool				m_collideAndSlide;
	};

	struct GODZ_API PhysicsObjectCreationData
	{
		MotionType m_motionType;
		EntityID m_id;								//Actor ID
		CollisionPrimitive* m_primitive;

		//center of the physics primitive in object space
		Vector3 m_origin;
	};

	/*
	* Runs the physics simulation in another thread
	*/
	class GODZ_API PhysicsWorld : public Job
	{
	public:

		PhysicsWorld();

		//Starts the physics sim (installs this job)
		void Begin(const WBox& Box, const Vector3& gravity);

		//Tells physics world to exit after it finishes its current update
		void Exit();

		//Called before the jobthread runs this for the 1st time
		virtual void Start();

		//Execute job specific code. Expect this code to be called only once unless declared as
		//'permenant'. Never put an infinite loop here. The job thread will keep calling this
		//if this job sets the permanent flag.
		virtual bool Run(float dt);

		//Called when this job needs to be stopped and perform cleanup
		virtual void Stop();

		//Returns true if this Job Type is 'unique' meaning it should be assigned it's own thread
		virtual bool isPermanent() { return true; }


		//Adds a synchronized physics object to the physics simulation and returns a proxy
		PhysicsObjectProxy CreatePhysicsObject(const Vector3& pos, const Vector3& vel, const PhysicsObjectCreationData& data);
		void AddPhysicsObject(PhysicsObjectProxy& proxy);

		//Removes the physics object from this physics simulation
		void RemovePhysicsObject(PhysicsObjectProxy& obj);

		//Shoots a ray through the physics world. The result will be stored into the future argument that's passed in
		void Trace(Future<TraceResults> result, const WRay& ray, PhysicsObject* ignore, bool sortResults = false);

		void Sweep(Future<CollisionList> list, CollisionPrimitive* primitive, const Vector3& vel, PhysicsObjectProxy* ignore = NULL);

		bool Collide(btConvexShape* shapeA, const Vector3& posA, const Vector3& velA, const Matrix3& initOrientationA, const Matrix3& destOrientationA, btConvexShape* shapeB, const Vector3& posB, const Vector3& velB, const Matrix3& initOrientationB, const Matrix3& destOrientationB, CollisionResult& result);

		//btVoronoiSimplexSolver* GetSimplexSolver() { return mSimplexSolver; }

	private:
		void SetBaseFound(PhysicsObject* moby);
		bool MoveActor(PhysicsObject* moby, const Vector3& origin, const Vector3& destination, Vector3& endPosition);
		bool CanRotateActor(PhysicsObject* moby, const Matrix3& destRot);
		bool IsModelStuck(const Vector3& position, PhysicsObject* moby);
		Vector3 CollideWithWorld(PhysicsObject* entity, const Vector3& origin, const Vector3& velocity, CollisionPackage& collData);
		void SubmitToScene(PhysicsObject* moby);
		void UpdateChildren(PhysicsObject* moby, const Vector3& velocity, bool isZeroVelocity);

		struct RayQuery
		{
			WRay mRay;
			Future<TraceResults> mTraceResults;
			bool mShouldSort;
			PhysicsObject* mIgnore;
		};

		struct SweepQuery
		{
			GenericReference<CollisionPrimitive> mPrimitive; //(needs to be freed) -- our local copy of the primitive
			Vector3 vel;
			Future<CollisionList> mResult;
			PhysicsObject* mIgnore;
		};

		bool mStarted;

		GodzAtomic mIsRunning;
		GodzAtomic mQueueLock;			//pending queue lock
		GodzAtomic mRemoveQueueLock;	//pending queue lock
		GodzAtomic mLock;				//mPhysicsObjects lock
		GodzAtomic mRayListLock;
		GodzAtomic mSweepListLock;
		SceneNode* mScene;
		
		std::vector<GenericReference<PhysicsObject> > mPhysicsObjects;		//all currently registered physics objects
		std::vector<GenericReference<PhysicsObject> > mAddQueue;			//queue of objects waiting to be added
		std::vector<PhysicsObject*> mRemoveQueue;			//queue of objects we need to drop
		std::vector<PhysicsObject*> mInternalRemoveQueue;	//internal, remove queue - not accessed by external callers
		std::vector< RayQuery > mRays;
		std::vector< SweepQuery > mSweeps;

		long mFrameNum;
		Vector3 m_gravity;

		//-----------------------------------------------------------
		//Bullet Physics stuff

		btCollisionWorld* m_world;		//Physics World
		btBroadphaseInterface* m_broadphaseInterface;
		btCollisionDispatcher* m_dispatch;
		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btVoronoiSimplexSolver* mSimplexSolver;
	};
}

#endif //__PHYSWORLD_H__
