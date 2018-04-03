/* ===========================================================
	GUIMenu

	Created Dec 12, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include <CoreFramework/Core/Godz.h>

#if !defined(__GUIMENU_H__)
#define __GUIMENU_H__

namespace GODZ
{
	class GUIControl;

	class GODZ_API GUIMenu
	{
	public:
		//Appends a component to this menu
		void AddControl(GUIControl* control);

		virtual void OnClose();

		//Notification this menu has been displayed
		virtual void OnDisplay();

	protected:
		std::vector<GUIControl*> m_controls;
	};
}

#endif //__GUIMENU_H__
