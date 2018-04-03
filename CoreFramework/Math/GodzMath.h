

#pragma once

#include <CoreFramework/Core/CoreFramework.h>
#include <math.h>

namespace GODZ
{
	#ifndef NOMINMAX
	#ifndef MAX
	#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
	#endif

	#ifndef MIN
	#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
	#endif
	#endif  /* NOMINMAX */ 

	static const float EPSILON = 0.0001f;


	static bool Equals(float v1, float v2)
	{
		float diff = fabsf(v1 - v2);
		return diff < EPSILON;
	}

	static float sign(float v)
	{
		if (v < 0.0f)
			return -1.0f;

		return 1.0f;
	}
}
