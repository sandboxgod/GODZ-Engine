/* ===========================================================
	Exports commonly used classes that may be useful to 
	external apps.

	Created Nov 9, 2003 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	========================================================== 
*/

#if !defined(_GODZEXPORTS_H_)
#define _GODZEXPORTS_H_

// Includes base classes
#include "Godz.h"

// Includes classes dependant on container definitions
#include "GenericObject.h"
#include "SceneManager.h"			//provides a scene manager
#include "Mesh.h"					//graphical representation of an object
#include "LevelNode.h"				//scene root node
#include "ModelResource.h"
#include "ResourceManager.h"
#include "WindowManager.h"
#include "IDriver.h"				//render device
#include "FPSCounter.h"				//provides a performance timer
#include "WCamera.h"

// Include gameplay --------------------------------------------------------
#include "WEntity.h"

// Include shaders -----------------------------------------------------------

#endif