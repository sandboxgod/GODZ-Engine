
#pragma once

#include <CoreFramework/Core/CoreFramework.h>

namespace GODZ
{
	struct GODZ_API ProjectionMatrixInfo
	{
		float fov;				//field of view given in radians
		float aspectRatio;		//aspect ratio for this light
		float near_plane;		//NEARPLANE for this light
		float far_plane;		//FARPLANE for this light
	};
}
