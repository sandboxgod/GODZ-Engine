/* ===========================================================
PlayerSkill_Body

All abilities that deal with the "Body". All skills that deal
with physical powers such as Strength, Running, Jumping, etc

Created Aug 21, '07 by Richard Osborne
Copyright (c) 2010
=============================================================
*/

#if !defined(PLAYERSKILL_BODY_H)
#define PLAYERSKILL_BODY_H

#include <CoreFramework/Core/PlayerSkill.h>


namespace GODZ
{

	//Wall Walking Talent
	class GODZ_API PlayerSkillWallWalk : public PlayerSkill
	{
	public:		
		PlayerSkillWallWalk();
		virtual void Use(WCharacter* skillUser);
	};

	class GODZ_API PlayerSkillJump : public PlayerSkill
	{
	public:		
		PlayerSkillJump();
	};
}



#endif PLAYERSKILL_BODY_H