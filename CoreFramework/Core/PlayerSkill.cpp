/* ===========================================================
PlayerSkill

A skill that belongs to a character (Skill based system)
whereas every power simply has a rating from 0-100%

Created Aug 16, '07 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "PlayerSkill.h"


namespace GODZ
{

//weird thing about skill based systems
void PlayerSkillInfo::AddSkillUp(float skillUp)
{
	
	if (m_level >= m_skillDefinition->GetMaxLevel())
	{
		//avoid database update
		return;
	}
	

	//compute the falloff value. Falloff is basically the closer a skill gets to 100% the longer it takes.
	//float falloff = MAX_SKILL_VALUE - m_power;

	//The higher the level of the skill, the longer it takes. If the Level is currently 0, then it would be 100%
	float level_falloff = 1.0f - (m_level / m_skillDefinition->GetMaxLevel());

	//note: if we want to ever lighten this falloff value well just do a maxf(falloff, 0.5f), etc. That
	//will make it so falloff can't go over that for example.

	m_power += skillUp * level_falloff; //* falloff

	if (m_power >= MAX_SKILL_VALUE)
	{
		m_power = 0;		//reset skill
		m_level++;
	}

	//todo: write to database the new value for this player skill...
}


void PlayerSkillInfo::Init(PlayerSkill* skill)
{
	m_level = 0;
	m_power = 0;
	m_isUnlocked = false;

	//go get the registered object for this skill
	m_skillDefinition = skill;
}


float PlayerSkillInfo::GetPowerScaled() const
{
	godzassert(m_skillDefinition != NULL);
	return m_level / m_skillDefinition->GetMaxLevel();
}




PlayerSkill::PlayerSkill()
: m_isBasicSkill(false)
, m_maxLevel(DEFAULT_MAX_SKILL_LEVEL)
, m_id(0)
, m_nameIndex(UNKNOWN_SKILL_NAME)
, m_type(PlayerSkill_Unknown)
{

}


} //namespace GODZ

