/* ===========================================================
AsyncFileLoader

Created Jan 14, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "AsyncFileLoader.h"
#include "PackageReader.h"
#include "GenericObjData.h"
#include "JobManager.h"
#include "NameTable.h"
#include "ResourceManager.h"
#include "HStringTable.h"

namespace GODZ
{


bool AsyncFileManager::streamPackage(const char* packageName, JobManager* jobMan, Future<GenericPackage*>& future, AsyncFileObserver* observer)
{
	//make sure the filename is an absolute path!
	size_t index = StringBuffer::StaticFindSubstring(packageName, ".");

	rstring path=packageName;
	if (index == -1)
	{
		if (!GetAbsolutePathToPackage(packageName, path))
		{
			Log("Cannot determine path to package %s", packageName);
			godzassert(0);
			return false;
		}
	}

	//First, check to see if there are any outstanding requests for this item.
	//If so, then we point the caller's future at that....
	std::vector<AsyncFileJob*>::iterator jobIter;
	for(jobIter = m_job.begin(); jobIter != m_job.end(); jobIter++)
	{
		AsyncFileJob* j = *jobIter;
		if ( j != NULL)
		{
			if (_stricmp(j->m_path, path.c_str()) == 0)
			{
				future = j->m_future;
				return true;
			}
		}
	}

	if (observer != NULL)
	{
		//Add the observer here to our global list...
		FileJobNotify notify;
		notify.m_future = future;
		notify.observer = observer;
		m_notifies.push_back(notify);
	}


	ResourceManager* resMngr = ResourceManager::GetInstance();
	PackageReader reader(&resMngr->GetResourceFactory());
	PackageDependencies dependencies;
	reader.ReadPackageDependencies(path, dependencies);

	AsyncFileJob* job = new AsyncFileJob(path, resMngr->GetResourceFactory(), future);

	//make sure all of our package dependencies are already loaded...
	size_t numDependencies = dependencies.Size();
	for (size_t i=0;i<numDependencies;i++)
	{
		HashCodeID hash = dependencies.Get(i);
		GenericPackage* package = GenericObjData::m_packageList.FindPackageByName(hash);

		if (package == NULL)
		{
			//spawn a task to load this package dependency....
			const char* pname = HStringTable::GetInstance()->GetString(hash);
			godzassert(pname != NULL); // Try updating the Hash Table.

			if (pname != NULL)
			{
				PackageDependency dep;
				streamPackage(pname, jobMan, dep.m_future);

				//inform the asyncfilejob it depends on this package
				job->m_depends.push_back(dep);
			}
		}
	}

	m_job.push_back(job);
	jobMan->addJob(job);

	return true;
}

//Main thread...
void AsyncFileManager::doTick(JobManager* jobMan)
{
	bool jobCompleted = false;

	size_t emptyJobSlot = 0;
	std::vector<AsyncFileJob*>::iterator jobIter;
	for(jobIter = m_job.begin(); jobIter != m_job.end(); jobIter++)
	{
		if (*jobIter == NULL)
		{
			emptyJobSlot++;
			continue;
		}

		AsyncFileJob* job = *jobIter;
		if (job->isReady())
		{
			//set flag   
			jobCompleted = true;

			//Notify all of our observers....
			bool hasRemoved = false;
			do
			{
				hasRemoved = false;
				std::vector<FileJobNotify>::iterator iter;
				for(iter=m_notifies.begin();iter != m_notifies.end(); iter++)
				{
					if (iter->m_future == job->getFuture())
					{
						iter->observer->OnJobCompleted();
						m_notifies.erase(iter);	//remove this listener
						hasRemoved = true;
						break;
					}
				}
			}while (hasRemoved);

			//remove this job from the queue and free it....
			delete job;
			job=NULL;
			*jobIter = NULL;
			emptyJobSlot++;
		}
	}

	size_t numJobs = m_job.size();
	if (numJobs > 0 && emptyJobSlot == numJobs)
	{
		//If all jobs have been processed let's cleanup our list....
		m_job.clear();
	}
}

void AsyncFileManager::removeAsyncFileObserver(AsyncFileObserver* observer)
{
	std::vector<FileJobNotify>::iterator iter;
	for(iter=m_notifies.begin();iter!=m_notifies.end();iter++)
	{
		if (iter->observer == observer)
		{
			m_notifies.erase(iter);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////

AsyncFileJob::AsyncFileJob(const char* packageName, ResourceFactory& factory, Future<GenericPackage*> future)
: m_package(NULL)
, m_path(packageName)
, m_future(future)
{
	m_factory = &factory;
}

GenericPackage* AsyncFileJob::getPackage()
{
	godzassert(m_isReady.IsSet());
	return m_package;
}

bool AsyncFileJob::HasFilledDependency()
{
	std::vector<PackageDependency>::iterator iter;
	for (iter = m_depends.begin(); iter != m_depends.end(); iter++)
	{
		if (iter->m_future.isReady())
		{
			m_depends.erase(iter);
			return true;
		}
	}

	return false;
}

//called by the Job thread
bool AsyncFileJob::Run(float dt)
{
	//Check to see if we have any dependencies....
	while(HasFilledDependency());

	//if we still have required dependencies then wait
	if (m_depends.size() > 0)
	{
		return true;
	}

	PackageReader reader(m_factory); //Pass in private resource factory
	reader.ReadPackage(m_path);
	
	m_package = reader.GetPackage();
	m_package->OnLoaded();

	//We register the dependency here, this way other AsyncFileJobs can process
	//this package
	GenericObjData::m_packageList.AddPackage(m_package);

	m_future.setValue(m_package);


	//must be the last step..... Let's main thread know we're ready
	m_isReady.Set();

	return false;
}

//called by the Job thread
void AsyncFileJob::Stop()
{
}

//called by the Job thread
void AsyncFileJob::Start()
{
}


}