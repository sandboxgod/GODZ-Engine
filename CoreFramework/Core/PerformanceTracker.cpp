/* ===========================================================
PerformanceTracker

Created Jan 14, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "PerformanceTracker.h"
    
#define TRACK_PROFILES

namespace GODZ
{

void PerformanceTracker::profilerBeginTime()
{
#if defined(TRACK_PROFILES)
	m_pTimer.PerformanceTimer(TIMER_START);
#endif
}

void PerformanceTracker::profilerEndTime(const char* txt)
{
#if defined(TRACK_PROFILES)
	float t = m_pTimer.PerformanceTimer(TIMER_GETELAPSEDTIME);
	Log("Profile Ended %s time %f \n", txt, t);
#endif
}

}