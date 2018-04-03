/* ===========================================================
	GUIControl

	Created Dec 12, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/HUD/WidgetInfo.h>

#if !defined(__GUICONTROL_H__)
#define __GUICONTROL_H__

namespace GODZ
{
	//GUIControls submits a Widget to the renderer
	class GODZ_API GUIControl
	{
	protected:
		WidgetInfo m_info;

		//Selection Width
		int m_width;

		//Selection Height
		int m_height;
	};
}

#endif //__GUICONTROL_H__
