
#pragma once

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/GDZArchive.h>

namespace GODZ
{
	//2D Vector
	struct GODZ_API Vector2
	{
		float x,y;

		Vector2()
		{}

		Vector2(float X, float Y)
			: x(X)
			, y(Y)
		{}

		friend GDZArchive& operator<<(GDZArchive& ar, Vector2& v)
		{
			ar << v.x << v.y;
			return ar;
		}
	};
}
