
#include "MActor.h"

namespace GODZ
{

MActor::MActor()
: mIsStandingOnPolygon(false)
, mPolyIndex(0)
, mMoveSpeed(600)
, m_orientation(1)
, m_rot(Quaternion::getIdentity())
{
}

void MActor::SetID(EntityID id)
{
	m_id = id;
}

bool MActor::IsStandingOnPolygon()
{
	return mIsStandingOnPolygon;
}

long MActor::GetNavMeshPolyIndex()
{
	return mPolyIndex;
}

void MActor::SetNavMeshPolygon(bool isStandingOnPolygon, long polyindex)
{
	mIsStandingOnPolygon = isStandingOnPolygon;
	mPolyIndex = polyindex;
}

void MActor::UnsetRotation()
{
	m_isRotationDirty.UnSet();
}

bool MActor::IsRotationDirty()
{
	return m_isRotationDirty.IsSet();
}

void MActor::GetRotation(Quaternion& rot)
{
	m_rot.get(rot);
}

void MActor::SetRotation(const Quaternion& rot)
{
	m_rot.set(rot);
	m_isRotationDirty.Set();
}

void MActor::SetOrientation(const WMatrix3& m)
{
	m_orientation.set(m);
	m_isRotationMatrixDirty.Set();
}

void MActor::GetOrientation(WMatrix3& m)
{
	m_orientation.get(m);
}

void MActor::UnsetOrientation()
{
	m_isRotationMatrixDirty.UnSet();
}

bool MActor::IsOrientationDirty()
{
	return m_isRotationMatrixDirty.IsSet();
}

void MActor::UnsetAnimation()
{
	m_isAnimationDirty.UnSet();
}

bool MActor::IsAnimationDirty()
{
	return m_isAnimationDirty.IsSet();
}

void MActor::PlayAnimation(AnimData& anim)
{	
	m_anim.set(anim);
	m_isAnimationDirty.Set();
}

void MActor::GetAnimation(AnimData& data)
{
	m_anim.get(data);
}

void MActor::SetPhysicsObject(PhysicsObjectProxy& proxy)
{
	m_proxy.set(proxy);
}

PhysicsObjectProxy MActor::GetPhysicsObject()
{
	return m_proxy;
}

void MActor::SetMoveSpeed(long speed)
{
	mMoveSpeed = speed;
}

long MActor::GetMoveSpeed()
{
	return mMoveSpeed;
}

void MActor::SetCollision(CollisionPrimitive* col)
{
	WSphere s;
	col->GetSphere(s);
	m_col = s;
}

void MActor::GetSphere(WSphere& s)
{
	s = m_col;
}

}