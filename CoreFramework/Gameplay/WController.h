/* ===========================================================
	WController

	Controllers are responsible for controlling the character.

	Created Apr 10, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WORLD__CONTROLLER_H_)
#define _WORLD__CONTROLLER_H_

#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/atomic_ref.h>

namespace GODZ
{
	//forward declar
	class WCharacter;

	class GODZ_API WController : public GenericObject
	{
		DeclareGeneric(WController, GenericObject)

	public:
		WController();

		//Returns character currently controlled by this player
		WCharacter* GetCharacter();

		//Possesses the character
		virtual void PossessCharacter(WCharacter * character);

		virtual void OnCharacterDestroyed();

		virtual void OnTick(float dt) {}

	protected:
		atomic_weak_ptr<WCharacter> m_pActor;	//character that we currently control

	};
}

#endif
