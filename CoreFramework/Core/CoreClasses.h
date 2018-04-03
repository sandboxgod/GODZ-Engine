/* ===========================================================
	Core Classes registry

	Created Sept 27, '05 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include "Godz.h"

namespace GODZ
{
	// Loads all the core classes used by the GODZ Engine. The engine
	// needs to have all the internal classes listed so that objects can be loaded by name via
	// commands like StaticLoadObject() which is an IMPORTANT part of the 
	// GODZ Package format and file loading*
	void LoadCoreClasses();
}

