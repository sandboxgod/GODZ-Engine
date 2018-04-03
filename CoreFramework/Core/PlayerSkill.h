/* ===========================================================
PlayerSkill

A skill that belongs to a character (Skill based system)
whereas every power simply has a rating from 0-100%

Created Aug 16, '07 by Richard 'vajuras' Osborne
Copyright (c) 2010
========================================================== 
*/

#if !defined(PLAYERSKILL_H)
#define PLAYERSKILL_H

#include "Godz.h"

//Note: I'm thinking to just hardcode all the possible player skills. define an Enum an each one is assigned
//a number. A character will have a 'skillbook' in which indicates his strength for a known skill only. This
//skillbook will inform us the rating of the skill for that player

namespace GODZ
{
	class WCharacter;
	class PlayerSkill;

	//do not use this enum for anything besides just referring to a skill type. Expect this thing to change order
	//*often*. So don't serialize this enum use the unique hash code the PlayerSkill Object will give you for that.
	enum PlayerSkillType
	{
		PlayerSkill_Unknown,
		PlayerSkill_Jump,			//(Body) power to jump high (Leaping)
		PlayerSkill_Sprint,			//(Body) faster foot movement at cost to endurance
		PlayerSkill_Strength,		//(Body) strength of this character (for melee attacks)
		PlayerSkill_Hearing,		//(Body) enhanced senses (supernatural)
		PlayerSkill_WallWalk,		//(Body) ability to walk on a wall (wall crawler)
		PlayerSkill_Fire,			//ability to control the fire element
		PlayerSkill_Water,			//Ice attacks (offense)
		PlayerSkill_Air,			//like lightning
		PlayerSkill_Earth,			//defense, rock
		PlayerSkill_Fly,			//(Air) Flight, power to use wind to help you move
		PlayerSkill_Wall,			//(shape) ability to erect a force wall using a primary element
		PlayerSkill_Ball,			//(shape) ability to fire off a projectile (homing missile)
		PlayerSkill_Bees,			//(shape) small little swarm of bolts
		PlayerSkill_LockOn,			//ability to lock-on to a target (firearms)
		PlayerSkill_HealOther,		//healing others (Cannot Target self)
		PlayerSkill_Black,			//negation, death, poison
		PlayerSkill_HomingAI,		//The artificial intelligence (accuracy) of a homing projectile	
		PlayerSkill_AccuracyFalloff, //The range at which homing projectiles sort of lose range
		PlayerSkill_Max
	};

	//CRC32 hash codes are all listed here that way we can manually check for collisions if we want.
	//see http://whatsmyip.org/hash_generator/
	enum PlayerSkillHashCodes
	{
		PlayerSkillHashCode_Jump			= 0xb6241c89,
		PlayerSkillHashCode_WallWalk		= 0x00ba365c,
		PlayerSkillHashCode_FireBall		= 0xacc794b9,
	};

	static const float MAX_SKILL_VALUE =			1.0f;
	static unsigned int UNKNOWN_SKILL_NAME =		0;

	//Default max skill level. Skills can vary in max levels. Many simple skills will possibly
	//max out at 1.
	static const unsigned char DEFAULT_MAX_SKILL_LEVEL =	4;

	//tells me how powerful this character is using a particular skill
	class GODZ_API PlayerSkillInfo
	{
	public:

		//The player has just earned some points towards increasing a skill...
		void	AddSkillUp(float skillUp);

		//Returns true if this power is 'available' to this character
		bool	IsUnlocked() const { return m_isUnlocked ; }

		//Returns the 'strength' of this power scaled from 0 -> 1.0 (represents 0-100%)
		float	GetPowerScaled() const;

		//Returns the 'strength' of this skill
		unsigned char	GetSkillLevel() const { return m_level; }
		void	SetSkillLevel(unsigned char level) { m_level = level;}		

		void	Init(PlayerSkill* skill);

	private:
		float m_power;						//The power level of this skill. Goes from 0-1.0f (0 to 100%)

		bool m_isUnlocked;

		//Level of this Skill. IF it's locked this skill will be 0. If this skill is unlocked for the player
		//the value will be 1 or greater
		unsigned char m_level;

		PlayerSkill* m_skillDefinition;
	}; 


	//Special power (static representation of the skill). Contains all the default settings for a player
	//skill. All the data stored within this class is common and shared (should be treated as a const object)
	class GODZ_API PlayerSkill
	{
	public:

		//Inform this power what 'type' of skill it is
		PlayerSkill();
		virtual ~PlayerSkill() {}

		//Whatever action in the game world this skill does it should be performed here
		virtual void Use(WCharacter* skillUser) {};

		//returns the unique identifier of this player skill
		unsigned int GetId() { return m_id; }

		//Returns the Maximum Level the skill can reach
		unsigned char GetMaxLevel() { return m_maxLevel; }

	protected:		
		PlayerSkillType		m_type;
		unsigned int		m_nameIndex;			//UI name of this skill (string table lookup)

		//Unique Hash Code that identifies this player skill. This is the value we should use for serialization.
		unsigned int		m_id;	

		//Maximum Level set for this skill
		unsigned char		m_maxLevel;

		//flag indicates if this skill should be available by default to the player
		bool				m_isBasicSkill;
	};


}



#endif PLAYERSKILL_H