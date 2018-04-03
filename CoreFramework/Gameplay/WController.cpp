
#include "WController.h"
#include "WCharacter.h"
#include <CoreFramework/Reflection/GenericClass.h>


namespace GODZ
{

ImplementGenericFlags(WController, ClassFlag_Transient)

WController::WController()
{
}

// Can possibly return NULL if the character has been removed
WCharacter* WController::GetCharacter()
{
	return m_pActor.lock();
}

void WController::PossessCharacter(WCharacter * character)
{
	atomic_weak_ptr<WCharacter> character_ptr = atomic_ptr_cast<WCharacter>( character->GetWeakReference() );

	m_pActor = character_ptr;
	character->SetController(this);
}

void WController::OnCharacterDestroyed()
{
}

}