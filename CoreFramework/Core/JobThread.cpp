/* ===========================================================
JobThread

Created Jan 14, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "JobThread.h"
#include "Process.h"
#include "GodzAtomics.h"
#include "Timer.h"


namespace GODZ
{

unsigned __stdcall DoJobThread(void* pData)
{
	PrivateJobThreadInfo* threadInfo = (PrivateJobThreadInfo*)pData;

	//Notify all of the jobs they are about to start
	size_t numJobs = threadInfo->m_jobs.size();
	for(size_t i=0;i<numJobs;i++)
	{
		threadInfo->m_jobs[i]->Start();
	}

	Timer timer;
	float t=timer.PerformanceTimer(TIMER_GETABSOLUTETIME);

	//remember, making calls to anywhere in the engine from this point on is dangerous....
	while(WaitForSingleObject(threadInfo->m_jobSignal,0) != WAIT_OBJECT_0 )
	{
		float currentTime=timer.PerformanceTimer(TIMER_GETABSOLUTETIME);
		float dt=currentTime-t;

		//iterate through local Jobs
		size_t numJobs = threadInfo->m_jobs.size();
		for(size_t i=0;i<numJobs;i++)
		{
			//tell the Job to advance his timer so he can wake up again
			threadInfo->m_jobs[i]->AdvanceTime(dt);

			if (!threadInfo->m_jobs[i]->IsSleeping())
			{
				bool keepRunning = threadInfo->m_jobs[i]->Run(threadInfo->m_jobs[i]->GetTimeAndReset());
				if (!keepRunning)
				{
					threadInfo->removeJob(threadInfo->m_jobs[i]);
				}
			}
		}

		//check queue for waiting jobs
		threadInfo->pushWaitingJobs();

		threadInfo->removeJobs();

		threadInfo->fillQueries();

		if (numJobs == 0)
		{
			//go ahead and exit this thread...
			break;
		}

		t = currentTime;
	}

	threadInfo->clear();
	threadInfo->m_done.Set();

	SetEvent(threadInfo->m_jobSignalDone);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////

void PrivateJobThreadInfo::addJobToQueue(Job *job)
{
	while(!m_jobQueueLock.Set());
	m_jobQueue.push_back(job);
	m_jobQueueLock.UnSet();
}

void PrivateJobThreadInfo::pushWaitingJobs()
{
	while(!m_jobQueueLock.Set());
	for (size_t i=0;i<m_jobQueue.size(); i++)
	{
		m_jobQueue[i]->Start();
		m_jobs.push_back(m_jobQueue[i]);
	}

	m_jobQueue.clear(); //clear the waiting queue now that the jobs have been handed off

	m_jobQueueLock.UnSet();
}

void PrivateJobThreadInfo::getJobInfo(Job* job, Future<JobQueryResult>& result)
{
	SynchronizedBlock block(m_queryLock);

	JobQuery q;
	q.m_job = job;
	q.m_result = &result;
	m_queryList.push_back(q);
}

void PrivateJobThreadInfo::fillQueries()
{
	SynchronizedBlock block(m_queryLock);
	for (size_t i=0;i<m_queryList.size(); i++)
	{
		JobQueryResult result;
		result.m_numJobs = m_jobs.size();
		result.m_hasJob =  false;

		for (size_t j=0;j<m_jobs.size(); j++)
		{
			if (m_jobs[j] == m_queryList[j].m_job)
			{
				result.m_hasJob = true;
				break;
			}
		}

		m_queryList[i].m_result->setValue(result);
	}

	m_queryList.clear();
}

void PrivateJobThreadInfo::removeJob(Job* job)
{
	SynchronizedBlock block(m_jobRemoveQueueLock);
	m_jobRemoveQueue.push_back(job);
}

void PrivateJobThreadInfo::removeJobs()
{
	SynchronizedBlock block(m_jobRemoveQueueLock);

	for (size_t i=0;i<m_jobRemoveQueue.size(); i++)
	{
		std::vector<Job*>::iterator iter;
		for(iter = m_jobs.begin(); iter != m_jobs.end(); iter++)
		{
			if (*iter == m_jobRemoveQueue[i])
			{
				(*iter)->Stop();
				(*iter)->SetThreadId(0);
				(*iter)->SetDone();
				m_jobs.erase(iter);
				break;
			}
		}
	}

	m_jobRemoveQueue.clear();
}

void PrivateJobThreadInfo::clear()
{
	std::vector<Job*>::iterator iter;
	for(iter = m_jobs.begin(); iter != m_jobs.end(); iter++)
	{
		(*iter)->Stop();
	}

	m_jobs.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////

JobThread::JobThread(size_t id)
: m_isRunning(false)
, m_isPermanentThread(false)
, m_id(id)
, m_numJobs(0)
{
	m_threadInfo.m_jobSignal = CreateEvent(NULL,false,false,NULL);
	m_threadInfo.m_jobThread = 0;
	m_threadInfo.m_jobSignalDone = CreateEvent(NULL,false,false,NULL);


	//permanant Threads always run (like Audio subsystem) and they do not
	//share their thread with anyone else
	m_threadInfo.m_isPermanentThread = false;
}


void JobThread::Start()
{
	m_isRunning = true;

	unsigned int threadId = NULL;

	//create the Job thread.
	m_threadInfo.m_jobThread = (HANDLE)_beginthreadex(NULL, 0, DoJobThread, &m_threadInfo, 0, &threadId);

	if ( m_threadInfo.m_jobThread != 0 )
	{
		SetThreadName( threadId, "Job Thread" );
	}
}


void JobThread::ExitThread()
{
	//signal the job thread we are ready to exit
	BOOL bSet = SetEvent(m_threadInfo.m_jobSignal);
	godzassert(bSet == 1);

	WaitForSingleObject(m_threadInfo.m_jobSignalDone, INFINITE);


	CloseHandle(m_threadInfo.m_jobThread);
	m_threadInfo.m_jobThread=0;

	//close event 
	CloseHandle(m_threadInfo.m_jobSignal);

	m_isRunning = false;
}

void JobThread::addJob(Job* job)
{
	m_numJobs++;
	job->SetThreadId(m_id);

	if (!m_isRunning)
	{
		//push this job into the active list of workers
		m_threadInfo.m_jobs.push_back(job);

		//Can only setup a permenant thread when the hardware thread isnt active
		m_threadInfo.m_isPermanentThread = job->isPermanent();
	}
	else
	{
		//cannot assign a unique job here, a new jobThread to hold the job needs to be instanced by
		//the Job Manager
		godzassert(!job->isPermanent()); 
		m_threadInfo.addJobToQueue(job);
	}
}

bool JobThread::IsDone()
{
	return m_threadInfo.m_done.IsSet();
}

void JobThread::hasJob(Job* job, JobQueryResult& result)
{
	if (m_threadInfo.m_done.IsSet())
	{
		result.m_hasJob = false;
		result.m_numJobs = 0;
		return;
	}

	Future<JobQueryResult> f;
	m_threadInfo.getJobInfo(job,f);

	//block calling thread til future filled out
	while(!f.isReady());

	result = f.getValue();
}

void JobThread::removeJob(Job* job)
{
	m_numJobs--;
	m_threadInfo.removeJob(job);
}


} //namespace GODZ



