
#pragma once

#include "Vector3.h"

namespace GODZ
{
	struct GODZ_API AxisAngle
	{
		Vector3 axis;
		float angle;

		void MakeZero()
		{
			axis.MakeZero();
			angle = 0;
		}

		//Returns a zero axis angle
		static AxisAngle Zero()
		{
			AxisAngle tmp;
			tmp.MakeZero();
			return tmp;
		}

		friend GDZArchive& operator<<(GDZArchive& ar, AxisAngle& v)
		{
			ar << v.axis << v.angle;
			return ar;
		}
	};
}
