/* ===========================================================
SkillDatabase


Created Sept 7, '07 by Richard Osborne
Copyright (c) 2010
=============================================================
*/

#if !defined(SKILLDATABASE_H)
#define SKILLDATABASE_H

#include "PlayerSkill_Body.h"


namespace GODZ
{
	//Contains a collection of skill definitions
	class GODZ_API SkillDatabase
	{
	public:
		SkillDatabase();
		PlayerSkill*	GetSkill(PlayerSkillType type);


	protected:		
		PlayerSkill* m_skill[PlayerSkill_Max];		//skillbook for this character.


		//Declare all the skills here
		PlayerSkillWallWalk m_playerSkillWallWalk;
		PlayerSkillJump m_playerSkillJump;
	};
}



#endif SKILLDATABASE_H