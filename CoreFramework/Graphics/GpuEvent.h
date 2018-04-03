#ifndef _GPUEVENT_H_
#define _GPUEVENT_H_

#include <CoreFramework/Core/IDriver.h>

namespace GODZ
{
	class GpuEvent
	{
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
	};

	class GpuEventHelper
	{
	public:
		GpuEventHelper(IDriver* pDriver, const char * pLabel, u32 eventColor)
		: m_pDriver(pDriver)
		{
			m_pEvent = m_pDriver->CreateGpuEvent(pLabel, eventColor);
			m_pEvent->Begin();
		}

		~GpuEventHelper()
		{
			m_pEvent->End();
			m_pDriver->DestroyGpuEvent(m_pEvent);
		}

	protected:
		GpuEvent * m_pEvent;
		IDriver * m_pDriver;
		
	};

#if !defined(_FINAL)
	#define GPUEVENT(pDriver, eventVar, eventLabel, eventColor) \
		GpuEventHelper gpuevent##eventVar##(pDriver, eventLabel, eventColor);
#else
#define GPUEVENT(pDriver, eventVar, eventLabel, eventColor)
#endif

	class GpuEventHelperStatic
	{
	public:
		GpuEventHelperStatic(GpuEvent &event)
		: m_Event(event)
		{
			m_Event.Begin();
		}

		~GpuEventHelperStatic()
		{
			m_Event.End();
		}

	protected:
		GpuEvent & m_Event;
	};

#if !defined(_FINAL)
	#define GPUEVENT_STATIC(pDriver, eventVar, eventLabel, eventColor) \
		static GpuEvent * s_gpuevent##eventVar## = pDriver->CreateGpuEvent(eventLabel, eventColor); \
		GpuEventHelperStatic gpuevent##eventVar##(*s_gpuevent##eventVar##);
#else
	#define GPUEVENT_STATIC(pDriver, eventVar, eventLabel, eventColor)
#endif
}

#endif // #ifndef _GPUEVENT_H_
