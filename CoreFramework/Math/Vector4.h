
#pragma once

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/GDZArchive.h>
#include <math.h>
#include "GodzMath.h"

namespace GODZ
{
	struct GODZ_API Vector4
	{
		float x,y,z,w;

		Vector4()
		{}

		Vector4(const Vector3& v)
			: x(v.x)
			, y(v.y)
			, z(v.z)
			, w(1)
		{}

		Vector4(float inX, float inY, float inZ, float inW)
			: x(inX)
			, y(inY)
			, z(inZ)
			, w(inW)
		{}

		static float Dot(const Vector4& a, const Vector4& b)
		{
			return (a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w);
		}

		bool operator==(const Vector4& v) const
		{
			if (!Equals(x,v.x))
				return false;
			if (!Equals(y,v.y))
				return false;
			if (!Equals(z,v.z))
				return false;
			if (!Equals(w,v.w))
				return false;

			return true;
		}

		friend GDZArchive& operator<<(GDZArchive& ar, Vector4& v)
		{
			ar << v.x << v.y << v.z << v.w;
			return ar;
		}
	};
}
