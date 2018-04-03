

#include "AABBCollisionPrimitive.h"
#include "OBBCollisionPrimitive.h"
#include "RayCollider.h" 
#include "ICollisionCallback.h"
#include "GeoBuilder.h"
#include <CoreFramework/Core/RenderDeviceEvent.h>
#include <CoreFramework/Core/SceneData.h>
#include <CoreFramework/Math/WPlane.h>
#include <CoreFramework/Math/OBBox.h>
#include <float.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

using namespace GODZ;

AABBCollisionPrimitive::AABBCollisionPrimitive()
: m_boxShape(NULL)
{
}

AABBCollisionPrimitive::~AABBCollisionPrimitive()
{
	if( m_boxShape != NULL )
	{
		delete m_boxShape;
		m_boxShape = NULL;
	}
}

Future<GenericReference<RenderDeviceObject> > AABBCollisionPrimitive::GetRenderObject( void ) const
{
	TriMeshEvent* tevent = new TriMeshEvent();

	Color4f c(0.2f, 0.2f, 1.0f, 1.0f);
	tevent->color = c;

	//make a new box thats center at origin (0,0,0)
	WBox b;
	b.SetDimensions(ZERO_VECTOR, m_box.GetExtent());

	//Matrix will translate us to the current world position
	WMatrix m(1);
	m.SetTranslation( m_box.GetCenter() );
	tevent->m = m;

	//Build polygons in object space...
	Vector3 min = b.GetMin();
	Vector3 max = b.GetMax();
	GeoBuilder::BuildBoxMesh(min, max, tevent->m_mesh);

	Future<GenericReference<RenderDeviceObject> > renderFuture = tevent->m_renderDeviceObject;
	SceneData::AddRenderDeviceEvent(tevent);

	return renderFuture;
}

void AABBCollisionPrimitive::GetBox(WBox& box) const
{
	box = m_box;
}

void AABBCollisionPrimitive::GetSphere(WSphere& sphere) const
{
	sphere = m_box;
}

GenericReference<CollisionPrimitive> AABBCollisionPrimitive::GetClone()
{
	AABBCollisionPrimitive* newModel = new AABBCollisionPrimitive();
	newModel->SetBounds(m_box);
	newModel->SetName( m_name );
	return newModel;
}


bool AABBCollisionPrimitive::SweepHit(CollisionPrimitive* otherPrimitive, const Vector3& va, const Vector3& vb, CollisionResult& result, ICollisionCallback* call)
{
	switch(otherPrimitive->GetCollisionType())
	{
	default:
		{
			WBox boxA;
			otherPrimitive->GetBox(boxA);

			float u0;
			float u1;

			if (WBox::IntersectsMovingBox(m_box, vb, boxA, va, u0, u1))
			{
				//compute hitlocation
				float t = u0;

				//most of the time (maybe always), you want time of first contact
				//if (t == 0.0f)
				//	t = u1;

				//TODO: best guess...
				result.m_hitLocation = m_box.GetCenter() + ( vb * t );

				//TODO: maybe look into making m_distance squared...
				result.m_distance = (vb * t).GetLength();

				//compute the nearest plane (used for collide & slide). If we don't specify a plane to slide along,
				//the character will make an abrupt stop when it collides with another character.
				/*
				result.m_plane.N = vb;
				result.m_plane.N.Normalize();
				result.m_plane.N.Invert();
				result.m_plane.N = WBox::GetNearPlane(result.m_plane.N);
				*/

				result.m_normal = m_box.GetNormalFromPoint(result.m_hitLocation);

				//result.m_bStuck = true;

				call->OnCollisionOccured(result);
				return true;
				/*
				else
				{
					//false positive?
					WBox::IntersectsMovingBox(boxA, va, m_box, vb, u0, u1);

					//try inverse????
					WBox::IntersectsMovingBox(m_box, vb, boxA, va, u0, u1);
					_asm nop;
				}
				*/
			}
		}
		break;

	case CollisionType_AABBTree:
		{
			//Let AABBTree handle the collision query
			return otherPrimitive->SweepHit(this, vb, va, result, call);
		}
		break;
	case CollisionType_OBB:
		{
			//OBB vs OBB
			OBBox myBox = m_box;

			OBBCollisionPrimitive* obb = (OBBCollisionPrimitive*)otherPrimitive;
			const OBBox& box = obb->GetBounds();

			/*
			if(m_name.getHashCode() > 0)
			{
				Log("Test other ---> [%d], %s %f %f %f\n", otherPrimitive->GetName().getHashCode(), otherPrimitive->GetName().c_str(), va.x, va.y, va.z );
			}
			*/

			float t;
			float tmax = 1;
			if ( OBBox::Intersects(myBox, vb, box, va, tmax, t) )
			{
				//TODO: best guess...
				result.m_hitLocation = myBox.center + myBox.extent + ( vb * t );

				//TODO: maybe look into making m_distance squared...
				result.m_distance = (vb * t).GetLength();

				//result.m_normal = myBox.GetNormalFromPoint(result.m_hitLocation);
				result.m_normal.MakeZero();

				//just reject a move that collides 2 obbs for now
				result.m_bStuck = true;

				call->OnCollisionOccured(result);
				return true;
				/*
				else
				{
					

					//false positive?
					OBBox::Intersects(myBox, vb, box, va, tmax, t);
					_asm nop;
				}
				*/
			}
		}
		break;
	}



	return false;
}

bool AABBCollisionPrimitive::SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result)
{
	//TODO: Need moving aabb vs triangle

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

bool AABBCollisionPrimitive::Intersects(CollisionPrimitive* prim)
{
	WBox otherBound;
	prim->GetBox(otherBound);

	return m_box.IntersectsBox(otherBound);
}

void AABBCollisionPrimitive::SetBounds(const WBox& bound)
{
	m_box=bound;

	if(m_boxShape != NULL)
	{
		delete m_boxShape;
	}

	const Vector3& extent = bound.GetExtent();
	m_boxShape = new btBoxShape( btVector3(extent.x, extent.y, extent.z) );
	m_boxShape->setMargin(0.f);
}

void AABBCollisionPrimitive::Serialize(GDZArchive& ar)
{
	ArchiveVersion version(1,1);
	ar << version;

	m_box.Serialize(ar);
	SetBounds(m_box);
}


bool AABBCollisionPrimitive::Trace(const WRay& ray, TraceResult& result)
{
	if (RayAABB4(m_box, ray.origin, ray.dir, result.hitLocation))
	{
		return true;
	}

	return false;
}

void AABBCollisionPrimitive::TransformBy(const WMatrix16f& matrix)
{
	m_box.Transform(matrix);
}

void AABBCollisionPrimitive::TransformBy(const Vector3& pos)
{
	m_box.Transform(pos);
}

void AABBCollisionPrimitive::GetTransform(Vector3& pos, WMatrix3& matrix) const
{
	pos = m_box.GetCenter();
	matrix.MakeIdentity();
}

void AABBCollisionPrimitive::GetTransform(WMatrix& matrix) const
{
	matrix.MakeIdentity();
	matrix.SetTranslation(m_box.GetCenter());
}

void AABBCollisionPrimitive::SetTransform(const Vector3& pos, const WMatrix3& matrix)
{
	m_box.SetCenter(pos);
}

void AABBCollisionPrimitive::SetTransform(const WMatrix& m)
{
	m_box.SetCenter( m.GetTranslation() );
}

void AABBCollisionPrimitive::SetPosition(const Vector3& pos)
{
	m_box.SetCenter(pos);
}

btConvexShape* AABBCollisionPrimitive::GetShape()
{
	return m_boxShape;
}
