/* ===========================================================
AsyncFileObserver

Created Mar 9, '08 by Richard Osborne
Copyright (c) 2010
========================================================== 
*/

#include "AsyncFileObserver.h"
#include "SceneManager.h"



namespace GODZ
{

AsyncFileObserver::~AsyncFileObserver()
{
	if (EngineStatus::engineRunning)
	{
		//Leave nothing to chance....
		SceneManager *smgr = SceneManager::GetInstance();
		smgr->RemoveAsyncFileObserver(this);
	}
}

}