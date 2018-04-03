/* ===========================================================
	Performance Timer

	Performance counter value may unexpectedly leap forward
	http://support.microsoft.com/kb/q274323/

	Copyright (c) 2003-4, Richard Osborne
	========================================================== 
*/

#if !defined(_TIMER_H_)
#define _TIMER_H_

#include "Godz.h"


namespace GODZ
{
	//static const char* DEFAULT_TIMER="default";

	class Timer;

	enum TIMER_COMMAND { TIMER_RESET, TIMER_START, TIMER_STOP, TIMER_ADVANCE,
                     TIMER_GETABSOLUTETIME, TIMER_GETAPPTIME, TIMER_GETELAPSEDTIME };

	#define GETTIMESTAMP GetTickCount	

	class GODZ_API Timer
	{
	public:
		Timer();
		float PerformanceTimer( TIMER_COMMAND command );

	public:
		//static Timer* m_pTimer;

	private:
		bool     m_bTimerInitialized;
		bool     m_bUsingQPF;
		bool     m_bTimerStopped;
		Int64 m_llQPFTicksPerSec;

        Int64 m_llStopTime;
        Int64 m_llLastElapsedTime;
		Int64 m_llBaseTime;

		// Get the time using timeGetTime()
		double m_fLastElapsedTime;
		double m_fBaseTime;
		double m_fStopTime;
	};

	//FLOAT GODZ_API PerformanceTimer(const char* timerGroup, TIMER_COMMAND command );
}

#endif