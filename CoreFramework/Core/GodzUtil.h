/* ===========================================================
	GodzUtil

	Created Nov 4, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GODZ__UTIL__)
#define __GODZ__UTIL__

#include "Godz.h"

namespace GODZ
{
	//various engine folders...
	enum EResourceType
	{
		RT_Character,
		RT_Model,
		RT_Texture,
		RT_Template,
		RT_LevelFile
	};

	//returns a full path to a file. For ex., if a character is going to be loaded,
	//named 'imp'. Just pass the resource type RT_Character. A full path to where
	//the file should be saved or 'loaded from' is returned.
	// [in] filename - name of the file (such as imp).
	GODZ_API rstring GetFullPath(EResourceType kType, const char* resourceName);
}

#endif //__GODZ__UTIL__