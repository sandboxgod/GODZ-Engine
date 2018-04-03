/* ===========================================================

	Created Jan 27, '10 by Richard Osborne
	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__MACTOR_H__)
#define __MACTOR_H__

#include "Godz.h"
#include "GodzAtomics.h"
#include "GenericReferenceCounter.h"
#include "SlotIndex.h"
#include <CoreFramework/Collision/Physics.h>
#include <CoreFramework/Collision/PhysicsObjectProxy.h>
#include <CoreFramework/Collision/CollisionPrimitive.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/WSphere.h>

namespace GODZ
{

	struct GODZ_API AnimData
	{
		size_t m_id;
		float m_ease;
	};

	/*
	* Multithreaded Actor proxy. Exposes basic functions that external threads can call.
	*/
	class GODZ_API MActor
	{

	public:
		MActor();

		void SetID(EntityID id);

		bool IsAnimationDirty();
		void UnsetAnimation();

		//Overrides the actor on the main thread and forces it to play the desired animation
		void PlayAnimation(AnimData& anim);

		//Returns the anim this local actor is playing
		void GetAnimation(AnimData& data);

		bool IsRotationDirty();
		void UnsetRotation();

		//Returns this object's local rotation value
		void GetRotation(Quaternion& rot);

		void SetRotation(const Quaternion& rot);

		void SetPhysicsObject(PhysicsObjectProxy& proxy);
		PhysicsObjectProxy GetPhysicsObject();

		bool IsOrientationDirty();
		void UnsetOrientation();
		void SetOrientation(const WMatrix3& mat);
		void GetOrientation(WMatrix3& mat);

		bool IsStandingOnPolygon();
		long GetNavMeshPolyIndex();
		void SetNavMeshPolygon(bool isStandingOnPolygon, long polyindex);

		void SetMoveSpeed(long speed);
		long GetMoveSpeed();

		void SetCollision(CollisionPrimitive* col);
		void GetSphere(WSphere& s);

	private:
		AtomicBool mIsStandingOnPolygon;		//navigation flag
		AtomicInt mPolyIndex;					//navmesh poly we standing on if the flag is true

		GodzAtomic m_isRotationDirty;			//set when we want the main thread to grab
		SyncPoint<Quaternion>	m_rot;

		GodzAtomic m_isRotationMatrixDirty;		//set when we want the main thread to grab
		SyncPoint<WMatrix3>	m_orientation;

		GodzAtomic m_isAnimationDirty;
		SyncPoint<AnimData> m_anim;

		SyncPoint<PhysicsObjectProxy> m_proxy;

		AtomicInt mMoveSpeed;
		SyncPoint<WSphere> m_col;

		EntityID m_id;
	};

}

#endif //__MACTOR_H__
