/* ===========================================================
	Entity Component

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#pragma once

#include "GenericReferenceCounter.h"
#include <CoreFramework/Collision/Physics.h>
#include "GenericObject.h"

namespace GODZ
{
	class WEntity;

	class GODZ_API EntityComponent : public GenericObject
	{
		DeclareGenericBase(EntityComponent, GenericObject)

	public:
		virtual void OnTick(float deltaTime) {}
		virtual void UpdatePhysics(CollisionStatus physicsState) {}
		virtual void OnSetPhysicsObject() {}

		virtual HashCodeID GetComponentName();
		void SetOwner(WEntity* owner);

	protected:
		EntityComponent()
			: mOwner(NULL)
		{
		}

		EntityComponent(WEntity* owner)
			: mOwner(owner)
		{
		}

		WEntity* mOwner;
	};
}

