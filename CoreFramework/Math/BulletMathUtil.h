
#pragma once

#include "Vector3.h"
#include "WMatrix3.h"
#include <LinearMath/btVector3.h>
#include <LinearMath/btMatrix3x3.h>

namespace GODZ
{
	class GODZ_API BulletMathUtil
	{
	public:
		static btVector3 ConvertTo(const Vector3& v)
		{
			return btVector3(v.x, v.y, v.z);
		}

		static btMatrix3x3 ConvertTo(const Matrix3& m)
		{
			return btMatrix3x3( m._11, m._12, m._13, m._21, m._22, m._23, m._31, m._32, m._33 );
		}

		static Vector3 ConvertTo(const btVector3& v)
		{
			return Vector3(v.x(), v.y(), v.z());
		}
	};
}
