#pragma once

#include "CollisionPrimitive.h"

class btBoxShape;

namespace GODZ
{
	class GODZ_API AABBCollisionPrimitive : public CollisionPrimitive
	{
	public:
		AABBCollisionPrimitive();
		~AABBCollisionPrimitive();

		//for debugging
		virtual void SetName(HString name) { m_name = name; }
		virtual HString GetName() { return m_name; }

		//Returns a Copy of this instance
		virtual GenericReference<CollisionPrimitive> GetClone();

		virtual CollisionType GetCollisionType() { return CollisionType_AABB; };
		virtual void GetBox(WBox& box) const;
		virtual void GetSphere(WSphere& sphere) const;

		//Returns true if the sweep volume test is successful
		virtual bool SweepHit(CollisionPrimitive* other, const Vector3& otherVelocity, const Vector3& myVelocity, CollisionResult& result, ICollisionCallback* call);
		virtual bool SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result);

		//Returns true if this primitive is hit by the ray.
		virtual bool Trace(const WRay& ray, TraceResult& result);

		//Returns true if this collision model is colliding with another model.
		virtual bool Intersects(CollisionPrimitive* prim);

		//Assigns a new bound to this collision primitive.
		void SetBounds(const WBox& bound);
		const WBox& GetBounds(void) const { return m_box; }

		virtual void Serialize(GDZArchive& ar);

		//Transforms this primitive using the matrix argument
		virtual void TransformBy(const WMatrix16f& matrix);
		virtual void TransformBy(const Vector3& pos);

		virtual void GetTransform(WMatrix& matrix) const;
		virtual void GetTransform(Vector3& pos, WMatrix3& matrix) const;
		virtual void SetTransform(const Vector3& pos, const WMatrix3& matrix);
		virtual void SetTransform(const WMatrix& matrix);
		virtual void SetPosition(const Vector3& pos);

		virtual Future<GenericReference<RenderDeviceObject> > GetRenderObject( void ) const;

		virtual btConvexShape* GetShape();

	protected:
		WBox m_box;
		HString m_name; //debugging
		btBoxShape* m_boxShape;
	};
}
