
#include "WPhysicsVolume.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>


namespace GODZ
{

ImplementGeneric(WPhysicsVolume)

REGISTER_VECTOR(WPhysicsVolume, Gravity)


WPhysicsVolume::WPhysicsVolume()
: gravity(0.0f, -200.0f, 0.0f) //gravity force
{
	m_bCollideWithWorld=false;
}


const Vector3& WPhysicsVolume::GetGravity()
{
	return gravity;
}

void WPhysicsVolume::SetGravity(const Vector3& grav)
{
	gravity = grav;
}




} //namespace