/*==========================================================
	PerformanceTracker.h


	Copyright (c) 2008, Richard Osborne
	========================================================== 
*/

#if !defined(__PerformanceTracker__H__)
#define __PerformanceTracker__H__

#include "Godz.h"
#include "Timer.h"

namespace GODZ
{

	/*
	*	Performance Tracker
	*/
	class GODZ_API PerformanceTracker
	{
	public:

		//profiling ---->
		void				profilerBeginTime();
		void				profilerEndTime(const char* txt);


	private:
		Timer							m_pTimer;

		//performance tracking
		float							m_profilerLastTime;

	};

}

#endif //__Renderer__H__
