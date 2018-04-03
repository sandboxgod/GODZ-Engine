/* ===========================================================
	PropertyNames

	Soon this will be replaced by a localized string table

	"Plans fail for lack of counsel, but with many advisers
	they succeed." - Proverbs 15:22

	Created Nov 25, '06 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__PROPERTY_NAMES_H__)
#define __PROPERTY_NAMES_H__

namespace GODZ
{
	static const char* PHYS_NONE="PHYS_None";				//physics constant
	static const char* PHYS_FALLING="PHYS_Falling";			//physics constant
	static const char* PHYS_WALKING="PHYS_Walking";			//physics constant
	static const char* PHYS_PROJECTILE="PHYS_Projectile";	//physics constant
	static const char* PHYS_FLYING="PHYS_Flying";			//physics constant
	static const char* PHYS_JUMPING="PHYS_Jumping";			//physics constant
	static const char* MESH="Mesh";							//property name
	static const char* PHYSICS="Physics";					//property name
	static const char* NORMAL_MAP="NormalMap";
	static const char* BLEND_MAP="BlendMap";

	static const char* PROJECTION_PERSPECTIVE="PT_Perspective";			//projection constant
	static const char* PROJECTION_ORTHOGONAL="PT_Orthogonal";			//projection constant
	static const char* PROJECTION_TYPE="Projection";					//property name
}

#endif //__PROPERTY_NAMES_H__
