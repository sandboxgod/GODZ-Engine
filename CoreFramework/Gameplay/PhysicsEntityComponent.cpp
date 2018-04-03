#include "PhysicsEntityComponent.h"
#include <CoreFramework/Core/WEntity.h>

namespace GODZ
{

PhysicsEntityComponent::PhysicsEntityComponent(WEntity* owner)
: EntityComponent(owner)
, m_physics(PhysicsType_Falling)
{
}

HashCodeID PhysicsEntityComponent::GetComponentName()
{
	return COMPONENT_NAME;
}

void PhysicsEntityComponent::FindBase(CollisionStatus physicsState)
{
	PhysicsObjectProxy* proxy = mOwner->GetPhysicsObject();

	if (proxy != NULL && proxy->IsValid() && proxy->HasLanded() && physicsState == CollisionStatus_BaseFound)
	{
		m_physics = PhysicsType_Walking;
		//NotifyLanded();
		proxy->ResetHasLanded();
	}
}

//Called anytime a physics frame is updated on our proxy
void PhysicsEntityComponent::UpdatePhysics(CollisionStatus physicsState)
{
	switch(m_physics)
	{
		default:
			break;
		case PhysicsType_Walking:
			{
			}
			break;
		case PhysicsType_Jumping:
			{
				FindBase(physicsState);
			}
			break;
		case PhysicsType_Falling:
			{
				FindBase(physicsState);
			}
			break;
	}
}

} //namespace GODZ