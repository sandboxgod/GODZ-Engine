/* ===========================================================
	IGUIController

	Manages GUI Menus

	Created Dec 12, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "Godz.h"

#if !defined(__GUICONTROLLER_H__)
#define __GUICONTROLLER_H__

namespace GODZ
{
	class GUIMenu;

	class GODZ_API GUIController
	{
	public:
		GUIController();

		//Adds the menu. Either the full qualified name <Package.Class> or <Class>
		//name can be specified.
		GUIMenu* AddMenu(const char* menuName);

		//Searches for the menu. Either the full qualified name <Package.Class> or <Class>
		//name can be specified.
		GUIMenu* FindMenu(HString menuName);

		bool RemoveMenu(GUIMenu* pMenu);

		//Displays the menu. While a menu is active, all HUDs are turned off and input
		//is swallowed by the controller.
		void DisplayMenu(GUIMenu* pMenu);

	protected:
		std::vector<GUIMenu*> m_menus;
		GUIMenu* m_pCurrMenu;
	};
}

#endif __GUICONTROLLER_H__
