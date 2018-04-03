/* ===========================================================
JobManager

Created Jan 14, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "JobManager.h"
#include "SceneManager.h"
#include "JobThread.h"

namespace GODZ
{

//Determine thread count using CPUCOUNT example:
//http://softwarecommunity.intel.com/articles/eng/1957.htm?target=http%3A%2F%2Fwww.intel.com%2Fcd%2Fids%2Fdeveloper%2Fasmo-na%2Feng%2F276610.htm

JobManager::JobManager()
	: m_maxNumThreads(1)
	, m_maxID(1) //count starts at 1; 0 is an invalid threadid
{
	SYSTEM_INFO info;
	info.dwNumberOfProcessors = 0;
	GetSystemInfo (&info);

	//Compute how much we should distribute our load (max # of threads to occupy)
	m_maxNumThreads = info.dwNumberOfProcessors;
	Log("Logical processors %d \n", m_maxNumThreads);

	//subtract one for rendering thread :(
	if (m_maxNumThreads > 0) //sanity check (I'm assuming should always be 1)
		m_maxNumThreads--;
}

JobManager::~JobManager()
{
	size_t len = m_threads.size();
	for(size_t i=0;i<len;i++)
	{
		//wait for the thread to exit.....
		m_threads[i]->ExitThread();

		delete m_threads[i];
	}
}


void JobManager::addJob(Job* job)
{
	//Goal: Keep all the cores on the platform busy. This means if there is a processor that is
	//idle, send it a "Job" to occupy it's resources.

	//Goal: Store hash codes of current Jobs. That way if we have 'unique' Jobs that can only
	//run as a single instance, we do not create another Job and cause resource contention

	bool found = false;

	if (m_threads.size() < m_maxNumThreads
		|| job->isPermanent()	//if this is a unique job it requires it's own thread
		)
	{
		//do nothing here, create a new jobthread and add the task
	}
	else
	{
		//find the optimum job thread to accept this task
		std::vector<JobThread*>::iterator jobIter;
        for (jobIter = m_threads.begin(); jobIter != m_threads.end(); jobIter++)
		{
            JobThread* jobThread = *jobIter;
            if (!jobThread->canSupportMultipleJobs())	//ask can the jobThread support multiple threads
			{
                jobThread->addJob(job);
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		//create a new JobThread
        JobThread* jobThread = new JobThread(m_maxID);
        m_threads.push_back(jobThread);

        jobThread->addJob(job);

        jobThread->Start();

		m_maxID++;
	}
}


void JobManager::removeJob(Job* job)
{
    std::vector<JobThread*>::iterator jobIter;
    for (jobIter = m_threads.begin(); jobIter != m_threads.end(); jobIter++)
	{
        JobThread* jobThread = *jobIter;

        if (jobThread->GetID() == job->GetThreadId())
		{
            if (jobThread->getNumJobs() == 1)
			{
                jobThread->ExitThread();
                m_threads.erase(jobIter);
                delete jobThread;
			}
			else
			{
                jobThread->removeJob(job);
			}

			break;
		}
	}
}

//blocks the calling thread until the jobthread exits
void JobManager::cleanup()
{
	bool requiresClean = false;

    std::vector<JobThread*>::iterator jobIter;
    for (jobIter = m_threads.begin(); jobIter != m_threads.end(); jobIter++)
	{
        JobThread* jobThread = *jobIter;
        while (!jobThread->IsDone());

        jobThread->ExitThread();
        m_threads.erase(jobIter);
        delete jobThread;

		//invoke it again, to search for others...
		requiresClean = true;
		break;
	}

    if (requiresClean)
    {
        cleanup();
    }
}

} //namespace GODZ