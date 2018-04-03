

#include "SphereCollisionPrimitive.h"
#include "RayCollider.h" 
#include "ICollisionCallback.h"
#include <CoreFramework/Core/RenderDeviceEvent.h>
#include <CoreFramework/Core/SceneData.h>

using namespace GODZ;


Future<GenericReference<RenderDeviceObject> > SphereCollisionPrimitive::GetRenderObject( void ) const
{
	CreateSphereEvent* tevent = new CreateSphereEvent();

	//make a new sphere thats center at origin (0,0,0)
	tevent->center = m_sphere.center;
	tevent->radius = m_sphere.radius;

	Future<GenericReference<RenderDeviceObject> > renderFuture = tevent->m_renderDeviceObject;
	SceneData::AddRenderDeviceEvent(tevent);

	return renderFuture;
}

GenericReference<CollisionPrimitive> SphereCollisionPrimitive::GetClone()
{
	SphereCollisionPrimitive* newModel = new SphereCollisionPrimitive();
	newModel->SetBounds(m_sphere);
	newModel->SetName( m_name );

	return newModel;
}

void SphereCollisionPrimitive::GetBox(WBox& box) const
{
	box = m_sphere;
}

void SphereCollisionPrimitive::GetSphere(WSphere& sphere) const
{
	sphere = m_sphere;
}

bool SphereCollisionPrimitive::SweepHit(CollisionPrimitive* otherPrimitive, const Vector3& va, const Vector3& vb, CollisionResult& result, ICollisionCallback* call)
{
	switch(otherPrimitive->GetCollisionType())
	{
	default:
		{
			WSphere s;
			otherPrimitive->GetSphere(s);

			float t;

			if (m_sphere.IntersectsSphere(s, va, vb, t) )
			{
				//TODO: best guess...
				result.m_hitLocation = m_sphere.center + m_sphere.radius + ( vb * t );

				//TODO: maybe look into making m_distance squared...
				result.m_distance = (vb * t).GetLength();

				//compute the nearest plane (used for collide & slide). If we don't specify a plane to slide along,
				//the character will make an abrupt stop when it collides with another character.
				/*
				result.m_plane.N = va;
				result.m_plane.N.Normalize();
				result.m_plane.N.Invert();
				result.m_plane.N = WBox::GetNearPlane(result.m_plane.N);
				*/

				WBox b = m_sphere;
				result.m_normal = b.GetNormalFromPoint(result.m_hitLocation);

				call->OnCollisionOccured(result);
				return true;
			}
		}
		break;
	case CollisionType_AABBTree:
		{
			//Let AABBTree handle the collision query
			return otherPrimitive->SweepHit(this, vb, va, result, call);
		}
		break;
	}

	return false;
}

bool SphereCollisionPrimitive::SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result)
{

	WPlane p(triangle[0], triangle[1], triangle[2]);
	if( m_sphere.IntersectTriangle(triangle[0], triangle[1], triangle[2], p.N, velocity, result.m_hitLocation, result.m_distance) )
	{
		float velmag = velocity.GetLength();
		if (result.m_distance <= velmag)
		{
			result.m_normal = p.N;
			result.m_bStuck = result.m_distance <= EPSILON;		//we are stuck in the geometry
			return true;
		}
	}

	return false;
}

bool SphereCollisionPrimitive::Intersects(CollisionPrimitive* prim)
{
	//TODO: Need obb vs Sphere!
	float t; //t doesn't matter for stationary objects
	WSphere s;
	prim->GetSphere(s);
	return m_sphere.IntersectsSphere(s, ZERO_VECTOR, ZERO_VECTOR, t);
}

void SphereCollisionPrimitive::SetBounds(const WSphere& bound)
{
	m_sphere=bound;
}

void SphereCollisionPrimitive::Serialize(GDZArchive& ar)
{
	ArchiveVersion version(1,1);
	ar << version;

	ar << m_sphere;
}


bool SphereCollisionPrimitive::Trace(const WRay& ray, TraceResult& result)
{
	bool isHit = m_sphere.Intersects(ray, result.m_distance);
	result.hitLocation = ray.origin + (ray.dir * result.m_distance);

	//TODO: Compute the hit normal....

	return isHit;
}

void SphereCollisionPrimitive::TransformBy(const WMatrix16f& matrix)
{
	matrix.Mul(m_sphere.center);
}

void SphereCollisionPrimitive::TransformBy(const Vector3& p)
{
	m_sphere.center + p;
}

void SphereCollisionPrimitive::GetTransform(Vector3& pos, WMatrix3& matrix) const
{
	pos = m_sphere.center;
	matrix.MakeIdentity();
}

void SphereCollisionPrimitive::GetTransform(WMatrix& matrix) const
{
	matrix.MakeIdentity();
	matrix.SetTranslation(m_sphere.center);
}

void SphereCollisionPrimitive::SetTransform(const Vector3& pos, const WMatrix3& m)
{
	m_sphere.center = pos;
}

void SphereCollisionPrimitive::SetTransform(const WMatrix& m)
{
	m_sphere.center = m.GetTranslation();
}

void SphereCollisionPrimitive::SetPosition(const Vector3& pos)
{
	m_sphere.center = pos;
}
