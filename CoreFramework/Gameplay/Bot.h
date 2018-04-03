/* ===========================================================
	Bot

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__BOT_H__)
#define __BOT_H__

#include "WController.h"
#include "MBot.h"

namespace GODZ
{
	class MActor;

	class GODZ_API Bot : public WController
	{
		DeclareGeneric(Bot, WController)

	public:
		Bot();
		virtual ~Bot();
		
		//WController interface
		virtual void PossessCharacter(WCharacter * character);
		virtual void OnCharacterDestroyed();

		void SetTarget(WEntity* target);

		virtual void OnTick(float dt);

	protected:
		atomic_weak_ptr<MActor> mActorProxy;
		MBot m_bot;
		bool m_jobStarted;
	};
}

#endif //__BOT_H__