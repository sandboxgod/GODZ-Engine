/* ===========================================================
	SceneData.h


	Copyright (c) 2008, Richard Osborne
	========================================================== 
*/

#if !defined(__SceneData__H__)
#define __SceneData__H__

#include "Godz.h"
#include "GodzAtomics.h"
#include <vector>
#include "RenderDeviceEvent.h"
#include "ScreenDimension.h"
#include "GenericReferenceCounter.h"
#include "ObjectStateChangeEvent.h"
#include "RenderQueue.h"
#include "MaterialInfo.h"

namespace GODZ
{
	class WidgetInfo;
	class IDriver;
	class ObjectStateChangeEvent;

	/*
	*	Middleman between the Renderer and SceneManager. Global Thread-safe container (all public functions are thread-safe)
	*/
	class GODZ_API SceneData
	{
	public:

		//thread-safe function; adds a Render Device Event
		static void				AddRenderDeviceEvent(RenderDeviceEvent* event);
		static void				RetrieveRenderDeviceEvents(std::vector<RenderDeviceEvent*>& list);

		//thread-safe function; adds an ObjectStateChangeEvent
		static void				AddObjectStateChangeEvent(ObjectStateChangeEvent* event);
		static unsigned int		GetMaxNumWidgets() { return MAX_WIDGETS; }
		static bool				RetrieveObjectStateChangeEvents(ObjectStateChangeEventList& list, unsigned int index);

		static Future<IDriver*>&	GetDriver();
		static void				SetDriver(IDriver *pDriver);

		//should be called when engine shutdown
		static void				Shutdown();

		//thread-safe method to generate a runtime id
		static long				CreateRuntimeID();
		static MaterialID		GetNextMaterialID();
		static VisualID			GetNextVisualID();
		static RenderQueue&		GetFreeSlotQueue();

		//thread-safe
		static bool				IsWindowValid();
		static void				SetWindowStatus(bool isValid);

	private:
		static const unsigned int MAX_WIDGETS = 2; //2 buckets (game/render threads)
		static ObjectStateChangeEventList	mObjStateChanges[MAX_WIDGETS];		//buckets for HUD widgets
		static std::vector<RenderDeviceEvent*>	m_events;					//(only used by "Render") stores exclusive copy for render thread

		//alternating locks, bit bucket
		static GodzAtomic			m_widgetLock[MAX_WIDGETS];
		static GodzAtomic			m_renderDeviceLock;			//atomic primitive used for thread safety

		static GodzAtomic			m_isWindowValid;

		static float				m_renderFrameTime;

		//Device driver......
		//TODO: Perhaps make an abstract device registry
		static Future<IDriver*>		m_driver;

		static AtomicInt			m_runtimeID;

		static RenderQueue			mFreeSlotQueue;
	};

}

#endif //__SceneData__H__
