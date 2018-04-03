/* ===========================================================
Projectile

=============================================================
*/

#pragma once

#include <CoreFramework/Core/EntityComponent.h>
#include <CoreFramework/Core/EntityTemplate.h>
#include "DamageTypes.h"

namespace GODZ
{
	/*
	* Describes properties for a Type of projectile
	*/
	class GODZ_API ProjectileTemplate : public EntityTemplate
	{
		DeclareGeneric(ProjectileTemplate, EntityTemplate)

	public:
		ProjectileTemplate();

		float GetDamage() const;
		void SetDamage(float v);

		float GetSpeed() { return mSpeed; }
		void SetSpeed(float s) { mSpeed = s; }

		virtual MotionType		GetMotionType() { return MotionType_Projectile; }

	protected:
		float mDamage;

		//amount of Qi Energy contained within this projectile. I was thinking about making it so each impact
		//will deduct energy from this bolt. Also, if a player blocks this bolt the amount of Qi energy invested
		//will be deducted from the other player.
		float mEnergy;

		DamageType mDamageType;			//type of damage we going to inflict
		float mSpeed;
	};

	/*
	* Setup to be a component so that it can be combined with Characters, etc
	* to possibly "weaponize" them in interesting ways
	*/
	class GODZ_API ProjectileComponent : public EntityComponent
	{
	public:
		static const int COMPONENT_NAME = 1054760822; // ProjectileComponent
		virtual HashCodeID GetComponentName() { return COMPONENT_NAME; }

		ProjectileComponent(WEntity* owner, ProjectileTemplate* projectileTemplate);
		virtual ~ProjectileComponent();

		virtual void OnTick(float deltaTime);

	protected:
		const ProjectileTemplate* mTemplate;
	};
}
