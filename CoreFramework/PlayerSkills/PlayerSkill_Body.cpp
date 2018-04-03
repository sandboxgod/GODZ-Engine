/* ===========================================================
PlayerSkill_Body

All abilities that deal with the "Body". All skills that deal
with physical powers such as Strength, Running, Jumping, etc

Created Aug 21, '07 by Richard Osborne
Copyright (c) 2010
=============================================================
*/

#include "PlayerSkill_Body.h"

namespace GODZ
{

PlayerSkillWallWalk::PlayerSkillWallWalk()
{
	m_type = PlayerSkill_WallWalk;
	m_id = PlayerSkillHashCode_WallWalk;
	m_nameIndex = UNKNOWN_SKILL_NAME; //todo: code the string table
	m_maxLevel = DEFAULT_MAX_SKILL_LEVEL;
}

void PlayerSkillWallWalk::Use(WCharacter* skillUser)
{
	//what I envision happening here is that the Physics system will always query this skill
	//to see which 'surface' or direction the player wishes to align to. Perhaps if the player
	//walks off the surface they must realign back to default direction (down, -y)
}

PlayerSkillJump::PlayerSkillJump()
{
	m_type = PlayerSkill_Jump;
	m_id = PlayerSkillHashCode_Jump;
	m_nameIndex = UNKNOWN_SKILL_NAME; //todo: code the string table
	m_maxLevel = DEFAULT_MAX_SKILL_LEVEL;
	m_isBasicSkill = true;
}


}