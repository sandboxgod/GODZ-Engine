/* ==============================================================
	Drawable

	Created Jan 21, '07 by Richard 'vajuras' Osborne
	Copyright (c) 2010
	=============================================================
*/

//This class was implemented all wrong. it should be a base class for any type of object that
//can be added to a scene. perhaps should be rewritten as an abstract interface at some point

#include "Godz.h"
#include "CollisionSystems.h"
#include "RenderQueue.h"
#include "HitResults.h"

#if !defined(__SCENE_ELEMENT__H__)
#define __SCENE_ELEMENT__H__

namespace GODZ
{


	class GODZ_API SceneElementCallback
	{
	public:
		virtual void AddSceneElement(SceneElement& element) = 0;
	};
}

#endif