/* ===========================================================
JobManager

Specializes in distributing tasks to hardware resources
as needed.

Created Jan 14, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#if !defined(__JOBMANAGER_H__)
#define __JOBMANAGER_H__

#include "Godz.h"


namespace GODZ
{
	class Job;
	class JobThread;

	class GODZ_API JobManager
	{
	public:
		//Determine how many threads should be created for this platform
		JobManager();

		//close job threads
		~JobManager();

		//Adds a Job to a job thread
		void			addJob(Job* job);
		void			removeJob(Job* job);

		//removes idle threads
		void			cleanup();

	protected:
		std::vector<JobThread*> m_threads;
		unsigned int m_maxNumThreads;
		long m_maxID;
	};
}


#endif