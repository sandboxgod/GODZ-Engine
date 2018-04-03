/* ===========================================================
	WDynamicLight.h

	Provides dynamic lighting capabilities. Derives from a
	camera because we want to have the ability to view the scene
	from the point-of-view of the light

	Created Nov 21, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WSUNLIGHT_H_)
#define _WSUNLIGHT_H_

#include "WDynamicLight.h"

namespace GODZ
{
	// Dynamic Light
	class GODZ_API WSunLight : public WDynamicLight
	{
		DeclareGeneric(WSunLight, WDynamicLight)

	public:
		WSunLight();

		//camera
		virtual Vector3				GetTarget();
		virtual Vector3				GetEye();

		//Notification this entity has been spawned on the level
		virtual void			OnAddToWorld();
	};
};

#endif //WSunLight