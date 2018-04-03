
#include "Projectile.h"
#include <CoreFramework/Core/LevelNode.h>
#include <CoreFramework/Core/WEntity.h>
#include <CoreFramework/Collision/PhysicsObjectProxy.h>
#include <CoreFramework/Collision/CollisionSystems.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{

ImplementGeneric(ProjectileTemplate)
REGISTER_FLOAT(ProjectileTemplate, Damage)
REGISTER_FLOAT(ProjectileTemplate, Speed)

ProjectileTemplate::ProjectileTemplate()
	: mDamage(0)
	, mEnergy(0)
	, mDamageType(DamageType_None)
	, mSpeed(1200)
{
}

float ProjectileTemplate::GetDamage() const
{
	return mDamage;
}

void ProjectileTemplate::SetDamage(float v)
{
	mDamage = v;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

ProjectileComponent::ProjectileComponent(WEntity* owner, ProjectileTemplate* projectileTemplate)
	: EntityComponent(owner)
	, mTemplate(projectileTemplate)
{

}

ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::OnTick(float deltaTime)
{
	PhysicsObjectProxy* physicsObj = mOwner->GetPhysicsObject();
	PhysicsCollisionResults results;
	physicsObj->GetCollisions(&results);

	size_t numResults = results.Size();
	if (numResults > 0)
	{
		for (size_t i = 0; i < numResults; i++)
		{
			PhysicsCollisionResult& r = results.GetResult(i);

			//don't allow projectiles to injure the owner by default
			if (r.m_id != mOwner->GetEntityID())
			{
				//find this actor...
				atomic_weak_ptr<WEntity> w = mOwner->GetLevel()->FindActor(r.m_id);
				atomic_ptr<WEntity> ptr = w.lock();
				if (ptr != NULL)
				{
					ptr->OnHit(mTemplate->GetDamage(), mOwner);
				}
			}
		}

		physicsObj->ClearCollisions();
	}
}

} //namespace GODZ