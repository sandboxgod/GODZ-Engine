/* ===========================================================
DamageTypes


Created Sep 7, '07 by Richard Osborne
Copyright (c) 2010
=============================================================
*/

#if !defined(DAMAGETYPES_H)
#define DAMAGETYPES_H

#include <CoreFramework/Core/Godz.h>


namespace GODZ
{

	enum DamageType
	{
		DamageType_None,
		DamageType_Fire,			//Heat
		DamageType_Water,			//Ice
		DamageType_Earth,			//tornado, etc
		DamageType_Air,				//lightning
		DamageType_Energy,			//radiation, energy, etc
		DamageType_Psionics			//Mental
	};
}



#endif DAMAGETYPES_H