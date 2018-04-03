/* ===========================================================
	Job

	Base Class for code executed by a JobThread. Jobs can run 
	asynchronously with each other. They can share hardware 
	threads, etc

	Created Jan 14, '08 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GODZ_JOB_H__)
#define __GODZ_JOB_H__

#include "Godz.h"
#include "GodzAtomics.h"

namespace GODZ
{
	class GODZ_API Job
	{
	public:
		Job();

		//****************************************************************************
		// Thread-safe function calls

		void SetThreadId(long threadId) { mThreadId = threadId; }
		long GetThreadId() { return mThreadId; }

		//****************************************************************************
		// Job Thread calls

		//Called before the thread runs this job for the 1st time
		virtual void Start() = 0;

		//Execute job specific code. Expect this code to be called only once unless the job returns true 
		//to keep running. Never put an infinite loop in this function; that will tie up the jobthread
		virtual bool Run(float dt) = 0;

		//Returns true if this Job Type is 'unique' meaning it should be assigned it's own thread
		virtual bool isPermanent() = 0; //Gets it's own thread

		//Called when this job needs to be stopped and perform cleanup
		virtual void Stop() = 0;

		//Called by the Job when this Job has been released and it's safe for it to be destroyed
		void SetDone() { mIsDone.Set(); }
		bool IsDone() { return mIsDone.IsSet(); }

		//time in seconds this Job should sleep
		void Sleep(float time);
		bool IsSleeping();

		void AdvanceTime(float dt);
		float GetTimeAndReset();
		//****************************************************************************



	private:
		float mTimer;
		float mSleepDuration;
		bool mSleeping;
		GodzAtomic mIsDone;

		//Thread safe access so that main thread can query this Job if it's still owned by a JobThread, etc
		AtomicInt mThreadId;
	};


}

#endif //__GODZ_JOB_H__