/* ===========================================================
Job

Created Jan 14, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "Job.h"
    
namespace GODZ
{

Job::Job()
: mTimer(0)
, mSleepDuration(0)
, mSleeping(false)
, mThreadId(0)
{
}

void Job::AdvanceTime(float dt)
{
	mTimer += dt;

	if (mTimer > mSleepDuration)
	{
		mSleeping = false;
	}
}

bool Job::IsSleeping() 
{ 
	return mSleeping; 
}

void Job::Sleep(float time) 
{ 
	mSleepDuration = time;
	mSleeping = true;
	mTimer = 0.0f;
}

float Job::GetTimeAndReset()
{
	float t = mTimer;
	mTimer = 0.0f;
	return t;
}

}