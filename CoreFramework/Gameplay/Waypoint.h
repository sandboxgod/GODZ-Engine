/* ===========================================================
	Waypoint

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#pragma once

#include <CoreFramework/Core/WEntity.h>

namespace GODZ
{
	/*
	* Simply an object placed in the map that can be used for reference
	* to spawn the player, etc
	*/
	class GODZ_API Waypoint : public WEntity
	{
		DeclareGeneric(Waypoint, WEntity)
	};
}
