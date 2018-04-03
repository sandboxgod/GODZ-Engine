/* ===========================================================
	WPlayerController

	Represents a player. World space player controllers can
	assume control of a character in the level.

	Created Apr 10, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WORLD_PLAYER_CONTROLLER_H_)
#define _WORLD_PLAYER_CONTROLLER_H_

#include "WController.h"
#include <CoreFramework/Core/InputSystem.h>



namespace GODZ
{
	//forward declar
	class WCharacter;
	class WCamera;
	class HUD;


	class GODZ_API WPlayerController 
		: public WController
		, public InputListener
	{
		DeclareGeneric(WPlayerController, WController)

	public:
		WPlayerController();
		~WPlayerController();
		
		
		virtual void				OnTick(float dt);
		virtual void				ReceiveInputEvent(const InputBroadcast& inputEvent);
		virtual void				PossessCharacter(WCharacter * character);
		void						SetCamera(WCamera* cam);

	protected:
		atomic_weak_ptr<WCamera>	m_pCamera;
		InputBroadcast			m_lastBroadcast;
		float m_elapsedTime;
	};
}

#endif
