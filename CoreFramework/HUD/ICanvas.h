/* ===========================================================
	IHUD.h

	Copyright (c) 2008, Richard Osborne
	========================================================== 
*/



#if !defined (__ICANVAS__)
#define __ICANVAS__

#include <CoreFramework/Core/Godz.h>


namespace GODZ
{
	class Widget;

	//drawing surface
	class GODZ_API ICanvas
	{
	public:
		virtual void DrawText(const char* text, int x, int y, int width, int height)=0;
		virtual void DrawWidget(Widget*)=0;
	};

}

#endif //__ICANVAS__