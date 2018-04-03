
#include "GodzAssert.h"
#include "Assert.h"

#if defined(_DEBUG)
GODZ_API void godzassert(bool condition)
{
	if (!condition)
	{
		assert(condition);
	}
}
#endif

