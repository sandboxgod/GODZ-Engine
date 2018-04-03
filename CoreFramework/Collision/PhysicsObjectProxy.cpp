
#include "PhysicsObjectProxy.h"
#include "PhysicsObject.h"
#include "PhysicsWorld.h"
#include <CoreFramework/Core/MActor.h>
#include "CollisionSystems.h"

namespace GODZ
{

PhysicsObjectProxy::PhysicsObjectProxy()
: m_physicsObject(NULL)
{
}

PhysicsObjectProxy::PhysicsObjectProxy(PhysicsObject* obj)
: m_physicsObject(obj)
{
}

void PhysicsObjectProxy::AddChild(PhysicsObjectProxy& child)
{
	m_physicsObject->AddChild(child.m_physicsObject);
}

void PhysicsObjectProxy::GetPosition(Vector3& pos)
{
	m_physicsObject->GetPosition(pos);
}

void PhysicsObjectProxy::SetPositionUnchecked(const Vector3& pos)
{
	m_physicsObject->SetPositionUnchecked(pos);
}

long PhysicsObjectProxy::GetFrameNumber() const
{
	return m_physicsObject->GetFrameNumber();
}

long PhysicsObjectProxy::GetRotationFrameNumber() const
{
	return m_physicsObject->GetRotationFrameNumber();
}

bool PhysicsObjectProxy::IsValid()
{
	return m_physicsObject != NULL;
}

void PhysicsObjectProxy::GetDesiredVelocity(Vector3& v)
{
	m_physicsObject->GetDesiredVelocity(v);
}

void PhysicsObjectProxy::SetDesiredVelocity(const Vector3& v)
{
	m_physicsObject->SetDesiredVelocity(v);
}

void PhysicsObjectProxy::GetVelocity(Vector3& v)
{
	m_physicsObject->GetVelocity(v);
}

void PhysicsObjectProxy::GetRotation(Matrix3& rot)
{
	m_physicsObject->GetRotation(rot);
}

void PhysicsObjectProxy::SetRotation(const Matrix3& rot)
{
	m_physicsObject->SetRotation(rot);
}

void PhysicsObjectProxy::GetWorldTransform(Vector3& pos, Matrix3& rot)
{
	m_physicsObject->GetWorldTransform(pos, rot);
}

void PhysicsObjectProxy::SetAngularVelocity(const AxisAngle& ang)
{
	m_physicsObject->SetAngularVelocity(ang);
}

void PhysicsObjectProxy::SetMotionType(MotionType type)
{
	m_physicsObject->SetMotionType(type);
}

MotionType PhysicsObjectProxy::GetMotionType()
{
	return m_physicsObject->GetMotionType();
}

CollisionStatus PhysicsObjectProxy::GetCollisionState()
{
	return m_physicsObject->GetCollisionState();
}

void PhysicsObjectProxy::GetCollisions(PhysicsCollisionResults* list)
{
	m_physicsObject->GetCollisions(*list);
}

void PhysicsObjectProxy::ClearCollisions()
{
	m_physicsObject->ClearCollisions();
}

bool PhysicsObjectProxy::IsInWorld() const
{
	return m_physicsObject->IsInWorld();
}

void PhysicsObjectProxy::ResetHasLanded()
{
	m_physicsObject->ResetHasLanded();
}

bool PhysicsObjectProxy::HasLanded() const
{
	return m_physicsObject->GetHasLanded();
}

MActor* PhysicsObjectProxy::GetMActor()
{
	//TODO: return m_physicsObject->GetMActor();
	return NULL;
}

EntityID PhysicsObjectProxy::GetID() const
{
	return m_physicsObject->GetID();
}

} //namespace GODZ


