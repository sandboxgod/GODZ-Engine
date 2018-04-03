/* ===========================================================
	ViewportEvents.h

	Copyright (c) 2010
	========================================================== 
*/

#include "ObjectStateChangeEvent.h"
#include "RenderQueue.h" //need viewportinfo

#if !defined(__VIEWPORTEVENTS_H__)
#define __VIEWPORTEVENTS_H__

namespace GODZ
{
	class GODZ_API ViewportCreationEvent : public ObjectStateChangeEvent
	{
	public:
		ViewportCreationEvent(const ViewportInfo& info)
			: ObjectStateChangeEvent(ObjectStateChangeEventID_ViewportCreation)
			, mViewportInfo(info)
		{
		}

		//optimal: creates a 'default' view
		ViewportCreationEvent()
			: ObjectStateChangeEvent(ObjectStateChangeEventID_ViewportCreation)
		{
		}

		ViewportInfo mViewportInfo;
	};

	class GODZ_API ViewportDestroyEvent : public ObjectStateChangeEvent
	{
	public:
		ViewportDestroyEvent(size_t id)
			: m_id(id)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_ViewportDestroy)
		{
		}

		size_t m_id;
	};

	class GODZ_API ViewportActivateEvent : public ObjectStateChangeEvent
	{
	public:
		ViewportActivateEvent(GODZHWND ptr)
			: m_id(ptr)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_ViewportActivate)
		{
		}
		
		//we only want to send one ViewportActivateEvent so always defer to the more recent event
		virtual bool isMoreRecent(ObjectStateChangeEvent* other) { return true; }

		GODZHWND m_id;
	};

	class GODZ_API ViewportSetBackgroundColorEvent : public ObjectStateChangeEvent
	{
	public:
		ViewportSetBackgroundColorEvent(GODZHWND ptr)
			: m_id(ptr)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_ViewportSetBackgroundColor)
			, r(255)
			, g(255)
			, b(255)
		{
		}
		
		//we only want to send one ViewportActivateEvent so always defer to the more recent event
		virtual bool isMoreRecent(ObjectStateChangeEvent* other) { return true; }

		GODZHWND m_id;
		UInt8 r, g, b; // RGB Color
	};
}

#endif //__VIEWPORTEVENTS_H__