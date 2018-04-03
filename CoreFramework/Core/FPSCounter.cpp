
#include "FPSCounter.h"
#include "GenericObject.h"
#include "SceneManager.h"

using namespace GODZ;

FPSCounter::FPSCounter()
: GenericSingleton<FPSCounter>(*this)
, m_fFPS(0.0f)
, m_dwFrames(0.0f)
, m_fLastTime(0.0f)
, m_hasInit(false)
, mElapsed(0)
{
}

float FPSCounter::UpdateStats()
{	
    // Keep track of the frame count    
	float fTime = m_pTimer.PerformanceTimer(TIMER_GETABSOLUTETIME );

	if (!m_hasInit)
	{
		m_fLastTime = fTime;
		m_hasInit = true;
	}

	mElapsed += fTime - m_fLastTime;

    ++m_dwFrames;

    // Update the scene stats once per second
    if( mElapsed >= 1.0f )
    {
        m_fFPS    = m_dwFrames / mElapsed;
		mElapsed = 0;
        m_dwFrames  = 0;
    }

	m_fLastTime = fTime;
	return mElapsed;
}

float FPSCounter::GetFPS()
{
	return m_fFPS;
}

