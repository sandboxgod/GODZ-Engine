/* ===========================================================
	Blur Manager


	========================================================== 
*/

#if !defined(__BLURMANAGER__)
#define __BLURMANAGER__

#include "Godz.h"

namespace GODZ
{
	class GODZ_API BlurManager
	{
	public:
		virtual void				Begin() = 0;
		virtual void				End() = 0;
		
	};
}

#endif