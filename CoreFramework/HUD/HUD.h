/* ===========================================================
	HUD

	Basic Heads Up Display Implementation

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__HUD_H__)
#define __HUD_H__

#include "Widget.h"

namespace GODZ
{
	//Note: would like to give scripters access to the HUD for some client side mods

	/*
	*	Heads Up Display object which is owned by the player.
	*/
	class GODZ_API HUD
	{
	public:
		HUD();
		~HUD();

		void						Clear();

		//creates a new widget and returns it
		Widget&						CreateWidget();
		void						RemoveWidget(WidgetId id);
		void						Update(float dt);

	protected:
		std::vector<Widget>			m_widgets;

		//nOTE:	All HUDs need to share the widget index so they can generate unique objects
	};
}

#endif
