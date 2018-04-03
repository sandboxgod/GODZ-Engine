/* ===========================================================
	JobThread

	Created Jan 14, '08 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include "Godz.h"
#include "Job.h"
#include "GodzAtomics.h"
#include <windows.h>

namespace GODZ
{

	struct JobQueryResult
	{
		bool m_hasJob;
		size_t m_numJobs;
	};

	struct JobQuery
	{
		Future<JobQueryResult>* m_result;
		Job* m_job;
	};

	//Stores Information for a Job thread. This private data should only be passed
	//to hardware job threads.
	struct PrivateJobThreadInfo
	{
		//adds a job to the queue
		void			addJobToQueue(Job* job);
		void			removeJob(Job* job);

		//transfer waiting jobs
		void			pushWaitingJobs();

		void			getJobInfo(Job* job, Future<JobQueryResult>& result);
		void			fillQueries();

		void			removeJobs();
		void			clear();

		//set to true if this JobThread should always keep running all jobs.
		bool m_isPermanentThread;
		GodzAtomic m_done;					//thread finished all of it's tasks and exited

		std::vector<Job*> m_jobQueue;	//all accesses to this object must be locked
		std::vector<Job*> m_jobRemoveQueue;
		std::vector<JobQuery> m_queryList;
		std::vector<Job*> m_jobs;		//current jobs that should be processed
		
		HANDLE			m_jobThread;	//thread id
		HANDLE			m_jobSignal;	//signal to exit
		HANDLE			m_jobSignalDone;		

		GodzAtomic			m_jobQueueLock;	//lock for job queue
		GodzAtomic			m_queryLock;
		GodzAtomic			m_jobRemoveQueueLock;
	};

	class GODZ_API JobThread
	{
	public:
		JobThread(size_t thread_id);

		//Adds a job to this Thread. 
		void			addJob(Job* job);
		void			hasJob(Job* job, JobQueryResult& result);
		void			removeJob(Job* job);
		size_t			getNumJobs() const { return m_numJobs; }

		bool			IsDone();

		//Returns false if this thread cannot accept more then 1 job.
		bool			canSupportMultipleJobs() { return !m_isPermanentThread; }

		void			Start();

		//This function should be called externally
		void			ExitThread();

		bool isPermenant() const { return m_isPermanentThread; }

		long			GetID() const { return m_id; }

	protected:
		PrivateJobThreadInfo m_threadInfo;		//private data that we send directly to the JobThread
		bool m_isRunning;

		//set to true if this JobThread should always keep running 1 job.
		bool m_isPermanentThread;

		long m_id;
		size_t m_numJobs;
	};
}

