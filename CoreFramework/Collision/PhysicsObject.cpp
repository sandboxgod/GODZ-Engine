
#include "PhysicsObject.h"
#include "PhysicsWorld.h"
#include <CoreFramework/Math/vectormath.h>
#include <CoreFramework/Math/BulletMathUtil.h>
#include <CoreFramework/Collision/CollisionResult.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/NarrowPhaseCollision/btSubsimplexConvexCast.h>
#include <BulletCollision/NarrowPhaseCollision/btPointCollector.h>
#include <BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h>

namespace GODZ
{

PhysicsObject::PhysicsObject(CollisionPrimitive* primitive)
: m_primitive(NULL)
, m_status(CollisionStatus_NotSet)
, m_frame(0)
, m_rotFrame(0)
, m_motionType(MotionType_Character)
, m_currentlyUpdating(false)
, m_hasLanded(false)
, m_region(NULL)
, m_parent(NULL)
, m_rot(Matrix3::Identity())
, m_goalAngularVelocity(AxisAngle::Zero())
, m_goalVel(Vector3::Zero())
, m_vel(Vector3::Zero())
, m_collisionObject(NULL)
{
	m_collisionObject = new btCollisionObject();

	//get a private copy of this primitive
	m_primitive = primitive->GetClone();

	/*
	if (m_primitive->GetShape() != NULL)
	{
		m_collisionObject->setCollisionShape(m_primitive->GetShape());
	}
	*/
}

//NOTE: Careful; the way things is currently designed this object can possibly be deleted in another thread
PhysicsObject::~PhysicsObject()
{
	m_primitive = NULL;
	m_children.clear();
	delete m_collisionObject;
	m_collisionObject = NULL;
}

//lock this resource for updates by the physics thread
void PhysicsObject::SetBusy(bool isBusy)
{
	m_currentlyUpdating = isBusy;

	if (m_nextPositionLock.IsSet())
	{
		Vector3 p;
		m_nextPos.get(p);

		m_pos = p;
		m_nextPositionLock.UnSet();
	}
}

//Should be called by Physics Thread only after moving along the object
void PhysicsObject::UpdateCollisionObject()
{
	/*
	//copy over the variables into our internal collision object
	Matrix3 m;
	m_rot.get(m);

	Vector3 p;
	m_pos.get(p);

	m_collisionObject->getWorldTransform().setBasis( BulletMathUtil::ConvertTo(m) );
	m_collisionObject->getWorldTransform().setOrigin( BulletMathUtil::ConvertTo(p) );
	*/

	//TODO: Set linear & angular velocity on rigid bodies
}

bool PhysicsObject::Collide(const Matrix4* invMat, const Vector3& velA, btConvexShape* shape, const Vector3& posB, const Vector3& velB, const Matrix3& initOrientation, const Matrix3& destOrientation, CollisionResult& result)
{
	PhysicsWorld* world;
	m_world.get(world);

	Matrix3 m;
	Vector3 p;
	GetWorldTransform(p, m);

	//Apply relative offset
	p += m_origin;

	//If valid, used to transform this physicsObject relative to something else...
	//For example, this is useful for transforming this phys Object into object space
	//where it can be collided against a static polygon soup
	if( invMat != NULL )
	{
		invMat->Mul(p);
	}

	Matrix3 initMat(1);
	return world->Collide( shape, p, velA, initMat, initMat,
		shape, posB, velB, initMat, initMat, result);
}

bool PhysicsObject::Collide(const Vector3& velA, PhysicsObject* other, const Vector3& velB, CollisionResult& result)
{
	Matrix3 initRot;
	Vector3 posOther;
	other->GetWorldTransform(posOther, initRot);

	//Apply relative offset
	posOther += other->m_origin;

	//fixme! Need to compute destRotation, need DeltaTime....! TODO: Perhaps we should
	//cache the current deltaTime within the physicsWorld so this object can query!
	return Collide( NULL, velA, other->GetCollisionModel()->GetShape(), posOther, velB, initRot, initRot, result);
}

bool PhysicsObject::IsParent(PhysicsObject* parent)
{
	PhysicsObject* temp = m_parent;
	while( temp != NULL)
	{
		if( temp == parent )
		{
			return true;
		}

		temp = temp->m_parent;
	}

	return false;
}

//Sets object position w/o checking for collisions....
void PhysicsObject::SetPositionUnchecked(const Vector3& pos)
{
	if (m_currentlyUpdating)
	{
		m_nextPositionLock.Set();
		m_nextPos = pos;
	}
	else
	{
		m_pos = pos;
	}
}

void PhysicsObject::GetPosition(Vector3& pos)
{
	m_pos.get(pos);
}

void PhysicsObject::SetRotation(const Matrix3& rot)
{
	if (m_currentlyUpdating)
	{
		m_nextRotationLock.Set();
		m_nextRot = rot;
	}
	else
	{
		m_rot = rot;
	}
}

void PhysicsObject::GetRotation(Matrix3& rot)
{
	m_rot.get(rot);
}

void PhysicsObject::SetAngularVelocity(const AxisAngle& angVel)
{
	m_goalAngularVelocity = angVel;
}

void PhysicsObject::GetVelocity(Vector3& vel)
{
	m_vel.get(vel);
}

void PhysicsObject::GetDesiredVelocity(Vector3& vel)
{
	m_goalVel.get(vel);
}

void PhysicsObject::SetDesiredVelocity(const Vector3& v)
{
	//goal velocity is only used once during a physics update. Thus, other threads can muck with this
	//value at anytime and the change will be grabbed next physics update
	m_goalVel = v;
}

long PhysicsObject::GetFrameNumber() const
{
	return m_frame;
}

long PhysicsObject::GetRotationFrameNumber() const
{
	return m_rotFrame;
}

void PhysicsObject::SetMotionType(MotionType type)
{
	m_motionType = type;
}

MotionType PhysicsObject::GetMotionType() const
{
	long v = m_motionType;
	return (MotionType)v;
}

CollisionStatus PhysicsObject::GetCollisionState() const
{
	long value = m_status;
	return (CollisionStatus)value;
}

void PhysicsObject::GetCollisions(PhysicsCollisionResults& list)
{
	m_collisions.GetCopy(list);
}

void PhysicsObject::ClearCollisions()
{
	m_collisions.Clear();
}

bool PhysicsObject::IsInWorld() const
{
	return m_world != NULL;
}

void PhysicsObject::ResetHasLanded()
{
	m_hasLanded = false;
}

void PhysicsObject::AddChild(GenericReference<PhysicsObject> child)
{
	m_children.push_back(child);
	child->m_parent = this;
}

void PhysicsObject::GetWorldTransform(Vector3& pos, Matrix3& rot)
{
	TransformState tm;
	m_worldTm.get(tm);
	pos = tm.m_pos;
	rot = tm.m_rot;
}


} //namespace GODZ


