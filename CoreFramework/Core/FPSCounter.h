/* ===========================================================
	FPS Counter

	Created Jan 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_FPSCOUNTER_H_)
#define _FPSCOUNTER_H_

#include "Timer.h"
#include "GenericSingleton.h"

namespace GODZ
{
	//static const char* FPSCOUNTER = "FPSCounter";
	class GODZ_API FPSCounter : public GenericSingleton<FPSCounter>
	{		
	public:
		FPSCounter();

		float UpdateStats();
		float GetFPS();

	private:
		float m_fFPS;
		Timer m_pTimer;
		float m_dwFrames;
		float m_fLastTime;
		bool m_hasInit;
		float mElapsed;
	};
}

#endif