/* ===========================================================
	Renderer.h


	Copyright (c) 2008, Richard Osborne
	========================================================== 
*/

#if !defined(__Renderer__H__)
#define __Renderer__H__

#include "Godz.h"
#include <CoreFramework/Math/WBox.h>
#include "RenderList.h"
#include "RenderData.h"
#include "DeviceCreation.h"
#include "SceneInfo.h"
#include "Timer.h"
#include "ScreenDimension.h"
#include "PerformanceTracker.h"


namespace GODZ
{

	class IDriver;
	class RenderOptions;

	/*
	*	Base class for the Renderer
	*/
	class GODZ_API Renderer
	{
	public:
		Renderer(DeviceCreation& deviceCreation);

		void				StartRenderThread();

		//notification to close all threads (calling thread will block until all threads closed)
		void				ExitThreads();

		//Notification we exited render thread
		void				OnExitRenderThread();

		void				SetViewBoxSize(float size);

		void				HandleObjectStateChanges(IDriver *driver);		//render thread - accesses the widgets array

		HANDLE				GetRenderEventObject() { return m_eventRenderObject; } 

		//notification to send render device events to render pipe (should only be called by render pipe)
		void				SendRenderDeviceEvents(IDriver* driver);

		void				Draw(IDriver* Driver);

		void				Init();

		//////////////////////////////////////////////////////////////
		// Single threaded call path below
		void				DoRenderPass();
		

	private:

		void ParseObjectStateChanges(IDriver* driver, ObjectStateChangeEventList& list);

		HANDLE				GetRenderDeviceInitializedEvent() { return m_driverInitialized; }
		
		void				DrawNodeTree(SceneInfo& sceneInfo,SceneNode *nodeTree);

		//parallel split shadowmapping - computes split planes that are parallel to our view plane
		void				CalculateSplitDistances(float _fCameraNear, float _fCameraFar, SceneInfo &sceneInfo);

		HANDLE							m_renderThread;				//render thread
		HANDLE							m_eventRenderObject;		//signals the render thread to halt
		HANDLE							m_eventRenderObjectDone;	//signal when the thread has closed
		HANDLE							m_driverInitialized;		//signals that "Render" has initialized the graphics device

		
		bool							m_bStartedRenderThread;
		DeviceCreation					m_deviceCreationParameters;	

		Timer							m_pTimer;

		//performance tracking
		PerformanceTracker				m_profileDeviceEvents;
		PerformanceTracker				m_profileHandleWidgets;
		PerformanceTracker				m_profileRenderer;
		PerformanceTracker				m_profileRenderViewports;
		PerformanceTracker				m_profileShadowmap;
		PerformanceTracker				m_profileRenderMain;
		PerformanceTracker				m_profileStats;
		PerformanceTracker				m_profileHDR;

		RenderList* m_renderList;		//data for this 'frame'
		RenderData m_renderData;		//cached data, renderer's view of the 'world'
	};
}

#endif //__Renderer__H__
