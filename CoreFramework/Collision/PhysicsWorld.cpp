
#include "PhysicsWorld.h"
#include "PhysicsObject.h"
#include "CollisionList.h"
#include <CoreFramework/Core/JobManager.h>
#include <CoreFramework/Core/SceneManager.h>
#include <CoreFramework/Core/OctreeSceneNode.h>
#include <CoreFramework/Math/vectormath.h>
#include <CoreFramework/Math/BulletMathUtil.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcher.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h>
#include <BulletCollision/NarrowPhaseCollision/btSubsimplexConvexCast.h>
#include <BulletCollision/NarrowPhaseCollision/btPointCollector.h>
#include <BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h>

//#define DEBUG_COLL_SLIDE 1

namespace GODZ
{

// =================================================================================================================================

//Notes:
//1. Physics thread updates the position of all entities.
//2. Main thread gets the updated position + octree position. If the collision octree is identical to render octree. we can
//   relay over the octree location of the entity as well.


PhysicsWorld::PhysicsWorld()
: mStarted(false)
, mScene(NULL)
, mFrameNum(0)
, m_world(NULL)
, m_broadphaseInterface(NULL)
, m_dispatch(NULL)
, mSimplexSolver(NULL)
, m_collisionConfiguration(NULL)
{
	mPhysicsObjects.reserve(50);
}


//called by main thread, starts the actual physics simulation
void PhysicsWorld::Begin(const WBox& Box, const Vector3& gravity)
{

	//don't add a new physics job if it's in the process of shutting down. Set this flag to signal this job is 
	//now started
	bool started = mIsRunning.Set();
	godzassert(started);

	//if ( (sceneFlags == SNF_Spatial))
	{
		mScene = new OctreeSceneNode(Box);
	}

	m_gravity = gravity;

	const Vector3& min = Box.GetMin();
	const Vector3& max = Box.GetMax();

	mSimplexSolver = new btVoronoiSimplexSolver();
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatch = new btCollisionDispatcher(m_collisionConfiguration);
	btVector3	worldAabbMin(min.x,min.y,min.z);
	btVector3	worldAabbMax(max.x,max.y,max.z);

	m_broadphaseInterface = new btAxisSweep3(worldAabbMin,worldAabbMax);
	
	//SimpleBroadphase is a brute force alternative, performing N^2 aabb overlap tests
	//SimpleBroadphase*	broadphase = new btSimpleBroadphase;

	m_world = new btCollisionWorld(m_dispatch,m_broadphaseInterface,m_collisionConfiguration);

	//Install this job on the current jobmngr
	SceneManager* scenemngr = SceneManager::GetInstance();
	JobManager& mngr = scenemngr->GetJobManager();

	mngr.addJob(this);
}

//called by main thread
void PhysicsWorld::Exit()
{
	SceneManager* scenemngr = SceneManager::GetInstance();
	JobManager& mngr = scenemngr->GetJobManager();

	mngr.removeJob(this);
}

//called by job thread before it tosses us
void PhysicsWorld::Stop()
{
	size_t num = mPhysicsObjects.size();
	for(size_t i = 0;i<num;i++)
	{
		mPhysicsObjects[i] = NULL;
	}
	mPhysicsObjects.clear();

	mAddQueue.clear();

	delete mScene;
	mScene = NULL;

	delete m_world;
	m_world = NULL;

	delete m_dispatch;
	m_dispatch = NULL;

	delete m_collisionConfiguration;
	m_collisionConfiguration= NULL;

	delete m_broadphaseInterface;
	m_broadphaseInterface = NULL;

	delete mSimplexSolver;
	mSimplexSolver = NULL;

	//Set flag to signal this job has finished
	mIsRunning.UnSet();
}

//called by Job thread
void PhysicsWorld::Start()
{
}

//called by job thread
bool PhysicsWorld::Run(float dt)
{
	//We do not want to process of dt of zero. Will make entities think they've stopped, etc
	//return true;

	mFrameNum++;

	//Transfer objects from the queue into the scene
	{
		SynchronizedBlock block(mQueueLock);
		size_t num = mAddQueue.size();
		for(size_t i = 0; i < num; i++)
		{
			SubmitToScene(mAddQueue[i]);

			size_t numKids = mAddQueue[i]->m_children.size();
			for(size_t j = 0; j < numKids; j++)
			{
				SubmitToScene(mAddQueue[i]->m_children[j]);
			}

			//only add root objects to the list
			mPhysicsObjects.push_back(mAddQueue[i]);
		}

		mAddQueue.clear();
	}


	size_t num = mPhysicsObjects.size();
	for(size_t i = 0; i < num; i++)
	{
		PhysicsObject* moby = mPhysicsObjects[i];

		long motion = moby->m_motionType;
		MotionType m = (MotionType)motion;

		if (m > MotionType_Static)
		{
			moby->SetBusy(true);

			//go ahead and grab all the current data now for this Moby; this way if main thread alters the data, we'll be unaffected....
			Vector3 origin;
			moby->m_pos.get(origin);

			Vector3 goalVelocity;
			moby->m_goalVel.get(goalVelocity);

			Matrix3 mobyRot;
			moby->m_rot.get(mobyRot);

			//Attempt to move the object here....
			Vector3 dest = origin + ((goalVelocity + m_gravity) * dt);

			//do collision stuff.....
			Vector3 endPosition;
			if ( MoveActor(moby, origin, dest, endPosition) )
			{
				/*
				Log("-----------------------------\n");
				Log("origin parent %f %f %f \n",origin.x, origin.y, origin.z);
				Log("endPosition %f %f %f \n",endPosition.x, endPosition.y, endPosition.z);
				*/

				//WMatrix tm(1);
				Vector3 actualVelocity;

				//if the moby had a position set during the physics update allow it
				//to override our comps...
				if (!moby->m_nextPositionLock.IsSet())
				{
					moby->m_pos = (endPosition);

					TransformState tm;
					tm.m_rot = mobyRot;
					tm.m_pos = endPosition;
					moby->m_worldTm = tm;

					//compute actual velocity...
					actualVelocity = (endPosition - origin);
					moby->m_vel = actualVelocity;

					//tm.SetTranslation(actualVelocity);
					moby->m_primitive->TransformBy(actualVelocity);

					if (moby->GetRegion() != NULL)
						moby->GetRegion()->OnEntityMoved(moby);
				}
				else
				{
					//another thread just manually set a moby's position
					Vector3 nextpos = moby->m_nextPos;
					Vector3 prevpos = moby->m_pos;
					moby->m_pos = nextpos;
					moby->m_nextPositionLock.UnSet();
					moby->m_vel = Vector3::Zero();

					TransformState tm;
					tm.m_rot = mobyRot;
					tm.m_pos = nextpos;
					moby->m_worldTm = tm;

					actualVelocity = nextpos - prevpos;
					
					//tm.SetTranslation(actualVelocity);
					moby->m_primitive->TransformBy(actualVelocity);

					if (moby->GetRegion() != NULL)
						moby->GetRegion()->OnEntityMoved(moby);
				}

				UpdateChildren(moby, actualVelocity, false);

				//update frame # only when there is a change (after child updates, etc)
				moby->m_frame = mFrameNum;

			} //if ( MoveActor )
			else
			{
				//moby->m_vel = ZERO_VECTOR;
				Vector3 vel = moby->m_vel;
				if (vel != ZERO_VECTOR)
				{
					moby->m_vel = ZERO_VECTOR;
					moby->m_frame = mFrameNum;

					UpdateChildren(moby, Vector3::Zero(), true);
				}
			}

			/*
			//compute rotation!
			AxisAngle goalAngularVelocity;
			moby->m_goalAngularVelocity.get(goalAngularVelocity);

			if (!Equals(goalAngularVelocity.angle, 0.0f))
			{
				Matrix3 offsetRot;
				float angle = goalAngularVelocity.angle * dt;
				Quaternion q(goalAngularVelocity.axis, angle);
				QuaternionToMatrix(q, offsetRot);
				offsetRot = mobyRot * offsetRot;
				

				//Now sweep the rotation. We do this seperately from position sweeps.
				//This way for cases whereas we both move + rotate, we don't cancel
				//out both motion types...

				//TODO: Remove this!
				if (CanRotateActor(moby, offsetRot))
				{
					//set the rotation...
					moby->m_rot = offsetRot;

					Vector3 pp = moby->m_pos;
					//Log("rot parent %f %f %f \n",pp.x, pp.y, pp.z);
					
					moby->m_primitive->SetTransform(pp, offsetRot);

					//We set pos / rot at sametime this way main thread, etc will get the
					//tm in the same frame....
					TransformState tm;
					tm.m_rot = offsetRot;
					tm.m_pos = pp;
					moby->m_worldTm = tm;

					//get parent's world space matrix
					WMatrix parentMatrix;
					moby->m_primitive->GetTransform(parentMatrix);

					//update children...
					size_t num = moby->m_children.size();
					for (size_t i = 0; i < num; i++)
					{
						PhysicsObject* po = moby->m_children[i];

						//update rotation relative to the parent...
						Vector3 p;
						po->m_pos.get(p);

						offsetRot.Mul(p);

						WMatrix objectTm(1);

						Matrix3 rot;
						po->m_rot.get(rot);
						CopyMatrix(rot, objectTm);
						objectTm.SetTranslation( p );

						WMatrix final = parentMatrix * objectTm;
						Matrix3 final3(final);

						Vector3 cp = final.GetTranslation();

						//Log("skeletal node child %f %f %f \n",cp.x, cp.y, cp.z);

						TransformState tm;
						tm.m_rot = final3;
						tm.m_pos = cp;
						po->m_worldTm = tm;

						//Rotate this child's primitive (in world space)
						po->m_primitive->SetTransform( cp, final3 );

						if (po->GetRegion() != NULL)
							po->GetRegion()->OnEntityMoved(po);

						moby->m_rotFrame = mFrameNum;
					} // if (CanRotateActor(moby, offsetRot))
				}
			} // if (!Equals(goalAngularVelocity.angle, 0.0f))
			*/
			
			moby->SetBusy(false);
			moby->UpdateCollisionObject();
		} //!static
		else if (m == MotionType_Static)
		{
			Vector3 p = moby->m_pos;
			if (moby->m_nextPositionLock.IsSet())
			{
				moby->m_pos = moby->m_nextPos;
				moby->m_nextPositionLock.UnSet();
			}

			Vector3 curr = moby->m_pos;
			WMatrix tm(1);
			tm.SetTranslation(curr - p);
			moby->GetCollisionModel()->TransformBy(tm);
		}
	}

	std::vector<PhysicsObject*> removequeue;

	//remove physics objects waiting to be dropped
	{
		SynchronizedBlock lock(mRemoveQueueLock);
		removequeue.reserve(mRemoveQueue.size());
		removequeue = mRemoveQueue;
		mRemoveQueue.clear();
	}

	size_t numdead = removequeue.size();
	for(size_t i=0;i<numdead;i++)
	{
		std::vector<GenericReference<PhysicsObject> >::iterator iter;
		for(iter = mPhysicsObjects.begin(); iter != mPhysicsObjects.end();iter++)
		{
			PhysicsObject* p = *iter;
			if (p == removequeue[i])
			{
				mScene->ReleaseEntity(p);
				p->m_world = NULL;
				mPhysicsObjects.erase(iter);
				break;
			}
		}
	}

	//check the special internal queue -- private to this just thread. Caches off objects that queued themselves
	//for deletion during the physics update
	numdead = mInternalRemoveQueue.size();
	for(size_t i=0;i<numdead;i++)
	{
		std::vector<GenericReference<PhysicsObject> >::iterator iter;
		for(iter = mPhysicsObjects.begin(); iter != mPhysicsObjects.end();iter++)
		{
			PhysicsObject* p = *iter;
			if (p == mInternalRemoveQueue[i])
			{
				mPhysicsObjects.erase(iter);
				break;
			}
		}
	}
	mInternalRemoveQueue.clear();

	//Now shoot the rays, gather results....
	std::vector<RayQuery> rays;

	//ray traces
	{
		SynchronizedBlock block(mRayListLock);
		rays.reserve(mRays.size());
		rays = mRays;
		mRays.clear();
	}

	size_t numrays = rays.size();
	for(size_t i=0;i<numrays;i++)
	{
		TraceResults result;
		mScene->Trace(rays[i].mRay, result, rays[i].mIgnore, rays[i].mShouldSort);
		rays[i].mTraceResults.setValue(result);
	}

	//now do the queued sweeps
	std::vector<SweepQuery> sweeps;
	{
		SynchronizedBlock block(mSweepListLock);
		sweeps.reserve(mSweeps.size());
		sweeps = mSweeps;
		mSweeps.clear();
	}
	size_t numsweeps = sweeps.size();
	for(size_t i=0;i<numsweeps;i++)
	{
		CollisionResult result;
		CollisionList list;
		list.setPhysicsWorld(this);

		mScene->SweepHit(sweeps[i].mPrimitive, sweeps[i].vel, result, &list, sweeps[i].mIgnore);
		sweeps[i].mResult.setValue(list);
		sweeps[i].mPrimitive = NULL;
	}

	//sleep for a little while, this way we avoid a deltatime = 0 which will cause our entities to
	//think they've stopped moving....
	Sleep(0.01f);

	return true;
}

bool PhysicsWorld::Collide(btConvexShape* shapeA, const Vector3& posA, const Vector3& velA, const Matrix3& initOrientationA, const Matrix3& destOrientationA, btConvexShape* shapeB, const Vector3& posB, const Vector3& velB, const Matrix3& initOrientationB, const Matrix3& destOrientationB, CollisionResult& result)
{
	btTransform fromA;
	btTransform fromB;
	btTransform toA;
	btTransform toB;

	Vector3 dest = posA + velA;

	fromA.setOrigin( BulletMathUtil::ConvertTo(posA) );
	fromA.setBasis( btMatrix3x3::getIdentity() ); //fixme! Need initial Rotation!
	toA.setOrigin( BulletMathUtil::ConvertTo(dest) );
	toA.setBasis( btMatrix3x3::getIdentity() ); //fixme!!! Need Dest Rotation!

	dest = posB + velB;

	fromB.setOrigin( BulletMathUtil::ConvertTo(posB) );
	fromB.setBasis( btMatrix3x3::getIdentity() ); ///fixme, use InitRotation
	toB.setOrigin( BulletMathUtil::ConvertTo(dest) );
	toB.setBasis( btMatrix3x3::getIdentity() ); ///fixme, use DestRotation

	btSubsimplexConvexCast convexCaster( shapeA, shapeB, mSimplexSolver );

	btConvexCast::CastResult castResult;

	if( convexCaster.calcTimeOfImpact( fromA, toA, fromB, toB, castResult ) )
	{
		result.m_normal = BulletMathUtil::ConvertTo(castResult.m_normal);
		//godzassert( result.m_normal.GetLength() > 1 - EPSILON && result.m_normal.GetLength() < 1 - EPSILON );
		result.m_normal.Normalize();
		result.m_hitLocation = BulletMathUtil::ConvertTo(castResult.m_hitPoint);

		//TODO: Might be better to return distance from
		result.m_distance = castResult.m_fraction * velA.GetLength();
		//result.m_distance = ( result.m_hitLocation - p).GetLength();
		result.m_bStuck = false; //castResult.m_fraction < 0.1f;
		result.m_plane = WPlane(result.m_hitLocation, result.m_normal);

		return true;
	}

	return false;
}

void PhysicsWorld::UpdateChildren(PhysicsObject* moby, const Vector3& velocity, bool isZeroVelocity)
{
	//Apply relative transformations to the child physics objects...
	size_t num = moby->m_children.size();
	for (size_t i = 0; i < num; i++)
	{
		PhysicsObject* po = moby->m_children[i];

		if (!isZeroVelocity)
		{
			//Move this child's primitive (since it stores pos/rot in world space)
			po->m_primitive->TransformBy(velocity);
		}

		//Note: position is stored relative to the parent, so we don't change
		//the child's local position!

		Vector3 p;
		Matrix3 mat;
		po->m_primitive->GetTransform(p, mat);

		TransformState tm;
		tm.m_rot = mat;
		tm.m_pos = p;
		po->m_worldTm = tm;

		po->m_vel = velocity;
		po->m_frame = mFrameNum;

		if (!isZeroVelocity)
		{
			if (po->GetRegion() != NULL)
				po->GetRegion()->OnEntityMoved(po);
		}

		//NOTE: Here is where we'd run a physics sim on this
		//object however atm it only makes sense to support Static
		//motion thus we don't bother sweeping the physics object or anything.
		//Just let other physics objects bump into this one...
	}

}

void PhysicsWorld::SubmitToScene(PhysicsObject* moby)
{
	//If the physics obj is just a detector, then don't add it to the scene.
	if (moby->GetMotionType() != MotionType_Phantom 
		&& moby->GetMotionType() != MotionType_MovementQuery
		&& moby->GetMotionType() != MotionType_Projectile
		)
	{
		mScene->AddEntity(moby);
	}
}

PhysicsObjectProxy PhysicsWorld::CreatePhysicsObject(const Vector3& pos, const Vector3& vel, const PhysicsObjectCreationData& data)
{
	//This part doesn't need to be synchronized....
	PhysicsObject* obj = new PhysicsObject(data.m_primitive);
	obj->m_pos.set(pos);
	obj->m_motionType = data.m_motionType;
	obj->m_world = this;
	obj->m_goalVel = vel;
	obj->m_id = data.m_id;
	obj->m_origin = data.m_origin;

	Vector3 p;

	TransformState tm;
	tm.m_pos = pos; //use passed in position

	data.m_primitive->GetTransform(p, tm.m_rot);

	//set initial transform
	obj->m_worldTm = tm;

	return PhysicsObjectProxy(obj);
}

void PhysicsWorld::AddPhysicsObject(PhysicsObjectProxy& proxy)
{
	//lock access to scene queue
	while(!mQueueLock.Set());
	mAddQueue.push_back(proxy.m_physicsObject);
	mQueueLock.UnSet();
}

void PhysicsWorld::RemovePhysicsObject(PhysicsObjectProxy& obj)
{
	SynchronizedBlock lock(mRemoveQueueLock);
	mRemoveQueue.push_back(obj.m_physicsObject);
}

bool PhysicsWorld::CanRotateActor(PhysicsObject* moby, const Matrix3& destRot)
{
	GenericReference<CollisionPrimitive> model = moby->GetCollisionModel()->GetClone();
	Vector3 p;
	Matrix3 mt;
	model->GetTransform(p, mt);

	model->SetTransform(p, destRot);

	CollisionResult result;
	CollisionList pList;
	pList.setPhysicsWorld(this);

	mScene->SweepHit(model, ZERO_VECTOR, result, &pList, moby);

	size_t numResults = pList.GetNumResults();
	if (numResults > 0)
	{
		//debug test!!!!
		//model->SetTransform(p, mt);
		//size_t numResultsTest = mScene->SweepHit(model, ZERO_VECTOR, result, &pList, moby);

		//cant rotate there....
		return false;
	}

	return true;
}

bool PhysicsWorld::MoveActor(PhysicsObject* moby, const Vector3& origin, const Vector3& destination, Vector3& endPosition)
{
	CollisionPackage collData;

	if (origin==destination)
		return false;


	collData.m_lastSafePosition = origin;
	collData.m_nQueries = 0;
	collData.m_collisionStatus = CollisionStatus_NotSet;
	collData.m_collideAndSlide = false;

	Vector3 vi = destination - origin;

	endPosition = CollideWithWorld(moby, origin, vi, collData);

	//TODO: get rid of float compares here
	if ( endPosition != origin 
		//&& !IsModelStuck(endPosition, moby) //sometimes the collide-and-slide algorithm screws up. This prevents falling out of world
		) 
	{
		return true;
	}

	return false;
}


//Telemachos: http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html provides some basic algorithms used here
//Paul Nettle: http://www.gamedev.net/reference/articles/article1026.asp prodides some useful background info on collision like
//which polygons should be discarded
Vector3 PhysicsWorld::CollideWithWorld(PhysicsObject* entity, const Vector3& origin, const Vector3& velocity, CollisionPackage& collData)
{
	collData.m_nQueries++;

#if defined(DEBUG_COLL_SLIDE)
		Log("----------------------------------------------\n");
		Log("[q:%d] intial vel:%f y:%f z:%f\n", collData.m_nQueries, velocity.x,velocity.y,velocity.z);
		Log("intial pos:%f y:%f z:%f\n", origin.x,origin.y,origin.z);
#endif


	static const int COLL_QUERY_MAX = 8;
	static const float MIN_DIST_TO_PLANE = 4; //this is how close we want to be to a plane (cushion)

	if (collData.m_nQueries == COLL_QUERY_MAX)
	{
#if defined(DEBUG_COLL_SLIDE)	
		Log("COLL_QUERY_MAX force_move\n");
#endif

		//at this point we have already slid on a ton of polys (probably same ones over and over)
		return origin;
	}

	CollisionPrimitive* model = entity->GetCollisionModel();

	CollisionResult result;
	CollisionList pList;
	pList.setPhysicsWorld(this);

	mScene->SweepHit(model, velocity, result, &pList, entity);

	size_t numResults = pList.GetNumResults();

	//if any hit got us stuck then reject all
	for (size_t i=0;i<numResults;i++)
	{
		result = pList[i];
		if (result.m_bStuck)
		{
			return collData.m_lastSafePosition;
		}
	}

	long motion = entity->m_motionType;
	MotionType m = (MotionType)motion;

	switch(m)
	{
	default:
		break;
	case MotionType_Phantom:
	case MotionType_Projectile:
		{
			//cache hits....
			for (size_t i=0;i<numResults;i++)
			{
				PhysicsCollisionResult r(pList[i]);
				entity->m_collisions.AddCollision(r);
			}

			if (m == MotionType_Projectile && numResults > 0)
			{
				mInternalRemoveQueue.push_back(entity);

				//projectiles remove themselves from physics world when they hit something...
				mScene->ReleaseEntity(entity);

				return pList.GetClosestResult()->m_hitLocation;
			}
			else //collision phantoms will maintain their trajectory
			{
				return origin + velocity;
			}
		}
		break;
	}

  // happens pretty often enough....
	if (velocity.GetLength() < EPSILON)
	{
#if defined(DEBUG_COLL_SLIDE)
		Log("velocity.GetLength() < EPSILON\n");
#endif
		return origin;
	}


	if (numResults > 0)
	{
		// how long is our velocity
		float distanceToTravel = lengthOfVector(velocity);
#if defined(DEBUG_COLL_SLIDE)
		
		Log("travel_distance %f\n",distanceToTravel);

		unsigned int numPlanesHit = 0;
#endif

		Vector3 slide = velocity; //slide velocity component
		

		//move close to plane
		float closest=pList.GetClosestResult()->m_distance;

		std::vector<Halfspace> planeList;
		planeList.reserve(numResults);

		for (size_t i = 0;i < numResults;i++)
		{
			result = pList[i];

			// Determine our location on the plane....
			planeList.push_back( result.m_plane.ClassifyPoint( origin ) );

			PhysicsObject* physObj = result.model.m_physicsObject;

#if defined(DEBUG_COLL_SLIDE)
				if (i>0)
				{
					Log("----------------------------------------------\n"); //debug
				}
				Log("index %d face %f %f %f\n", i, result.m_normal.x, result.m_normal.y, result.m_normal.z);
				Log("query %d\n", collData.m_nQueries);
#endif

			if (result.m_bStuck)
			{
				//This looks bad but what it means is that our slide vector has gotten rejected if num of 
				//queries was greater than 1. However, if we get rejected on first query that means we somehow
				//let the user get themselves embedded which would be bad

#if defined(DEBUG_COLL_SLIDE)
				Log("Stuck in plane (query %d) -- reset to lastPos: %f %f %f \n",collData.m_nQueries, collData.m_lastSafePosition.x,collData.m_lastSafePosition.y,collData.m_lastSafePosition.z);

				if (collData.m_nQueries==1)
				{
					Log("bah we're stuck / embedded \n");
					//_asm nop;
				}
#endif DEBUG_COLL_SLIDE

				return collData.m_lastSafePosition;
			}
			
			//We assume we hit an AABB surf
			{

#if defined(DEBUG_COLL_SLIDE)
				Log("distance_for_plane %f\n",result.m_distance);
#endif

				Vector3 dest = origin;

				//perform sliding
				//http://www.gamedev.net/community/forums/topic.asp?topic_id=395347
				//A slide is kind of like a reflection, except you just remove the component 
				//in the direction of the plane normal instead of reflecting it. So if an object 
				//with initial velocity vi collides with a plane with normal n (unit vector), 
				//its slide velocity is vi-(vi.n)n (or, in code, slideVelocity = initialVelocity - (Vector.DotProduct(plane.Normal, initialVelocity) * plane.Normal)).		
				
				float dotprod = result.m_normal.Dot(slide); //really should be using vi here not slide

#if defined(DEBUG_COLL_SLIDE)
				Log("dotprod:%f \n", dotprod);
#endif

				//From Paul Nettle & Quake3
				if (dotprod > 0.1f) {

#if defined(DEBUG_COLL_SLIDE)
				Log("bail\n");
				Log("ignoring face %f %f %f\n", result.m_normal.x, result.m_normal.y, result.m_normal.z);

				//Vector3 temp = slide - ( result.m_normal * dotprod);
				//Log("tempSlide x:%f y:%f z:%f\n", temp.x,temp.y,temp.z);
#endif	

					continue; //avoid solid walls
				}

#if defined(DEBUG_COLL_SLIDE)
				Vector3 tempA = ( result.m_normal * dotprod);
				Log("( result.m_normal * dotprod) %f %f %f dot: %f\n", tempA.x,tempA.y,tempA.z,dotprod);
				Log("slide_before slx:%f sly:%f slz:%f\n", slide.x,slide.y,slide.z);

				numPlanesHit++;
#endif	

				slide -= ( result.m_normal * dotprod);
				


#if defined(DEBUG_COLL_SLIDE)
				Log("slide_result= slx:%f sly:%f slz:%f\n", slide.x,slide.y,slide.z);	
#endif
			}
		} //for loop
		

		
		//compute new destination point
		Vector3 dest = origin;

		if ((closest > 0) && (closest <= distanceToTravel) 
			&& closest > MIN_DIST_TO_PLANE //fix for bobbing
			)
		{
			Vector3 v = velocity;
			setLength(v, closest-EPSILON);

#if defined(DEBUG_COLL_SLIDE)
			Log("closest_slide_for_all_planes x:%f y:%f z:%f dist %f\n", v.x,v.y,v.z,closest);
#endif
			dest = origin + v;
			//dest.y += MIN_DIST_TO_PLANE; //bring the character feet off the polygon a bit

			//try to move the model close to destination
			v.Normalize();
			dest -= (v * MIN_DIST_TO_PLANE);

			collData.m_collideAndSlide = true; //unused atm
		}

		//Check to see if we slid through the polygons...
		Vector3 test_pos = dest+slide;
		for( size_t planeInd = 0; planeInd < planeList.size(); planeInd++ )
		{
			if( planeList[planeInd] != pList[planeInd].m_plane.ClassifyPoint( test_pos ) )
			{
				// Abort, we just slid through the polygon or penetrated it...
				return collData.m_lastSafePosition;
			}
		}

#if defined(DEBUG_COLL_SLIDE)
		//Vector3 finalpos=dest + slide; 
		Log("num planes slide %d\n",numPlanesHit);
		Log("slide pos %f %f %f\n",dest.x,dest.y,dest.z);
		Log("slide:%f y:%f z:%f\n", slide.x,slide.y,slide.z);
#endif DEBUG_COLL_SLIDE

		collData.m_collisionStatus = CollisionStatus_BaseFound;
		SetBaseFound(entity);
		
		return CollideWithWorld(entity, dest, slide, collData);
	}
	else
	{
		//we only consider that we've gone into a freefall if we pass collision check on 1st pass
		if (collData.m_nQueries <= 1)
		{
			entity->m_status = CollisionStatus_NotSet;
		}

#if defined(DEBUG_COLL_SLIDE)
		Vector3 debugfinalpos = (origin+velocity);
		Log("freefall pos %f %f %f\n",debugfinalpos.x,debugfinalpos.y,debugfinalpos.z);
#endif
	}

	Vector3 final_pos = (origin+velocity);
	collData.m_lastSafePosition=final_pos; //please keep this. keep in mind this is a recursive routine

	return final_pos; //no collisions occured
}

void PhysicsWorld::SetBaseFound(PhysicsObject* entity)
{
	//tell the entity it has recently landed since we bout to flip the bit
	if (entity->m_status == CollisionStatus_NotSet)
	{
		entity->m_hasLanded = true;
	}

	entity->m_status = CollisionStatus_BaseFound;
}

bool PhysicsWorld::IsModelStuck(const Vector3& position, PhysicsObject* moby)
{
	//Check to see if the model has been moved to a bad spot.
	GenericReference<CollisionPrimitive> newModel = moby->GetCollisionModel()->GetClone();

	CollisionList validiateCollision;
	validiateCollision.SetBailOnContact(true); //bail if any polygons are collided

	CollisionResult newResult;
	mScene->SweepHit(newModel, ZERO_VECTOR, newResult, &validiateCollision, moby);

	size_t numResults = validiateCollision.GetNumResults();
	return (numResults > 0);
}


void PhysicsWorld::Trace(Future<TraceResults> result, const WRay& ray, PhysicsObject* ignore, bool sortResults)
{
	RayQuery query;
	query.mRay = ray;
	query.mTraceResults = result;
	query.mShouldSort = sortResults;
	query.mIgnore = ignore;

	SynchronizedBlock block(mRayListLock);
	mRays.push_back(query);
}

void PhysicsWorld::Sweep(Future<CollisionList> result, CollisionPrimitive* primitive, const Vector3& vel, PhysicsObjectProxy* ignore)
{
	//instance a copy for this thread to access
	GenericReference<CollisionPrimitive> clone = primitive->GetClone();

	SweepQuery query;
	query.mPrimitive = clone;
	query.vel = vel;
	query.mResult = result;

	if (ignore != NULL)
	{
		query.mIgnore = ignore->m_physicsObject;
	}
	else
		query.mIgnore = NULL;

	SynchronizedBlock block(mSweepListLock);
	mSweeps.push_back(query);
}

} //namespace GODZ


