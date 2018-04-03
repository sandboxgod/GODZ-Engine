/* ===========================================================
	WCharacter

	Character that exists in world space. 

	Proverbs 3:9 "Honor the Lord with your wealth, with the
	firstfruits of all of your crops; then your barns will be
	filled to overflowing, and your vats will brim over with
	new wine."

	Created Feb 16, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WORLD_CHARACTER_H_)
#define _WORLD_CHARACTER_H_

#include <CoreFramework/Core/WEntity.h>
#include <CoreFramework/Animation/AnimationTable.h>
#include <CoreFramework/Core/PlayerSkill.h>

namespace GODZ
{
	//tweaks speed towards zero
	void AdjustSpeed(float &speed, float delta);

	//forward decl
	class WController;
	class CharacterTemplate;

	enum CharacterAttribute
	{
		CharacterAttribute_Intelligence,		//
		CharacterAttribute_Strength,			//strength of the character to pickup objects, etc
		CharacterAttribute_Tech,				//mechanical / automation skills (hacking?) gadgets
		CharacterAttribute_Fitness,				//character movement (Stamina bar)
		CharacterAttribute_Health,				//total number of hit points
		CharacterAttribute_Max
	};

	enum CharacterControllerType
	{
		CharacterControllerType_Player,
		CharacterControllerType_Mob
	};

	//Used to identify a character
	struct CharacterId
	{
		unsigned int m_id;
		CharacterControllerType m_type;
	};

	class GODZ_API WCharacter : public WEntity
	{
		DeclareGeneric(WCharacter, WEntity)

	public:
		WCharacter();

		virtual void		OnAddToWorld();

		void				SetHealth(float health);
		float				GetHealth();

		//analyzes animation data and constructs the AnimInfo Table
		virtual void		OnMeshChanged();

		PlayerSkillInfo&	GetPlayerSkillInfo(PlayerSkillType type) { return m_skills[type]; }

		//returns true if this character is currently playing the firing anim
		bool				IsFiring();

		//Loads Attributes for this character. Receives either a Player or Mob Id. This should be called anytime
		//this character is posessed by an entity
		void				LoadAttributes(CharacterId id);

		virtual void		OnAnimEnd(AnimSequence *pSequence);

		//Notifies this character it has taken damage
		virtual void		OnHit(float damage, WEntity* who);

		virtual void		OnTick(float dt);

		//plays desired animation (if not already playing)
		void				PlayAnim(HashCodeID kType);

		//play firing animation
		void				PlayFiring();

		//Sets the controller of this character.
		void				SetController(WController* controller);

		//Characters will coliide-and-slide by default
		virtual MotionType	GetMotionType() { return MotionType_Character; }

		virtual void OnRemoved();

	protected:
		bool m_bIsFiring;
		float m_health;					//health
		float m_endurance;				//fuel for our skills
		AnimationTable* m_pTable;		//animation table for attached anim instance
		WController* m_pController;		//the 'brains' of this character
		
		PlayerSkillInfo m_skills[PlayerSkill_Max];		//skillbook for this character.
		float m_attributes[CharacterAttribute_Max];		//Attributes that define this character's specializations

	};
}

#endif
