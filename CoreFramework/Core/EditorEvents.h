/* ===========================================================
	EditorEvents

	Maybe badly named. These events can be used to inform the
	renderer to switch modes

	Created Sept 2010
	========================================================== 
*/

#pragma once

#include "ObjectStateChangeEvent.h"


namespace GODZ
{
	class GODZ_API EditorRenderSwitchEvent : public ObjectStateChangeEvent
	{
	public:
		enum RenderSwitch
		{
			RenderSwitch_Deferred,
			RenderSwitch_Forward
		};

		EditorRenderSwitchEvent(RenderSwitch renderMode)
			: mMode(renderMode)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_RenderSwitch)
		{
		}

		virtual rstring getDebug() { return GetString("EditorRendererEvent RenderSwitch %d", mMode); }

		RenderSwitch mMode;
	};

}
