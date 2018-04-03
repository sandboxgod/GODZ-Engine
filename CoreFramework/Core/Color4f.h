/* ===========================================================
	Color4f

	"Plans fail for lack of counsel, but with many advisers
	they succeed." - Proverbs 15:22

	Created Nov 17, '05 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GODZ_COLOR4f_H__)
#define __GODZ_COLOR4f_H__

#include <CoreFramework/Math/Vector3.h>

namespace GODZ
{
	struct GODZ_API Color4f
	{
		float r,g,b;		//rgb color values
		float a;			//alpha

		Color4f()
		{}

		Color4f(float r, float g, float b, float a)
		{
			this->r=r;
			this->g=g;
			this->b=b;
			this->a=a;
		}

		Color4f(const Vector3& v)
			: r(v.x),g(v.y),b(v.z),a(1)
		{
		}

		friend GDZArchive& operator<<(GDZArchive& ar, Color4f& color) 
		{
			ar << color.r << color.g << color.b << color.a;
			return ar;
		}
	};

	static const Color4f ZeroColor(0,0,0,0);
}

#endif //__GODZ_COLOR4f_H__
