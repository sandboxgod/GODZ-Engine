
#include "ProjectileTrigger.h"
#include "Projectile.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Core/WEntity.h>
#include <CoreFramework/Core/Layer.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{
ImplementGeneric(ProjectileTrigger)
REGISTER_OBJECT(ProjectileTrigger, ProjectileType, ProjectileTemplate)
REGISTER_HASHSTRING(ProjectileTrigger, RefNode)

void ProjectileTrigger::apply(WEntity* entity)
{
	if ( mTemplate == NULL )
		return;

	atomic_ptr<WEntity> projectile( new WEntity() );

	//compute physics --> velocity
	const WMatrix3& m = entity->GetOrientationMatrix();
	Vector3& dir = m.GetForward();
	dir.Normalize();

	if (mRefNode.isEmpty())
	{
		Vector3 loc = entity->GetLocation();
		loc += dir * -200;
		projectile->SetLocation(loc);
	}

	Vector3 vel = dir * -mTemplate->GetSpeed();
	projectile->SetVelocity(vel);

	entity->GetLayer()->AddActor(projectile);

	projectile->SetTemplate(mTemplate);

	atomic_ptr<ProjectileComponent> pc(new ProjectileComponent(entity, GetProjectileType()));
	projectile->AddEntityComponent(pc);
}

}