/* ===========================================================
	High Dynamic Range Manager

	Created May 8, '07 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__HDRMANAGER__)
#define __HDRMANAGER__

#include "Godz.h"

namespace GODZ
{
	//high dynamic range shader / Bloom
	class GODZ_API HDRManager
	{

	public:
		virtual void					Begin() {}
		virtual void					End() {}
		virtual void					Render() {}
		
	};
}

#endif