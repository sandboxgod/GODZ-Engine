/* =======================================================

===========================================================*/
#if !defined(_GODZASSERT_H_)
#define _GODZASSERT_H_

#include "CoreFramework.h"


#if defined(_DEBUG)
	GODZ_API void godzassert(bool condition);
#else
#define godzassert(condition) 0
#endif


#endif
