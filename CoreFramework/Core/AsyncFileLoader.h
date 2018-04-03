/* ===========================================================
	AsyncFileLoader

	Created Jan 1, '08 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__ASYNCFILELOADER_H__)
#define __ASYNCFILELOADER_H__

#include "Godz.h"
#include "Job.h"
#include "GodzAtomics.h"
#include "ResourceFactory.h"
#include "AsyncFileObserver.h"

namespace GODZ
{
	class SceneManager;
	class AsyncFileManager;
	class JobManager;

	struct PackageDependency
	{
		Future<GenericPackage*> m_future;
	};

	/**
	* AsyncFileJobs work on loading a package
	*/
	class GODZ_API AsyncFileJob	: public Job
	{
		friend class AsyncFileManager;

	public:

		//This job runs only once.....
		virtual bool isPermanent() { return false;}

		//thread-safe method to request has the job completed
		bool isReady() { return m_isReady.IsSet() && IsDone(); }

		//Call after the isReady() flag has been set.
		GenericPackage* getPackage();

		//actual job code......
		virtual void Start();
		virtual bool Run(float dt);
		virtual void Stop();

	protected:
		bool HasFilledDependency();

		std::vector<PackageDependency> m_depends;
		AsyncFileJob(const char* packageName, ResourceFactory& factory, Future<GenericPackage*> future);
		Future<GenericPackage*> getFuture() { return m_future; }

		Future<GenericPackage*> m_future;
		ResourceFactory* m_factory;			//used to instantiate resources for this job
		GodzAtomic		m_isReady;
		rstring			m_path;
		GenericPackage* m_package;			//do not access until atomic is set by the job thread

	private:
		//non-copying
		AsyncFileJob(const AsyncFileJob& other)
		{
		}

		//non-copying
		AsyncFileJob& operator=(const AsyncFileJob &V)
		{
			return *this;
		}
	};


	/*
	* Manages AsyncFileJobs from the main thread... Stores all package dependencies
	*/
	class GODZ_API AsyncFileManager
	{
	public:
		//Creates a job to stream the package....
		//future - this is actually provided by the caller. They can check this object for status
		bool streamPackage(const char* packageName, JobManager* jobManager, Future<GenericPackage*>& future, AsyncFileObserver* observer = NULL);

		//called every so often by the main thread to manage asynchronous jobs
		void doTick(JobManager* jobManager);

		void removeAsyncFileObserver(AsyncFileObserver* observer);

	protected:

		struct FileJobNotify
		{
			AsyncFileObserver* observer;
			Future<GenericPackage*> m_future;
		};

		std::vector<AsyncFileJob*> m_job;	//currently running jobs
		std::vector<FileJobNotify> m_notifies;
	};
}

#endif //__ASYNCFILELOADER_H__
