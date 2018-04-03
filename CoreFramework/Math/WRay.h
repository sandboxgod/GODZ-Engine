
#pragma once

#include "Vector3.h"

namespace GODZ
{
	//Represents a world trace
	struct GODZ_API WRay
	{
		Vector3 origin, dir;

		WRay() {}
		WRay(const Vector3& org, const Vector3& dir)
		{
			this->origin=org;
			this->dir=dir;
		}
	};
}
