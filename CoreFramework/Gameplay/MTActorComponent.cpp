
#include "MTActorComponent.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Core/ClassRegistry.h>
#include <CoreFramework/Core/GenericPackage.h>
#include <CoreFramework/Core/WEntity.h>
#include <CoreFramework/Core/NavigationMesh.h>
#include <CoreFramework/Core/Layer.h>
#include <CoreFramework/Collision/PhysicsObjectProxy.h>

namespace GODZ
{

MTActorComponent::MTActorComponent(WEntity* owner)
: EntityComponent(owner)
, m_polyIndex(0)
, m_standingOnPolygon(false)
{
	mActor = new MActor();
	mActor->SetID(owner->GetRuntimeID());
}

MTActorComponent::~MTActorComponent()
{
}

HashCodeID MTActorComponent::GetComponentName()
{
	return COMPONENT_NAME;
}

void MTActorComponent::UpdatePhysics(CollisionStatus physicsState)
{
	//when the entity moves, check to see where we're standing....
	NavigationMesh& navmesh = mOwner->GetLayer()->GetNavigationMesh();
	WRay ray;
	ray.dir = mOwner->GetOrientationMatrix().GetUp();
	ray.dir.Invert();
	ray.origin = mOwner->GetLocation();
	m_standingOnPolygon = navmesh.IsPolygonHit(ray, m_polyIndex);

	//make sure its a valid downcast
	godzassert(m_polyIndex < LONG_MAX);
	long index = static_cast<long>(m_polyIndex);

	mActor->SetNavMeshPolygon(m_standingOnPolygon, index);
}

void MTActorComponent::OnSetPhysicsObject()
{
	if(mOwner->GetPhysicsObject() != NULL)
		mActor->SetPhysicsObject(*mOwner->GetPhysicsObject());
}

}