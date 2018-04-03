/* ===========================================================

	Created Jan 16, '10 by Richard Osborne
	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#pragma once

#include "Physics.h"
#include "CollisionSystems.h"
#include "CollisionList.h"
#include <vector>
#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/SlotIndex.h>


//Bullet Physics
class btCollisionObject;

namespace GODZ
{
	class AABBNode;
	class PhysicsWorld;
	class SceneNode;
	class CollisionPrimitive;
	struct CollisionResult;

	/*
	* Physics object. Note: for child objects, position / rotation should be maintained
	* in object space (this way it's relative to the parent)
	*/
	class GODZ_API PhysicsObject : public GenericReferenceCounter
	{
		friend class PhysicsWorld;

	public:
		PhysicsObject(CollisionPrimitive* primitive);
		~PhysicsObject();
		
		void AddChild(GenericReference<PhysicsObject> child);

		//returns local position
		void SetPositionUnchecked(const Vector3& pos);
		void GetPosition(Vector3& pos);

		//returns local rotation
		void GetRotation(Matrix3& rot);
		void SetRotation(const Matrix3& rot);
		void SetAngularVelocity(const AxisAngle& angVel);

		//returns world space pos / rot
		void GetWorldTransform(Vector3& pos, Matrix3& rot);

		void GetVelocity(Vector3& vel);
		void GetDesiredVelocity(Vector3& vel);
		void SetDesiredVelocity(const Vector3& v);

		long GetFrameNumber() const;
		long GetRotationFrameNumber() const;

		void SetMotionType(MotionType type);
		MotionType GetMotionType() const;

		CollisionStatus GetCollisionState() const;
		void GetCollisions(PhysicsCollisionResults& list);
		void ClearCollisions();
		EntityID GetID() const { return m_id; }
		bool IsInWorld() const;

		void ResetHasLanded();
		bool GetHasLanded() const { return m_hasLanded; }

		void UpdateCollisionObject();

		bool Collide(const Vector3& velA, PhysicsObject* other, const Vector3& velB, CollisionResult& result);

		// invMat - Inverse Matrix that should be used to transform this PhysicsObject 
		// into object space. Should be set to null in most cases.
		bool Collide(const Matrix4* invMat, const Vector3& velA, btConvexShape* shape, const Vector3& posB, const Vector3& velB, const Matrix3& initOrientation, const Matrix3& destOrientation, CollisionResult& result);

		//-----------------------------------------------------------------------------------------------------------------
		// Functions that should not be called outside of physics thread... So should not be exposed to the 
		// main thread via proxy...
		//-----------------------------------------------------------------------------------------------------------------
		CollisionPrimitive* GetCollisionModel() { return m_primitive; }
		void SetRegion(SceneNode* node) { m_region = node; }
		SceneNode* GetRegion() { return m_region; }
		PhysicsObject* GetParent() { return m_parent; }
		bool IsParent(PhysicsObject* parent);
		

	private:

		//Set the status of this object; whether or not its currently within a physics update frame
		void SetBusy(bool state);

		// Thread Safe data that is synchronized between the threads
		SyncPoint<Vector3> m_pos;					//position
		SyncPoint<Vector3> m_vel;					//current velocity (computed during last physics frame)
		SyncPoint<Vector3> m_goalVel;				//goal / desired velocity
		SyncPoint<Matrix3> m_rot;
		SyncPoint<AxisAngle> m_goalAngularVelocity;	//goal / desired angular velocity

		//TODO: Move these child only variables into a subclass....
		//Children -- Child nodes store data in local space. So need to backup world space
		//data captured from the primitive
		SyncPoint<TransformState> m_worldTm;

		AtomicInt m_status;
		AtomicInt	m_frame;				//last physics frame # we were updated
		AtomicInt	m_rotFrame;				//last physics frame # we were updated
		AtomicInt m_motionType;
		AtomicBool	m_currentlyUpdating;	//flag indicating current physics object status
		AtomicBool m_hasLanded;				//set if we detected model has landed

		GodzAtomic m_nextPositionLock;
		SyncPoint<Vector3> m_nextPos;

		GodzAtomic m_nextRotationLock;
		SyncPoint<Matrix3> m_nextRot;

		GenericReference<CollisionPrimitive> m_primitive;
		SceneNode*			m_region;
		SyncPoint<PhysicsWorld*>		m_world;
		SynchronizedCollisionList m_collisions;					//collision list

		//(read-only) cached object ID; useful for cached collision results....
		EntityID m_id;

		//Relative Offset of this object from the origin in object space
		Vector3 m_origin;

		PhysicsObject* m_parent;
		std::vector<GenericReference<PhysicsObject> > m_children;

		//Bullet.... Physics Thread only for now....
		btCollisionObject* m_collisionObject;
	};

}

