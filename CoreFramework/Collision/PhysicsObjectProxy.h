/* ===========================================================

	Created Jan 17, '10 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__PHYSOBJPROXY_H__)
#define __PHYSOBJPROXY_H__

#include <CoreFramework/Core/GodzAtomics.h>
#include "Physics.h"
#include <CoreFramework/Core/SlotIndex.h>
#include <CoreFramework/Math/Vector3.h>
#include <CoreFramework/Math/AxisAngle.h>
#include <CoreFramework/Math/WMatrix3.h>

namespace GODZ
{
	class PhysicsWorld;
	class PhysicsObject;
	class MActor;
	class PhysicsCollisionResults;

	/*
	* Physics proxy object, all exposed functions are thread safe
	*/
	class GODZ_API PhysicsObjectProxy
	{
		friend class PhysicsWorld;

	public:
		PhysicsObjectProxy();
		PhysicsObjectProxy(PhysicsObject* obj);

		MActor* GetMActor();

		EntityID GetID() const;
		void AddChild(PhysicsObjectProxy& child);

		void GetDesiredVelocity(Vector3& v);
		void SetDesiredVelocity(const Vector3& v);
		void GetVelocity(Vector3& v);

		//Return's this node's rotation
		void GetRotation(Matrix3& rot);
		void SetRotation(const Matrix3& rot);
		void SetAngularVelocity(const AxisAngle& ang);

		//Return's this node's position
		void GetPosition(Vector3& pos);
		void SetPositionUnchecked(const Vector3& pos);

		void GetWorldTransform(Vector3& pos, Matrix3& rot);

		long GetFrameNumber() const;
		long GetRotationFrameNumber() const;
		bool IsValid();

		void SetMotionType(MotionType type);
		MotionType GetMotionType();
		CollisionStatus GetCollisionState();

		void GetCollisions(PhysicsCollisionResults* list);
		void ClearCollisions();
		bool IsInWorld() const;

		//Resets the hasLanded cached value to false.
		void ResetHasLanded();

		//Returns if this object has landed.
		bool HasLanded() const;

		bool operator==(PhysicsObjectProxy& other) const
		{
			return m_physicsObject == other.m_physicsObject;
		}

	private:
		//kept private; only thread safe functions will be exposed. Also it's basically read-only, so it's
		//thread safe for other threads to access concurrently
		GenericReference<PhysicsObject> m_physicsObject;
	};

}

#endif //__PHYSOBJPROXY_H__
