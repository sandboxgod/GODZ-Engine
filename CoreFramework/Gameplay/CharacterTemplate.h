


#pragma once
#include <CoreFramework/Core/EntityTemplate.h>

namespace GODZ
{
	/*
	* Common Settings for all characters of a certain Type
	*/
	class GODZ_API CharacterTemplate : public EntityTemplate
	{
		DeclareGeneric(CharacterTemplate, EntityTemplate)

	public:
		CharacterTemplate();

		float GetMaxHealth() { return mMaxHealth; }
		void SetMaxHealth(float health) { mMaxHealth = health; }

		virtual MotionType		GetMotionType() { return MotionType_Character; }

	protected:
		float mMaxHealth;			//max health
	};
}
