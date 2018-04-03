
#include "OBBCollisionPrimitive.h"
#include "AABBCollisionPrimitive.h"
#include "SphereCollisionPrimitive.h"
#include "RayCollider.h" 
#include "ICollisionCallback.h"
#include "GeoBuilder.h"
#include <float.h>
#include <CoreFramework/Core/RenderDeviceEvent.h>
#include <CoreFramework/Core/SceneData.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

using namespace GODZ;

OBBCollisionPrimitive::OBBCollisionPrimitive()
: m_boxShape(NULL)
{
}

OBBCollisionPrimitive::~OBBCollisionPrimitive()
{
	if( m_boxShape != NULL )
	{
		delete m_boxShape;
		m_boxShape = NULL;
	}
}

Future<GenericReference<RenderDeviceObject> > OBBCollisionPrimitive::GetRenderObject( void ) const
{
	TriMeshEvent* tevent = new TriMeshEvent();

	Color4f c(0.2f, 0.2f, 1.0f, 1.0f);
	tevent->color = c;

	Matrix3 matrix;
	Vector3 p;
	GetTransform(p, matrix);

	//make a new box thats center at origin (0,0,0)

	//Matrix will translate us to the current world position
	WMatrix m(1);
	CopyMatrix(matrix, m);
	m.SetTranslation( m_box.center );
	tevent->m = m;

	//Build polygons in object space using an identity rotation...
	GeoBuilder::BuildBoxMesh(m_box.extent * -1, m_box.extent, tevent->m_mesh);

	Future<GenericReference<RenderDeviceObject> > renderFuture = tevent->m_renderDeviceObject;
	SceneData::AddRenderDeviceEvent(tevent);

	return renderFuture;
}

GenericReference<CollisionPrimitive> OBBCollisionPrimitive::GetClone()
{
	OBBCollisionPrimitive* newModel = new OBBCollisionPrimitive();
	newModel->SetBounds(m_box);
	newModel->SetName( m_name );
	return newModel;
}


bool OBBCollisionPrimitive::SweepHit(CollisionPrimitive* otherPrimitive, const Vector3& va, const Vector3& vb, CollisionResult& result, ICollisionCallback* call)
{
	switch(otherPrimitive->GetCollisionType())
	{
	default:
		{
			//OBB vs OBB
			WBox boxA;
			otherPrimitive->GetBox(boxA);

			//convert AABB ---> OBB
			OBBox otherBox = boxA;

			return ComputeCollision(va, otherBox, vb, result, call);
		}
		break;
	case CollisionType_OBB:
		{
			//OBB vs OBB
			OBBCollisionPrimitive* obb = (OBBCollisionPrimitive*)otherPrimitive;
			const OBBox& otherBox = obb->GetBounds();

			return ComputeCollision(va, otherBox, vb, result, call);
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

bool OBBCollisionPrimitive::ComputeCollision(const Vector3& myVelocity, const OBBox& otherBox, const Vector3& velOther, CollisionResult& result, ICollisionCallback* call)
{
	float t1, t2;
	float tmax = 1;
	if ( OBBox::Intersects(m_box, myVelocity, otherBox, velOther, tmax, t1) )
	{

		int numCollisionPoints;
		Vector3 p[8];
		bool hasHit = OBBox::Find(tmax, myVelocity, velOther, m_box, otherBox, numCollisionPoints, t2, p);
		godzassert( hasHit ); //what??? should always be true!
		if( numCollisionPoints > 0 )
		{
			result.m_hitLocation = p[0];
			result.m_distance = (result.m_hitLocation - m_box.center).GetLength();
			result.m_normal = otherBox.GetNormalFromPoint(result.m_hitLocation);
		}
		else // both boxes are stuck on each other (time = 0)
		{
			OBBox::Find(tmax, myVelocity, velOther, m_box, otherBox, numCollisionPoints, t2, p);
			result.m_bStuck = true;
		}

		//just reject a move that collides 2 obbs for now
		//result.m_bStuck = true; //result.m_distance <= vb.GetLength();		//we are stuck in the geometry

		//find which normal we hit....
		call->OnCollisionOccured(result);
		return true;
	}

	return false;
}

bool OBBCollisionPrimitive::SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result)
{
	/*
	//Bugged!~!!!
	float tmax = 1.0f; //????
	float t; //contact time
	if (m_box.Intersects(tmax, triangle, velocity, ZERO_VECTOR, t) )
	{
		WPlane p(triangle[0], triangle[1], triangle[2]);
		result.m_normal = p.N;		
		t = t / tmax;
		result.m_distance = (velocity * t).GetLength();
		float velmag = velocity.GetLength();
		if (result.m_distance <= velmag)
		{
			result.m_normal = p.N;
			result.m_bStuck = result.m_distance <= EPSILON;		//we are stuck in the geometry
			return true;
		}
	
	}

	return false;
	*/

	//Note: Obb -> Sphere collision is just too inaccurate. need something better! fixme!
	WSphere s = m_box;
	WPlane p(triangle[0], triangle[1], triangle[2]);
	if( s.IntersectTriangle(triangle[0], triangle[1], triangle[2], p.N, velocity, result.m_hitLocation, result.m_distance) )
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

bool OBBCollisionPrimitive::Intersects(CollisionPrimitive* prim)
{
	switch(prim->GetCollisionType())
	{
	default:
		{
			//Default implementation does AABB vs AABB
			const WBox worldBound = m_box;
			WBox otherBound ;
			prim->GetBox(otherBound);

			return worldBound.IntersectsBox(otherBound);
		}
		break;
	case CollisionType_Sphere:
		{
			//TODO: could use a true OBB vs Sphere....
			SphereCollisionPrimitive* s = (SphereCollisionPrimitive*)prim;
			WSphere sphere = s->GetBounds();

			float t; //t doesn't matter when it's stationary at time 0
			WSphere shape = m_box;
			return sphere.IntersectsSphere(sphere, ZERO_VECTOR, ZERO_VECTOR, t);
		}
		break;
	case CollisionType_AABB:
		{
			//We convert the AABB into an OBB, this way we can do accurate collision detection
			//that fully utilizes our oriented shape
			AABBCollisionPrimitive* aabb = (AABBCollisionPrimitive*)prim;
			OBBox obb = aabb->GetBounds();
			return OBBox::Test(m_box, obb);
		}
		break;
	case CollisionType_OBB:
		{
			OBBCollisionPrimitive* obb = (OBBCollisionPrimitive*)prim;
			return OBBox::Test(m_box, obb->GetBounds());
		}
		break;
	}
}

void OBBCollisionPrimitive::SetBounds(const OBBox& bound)
{
	m_box=bound;

	if(m_boxShape != NULL)
	{
		delete m_boxShape;
	}

	m_boxShape = new btBoxShape( btVector3(bound.extent.x, bound.extent.y, bound.extent.z) );
	m_boxShape->setMargin(0.f);
}

btConvexShape* OBBCollisionPrimitive::GetShape()
{
	return m_boxShape;
}

void OBBCollisionPrimitive::Serialize(GDZArchive& ar)
{
	ArchiveVersion version(1,1);
	ar << version;

	if ( ar.IsSaving() )
	{
		ar << m_box;
	}
	else
	{
		OBBox box;
		ar << box;

		SetBounds(box);
	}
}


bool OBBCollisionPrimitive::Trace(const WRay& ray, TraceResult& result)
{
	float tmin = 0;
	if (m_box.Intersects(ray, tmin, result.hitLocation))
	{
		result.m_distance = (result.hitLocation - ray.origin).GetLength();
		return true;
	}

	return false;
}

void OBBCollisionPrimitive::TransformBy(const WMatrix16f& matrix)
{
	m_box.TransformBy(matrix);
}

void OBBCollisionPrimitive::TransformBy(const Vector3& p)
{
	m_box.center += p;
}

void OBBCollisionPrimitive::GetTransform(Vector3& pos, WMatrix3& matrix) const
{
	pos = m_box.center;
	matrix.SetX(m_box.axis[0]);
	matrix.SetY(m_box.axis[1]);
	matrix.SetZ(m_box.axis[2]);
}

void OBBCollisionPrimitive::GetTransform(WMatrix& matrix) const
{
	matrix.MakeIdentity();

	matrix.SetTranslation(m_box.center);
	matrix.SetX(m_box.axis[0]);
	matrix.SetY(m_box.axis[1]);
	matrix.SetZ(m_box.axis[2]);
}

void OBBCollisionPrimitive::SetTransform(const Vector3& pos, const WMatrix3& m)
{
	m_box.center = pos;
	m.GetAxes(m_box.axis[0], m_box.axis[1], m_box.axis[2]);
}

void OBBCollisionPrimitive::SetTransform(const WMatrix& m)
{
	m_box.center = m.GetTranslation();
	m.GetAxes(m_box.axis[0], m_box.axis[1], m_box.axis[2]);
}

void OBBCollisionPrimitive::SetPosition(const Vector3& pos)
{
	m_box.center = pos;
}

void OBBCollisionPrimitive::GetBox(WBox& box) const
{
	box = m_box;
}

void OBBCollisionPrimitive::GetSphere(WSphere& sphere) const
{
	sphere = m_box;
}
