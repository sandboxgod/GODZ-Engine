/* ===========================================================
	Collision interface.

	Copyright (c) 2011
	========================================================== 
*/

#pragma once

#include "CollisionSystems.h"
#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/GDZArchive.h>
#include <CoreFramework/Core/GenericReferenceCounter.h>
#include <CoreFramework/Math/WSphere.h>
#include <CoreFramework/Math/WBox.h>
#include <CoreFramework/Math/WMatrix3.h>

class btConvexShape;

namespace GODZ
{
	class ICollisionCallback;
	class RenderDeviceObject;

	//pure virtual interface, represents a Collision primitive (Shape)
	class GODZ_API CollisionPrimitive : public GenericReferenceCounter
	{
	public:
		virtual ~CollisionPrimitive() {}

		enum CollisionType
		{
			CollisionType_AABB,
			CollisionType_AABBTree,
			CollisionType_Sphere,
			CollisionType_OBB
		};

		//for debugging
		virtual void SetName(HString name) {};
		virtual HString GetName() { return HString(); };

		//Returns a Copy of this instance
		virtual GenericReference<CollisionPrimitive> GetClone() = 0;
		virtual CollisionType GetCollisionType() = 0;
		virtual void GetBox(WBox& box) const = 0;
		virtual void GetSphere(WSphere& sphere) const = 0;

		//Returns true if the sweep volume test is successful
		virtual bool SweepHit(CollisionPrimitive* other, const Vector3& otherVelocity, const Vector3& myVelocity, CollisionResult& result, ICollisionCallback* call) = 0;
		virtual bool SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result) = 0;

		//Returns true if this collision model is colliding with another model.
		virtual bool Intersects(CollisionPrimitive* prim) = 0;

		virtual void Serialize(GDZArchive& ar) = 0;

		//Returns true if this primitive is hit by the ray.
		virtual bool Trace(const WRay& ray, TraceResult& result) = 0;

		//Applies a relative transform to this primitive using the matrix argument
		virtual void TransformBy(const WMatrix16f& matrix) = 0;
		virtual void TransformBy(const Vector3& pos) = 0;

		virtual void GetTransform(Vector3& pos, WMatrix3& matrix) const = 0;
		virtual void GetTransform(WMatrix& matrix) const = 0;

		//Applies an absolute world transform to this primitive using the matrix argument
		virtual void SetTransform(const Vector3& pos, const WMatrix3& matrix) = 0;
		virtual void SetTransform(const WMatrix& matrix) = 0;
		virtual void SetPosition(const Vector3& pos) = 0;

		virtual Future<GenericReference<RenderDeviceObject> > GetRenderObject( void ) const = 0;

		//Returns the Collision Shape used by this primitive
		virtual btConvexShape* GetShape() { return NULL; }
	};
}