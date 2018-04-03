
#pragma once

#include "Vector3.h"
#include "GodzMath.h"

namespace GODZ
{
	//Describes an Euler rotation in terms of yaw, pitch, roll
	struct GODZ_API EulerAngle
	{
		float y,p,r;

		EulerAngle()
			: y(0)
			, p(0)
			, r(0)
		{
		}

		EulerAngle(float yaw, float pitch, float roll)
			: y(yaw)
			, p(pitch)
			, r(roll)
		{
		}

		operator float*()
		{
			return &y;
		}

		operator const float*() const
		{
			return &y;
		}

		bool operator==(const EulerAngle& v) const
		{
			if (!Equals(y,v.y))
				return false;

			if (!Equals(p,v.p))
				return false;

			if (!Equals(r,v.r))
				return false;

			return true;
		}

		friend GDZArchive& operator<<(GDZArchive& ar, EulerAngle& v)
		{
			ar << v.y << v.p << v.r;
			return ar;
		}
	};

	static const EulerAngle ZERO_ROTATION=EulerAngle(0.0f,0.0f,0.0f);
}
