
#include "SceneData.h"
#include <CoreFramework/HUD/WidgetInfo.h>
#include "Timer.h"
#include "PerformanceTracker.h"


namespace GODZ
{

ObjectStateChangeEventList		SceneData::mObjStateChanges[MAX_WIDGETS];		//buckets for objects
std::vector<RenderDeviceEvent*>	SceneData::m_events;					//(only used by "Render") stores exclusive copy for render thread

//alternating locks, bit bucket
GodzAtomic					SceneData::m_widgetLock[MAX_WIDGETS];
GodzAtomic					SceneData::m_renderDeviceLock;			//atomic primitive used for thread safety

Future<IDriver*>			SceneData::m_driver;

//Runtime ID is atomic locked so that any thread may get an ID for an object. If we determine
//this is never the case, we can put debug checks to ensure only main thread needs this and turn
//this into an Unsigned Int
AtomicInt					SceneData::m_runtimeID = 1;			//begin at 1, so 0 can represent an invalid value
GodzAtomic					SceneData::m_isWindowValid;

RenderQueue			SceneData::mFreeSlotQueue;

long SceneData::CreateRuntimeID()
{
	return ++m_runtimeID;
}

MaterialID SceneData::GetNextMaterialID()
{
	return mFreeSlotQueue.GetNextMaterialIndex();
}

VisualID SceneData::GetNextVisualID()
{
	return mFreeSlotQueue.GetNextVisualIndex();
}

RenderQueue& SceneData::GetFreeSlotQueue()
{
	return mFreeSlotQueue;
}

void SceneData::AddRenderDeviceEvent(RenderDeviceEvent* event)
{
	while(!m_renderDeviceLock.Set()); //lock the resource list for thread safety		
	m_events.push_back(event);
	m_renderDeviceLock.UnSet();
}

void SceneData::RetrieveRenderDeviceEvents(std::vector<RenderDeviceEvent*>& list)
{
	while(!m_renderDeviceLock.Set()); //lock the resource list for thread safety
	list = m_events;
	m_events.clear();
	m_renderDeviceLock.UnSet();
}

void SceneData::AddObjectStateChangeEvent(ObjectStateChangeEvent* event)
{
	while (!m_widgetLock[0].Set());

	//acquired the lock, now set the data.....
	mObjStateChanges[0].Add(event);
	m_widgetLock[0].UnSet();
}

bool SceneData::RetrieveObjectStateChangeEvents(ObjectStateChangeEventList& list, unsigned int index)
{
	bool isSet = false;

	while(!m_widgetLock[0].Set());

	isSet = true;
	list = mObjStateChanges[0];
	mObjStateChanges[0].Clear();
	m_widgetLock[0].UnSet();

	return isSet;
}

Future<IDriver*>& SceneData::GetDriver()
{
	return m_driver;
}

void SceneData::SetDriver(IDriver *pDriver)
{
	m_driver.setValue(pDriver);
}


//This should be called after all of the Packages, etc have been deleted. Basically, this should run last. This way we can be sure
//no new events are added while shutting down
void SceneData::Shutdown()
{
	//remove outstanding events
	size_t num = mObjStateChanges[0].GetNumObjects();
	for(size_t i = 0; i < num; i++)
	{
		delete mObjStateChanges[0][i];
	}
	mObjStateChanges[0].Clear();
}

bool SceneData::IsWindowValid()
{
	return m_isWindowValid.IsSet();
}

void SceneData::SetWindowStatus(bool isValid)
{
	if (isValid)
		m_isWindowValid.Set();
	else
	{
		m_isWindowValid.UnSet();
	}
}

} //namespace 

