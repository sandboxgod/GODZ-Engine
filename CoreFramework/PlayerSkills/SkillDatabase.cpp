/* ===========================================================
SkillDatabase


Created Sept 7, '07 by Richard Osborne
Copyright (c) 2010
=============================================================
*/

#include "SkillDatabase.h"

namespace GODZ
{

SkillDatabase::SkillDatabase()
{
	m_skill[PlayerSkill_WallWalk] = &m_playerSkillWallWalk;
	m_skill[PlayerSkill_Jump] = &m_playerSkillJump;
}

PlayerSkill*	SkillDatabase::GetSkill(PlayerSkillType type)
{
	return m_skill[type];
}

}