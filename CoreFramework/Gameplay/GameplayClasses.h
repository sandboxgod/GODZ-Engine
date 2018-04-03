#pragma once

#include "Bot.h"
#include "SpawnPoint.h"
#include "Waypoint.h"
#include "Projectile.h"
#include "WController.h"
#include "WCharacter.h"
#include "WPlayerController.h"
#include "CharacterTemplate.h"
#include "ProjectileTrigger.h"

namespace GODZ
{

// Registry for Gameplay Classes
void LoadGameplayClasses()
{
	Bot::GetClass();
	SpawnPoint::GetClass();
	Waypoint::GetClass();
	ProjectileTemplate::GetClass();
	WCharacter::GetClass();
	CharacterTemplate::GetClass();
	WController::GetClass();
	WPlayerController::GetClass();
	ProjectileTrigger::GetClass();

	//TODO: All new classes need to be listed here to enable 'object loading via strings'
}

}
