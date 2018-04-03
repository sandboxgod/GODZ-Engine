#pragma once

#include <CoreFramework/Core/EntityComponent.h>

namespace GODZ
{
class WEntity;

class PhysicsEntityComponent : public EntityComponent
{
public:
	static const int COMPONENT_NAME = 2962822171; // PhysicsEntityComponent

	//Physics states (note: don't change order of physics states otherwise you risk
	//breaking packages- godz scene files, etc)
	enum PhysicsType
	{
		PhysicsType_None,
		PhysicsType_Falling,		//object is falling
		PhysicsType_Walking,		//entity is walking
		PhysicsType_Projectile,	//projectile- just flies straight...
		PhysicsType_Flying,		//flying
		PhysicsType_Jumping
	};

public:
	PhysicsEntityComponent(WEntity* owner);

	virtual HashCodeID GetComponentName();
	virtual void UpdatePhysics(CollisionStatus physicsState);

	PhysicsType GetPhysics() { return m_physics; }
	void SetPhysics(PhysicsType type) { m_physics = type; }

protected:
	void FindBase(CollisionStatus physicsState);

protected:
	PhysicsType m_physics;
};

}
