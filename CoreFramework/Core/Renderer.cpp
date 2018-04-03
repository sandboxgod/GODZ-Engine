
#include "EntityEvents.h"
#include "EditorEvents.h"
#include "FPSCounter.h"
#include "IDriver.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "RenderOptions.h"
#include "SceneData.h"
#include "Material.h"
#include "ViewportEvents.h"
#include "ViewportInterface.h"

#include <CoreFramework/Graphics/RendererHelper.h>
#include <Process.h>

//#define TRACK_PROFILER_RENDERER
//#define LIST_EVENTS

namespace GODZ
{

unsigned __stdcall DoRenderThread(LPVOID pData)
{

	DeviceCreation* deviceCreation = (DeviceCreation*)pData;

	Renderer* renderer = deviceCreation->m_renderer;

	RenderOptions* opt = RenderOptions::GetInstance();
	renderer->SetViewBoxSize(opt->m_viewerBoxSize);

	//create a local ResourceManager that is created by the render device...
	ResourceManager* renderDeviceResourceMngr = ResourceManager::GetInstance();

	HANDLE eventObject = renderer->GetRenderEventObject();

	Future<IDriver*>& driverFuture = SceneData::GetDriver();
	while(!driverFuture.isReady()) //wait til ready
	{
		//check for signal; we perform this logic just incase the dx9.dll never loads and the app is closed
		//by the user while we're spinning...
		if (WaitForSingleObject(eventObject,0) == WAIT_OBJECT_0)
		{
			//the render thread was closed before the driver was even set...
			renderer->OnExitRenderThread();
			return 0;
		}
	}

	IDriver* pDriver = driverFuture.getValue();

	deviceCreation->m_pRenderDeviceResMngr = renderDeviceResourceMngr;
	pDriver->CreateDevice(*deviceCreation);

	godzassert(pDriver != NULL);

	while(WaitForSingleObject(eventObject,0) != WAIT_OBJECT_0 )
	{
		static Timer pTimer;
		float currentTime=pTimer.PerformanceTimer(TIMER_GETABSOLUTETIME);
		static float sLastTick=currentTime; //init to current time so we know no time elapsed
		float fElapsedTime=currentTime-sLastTick;

		//update HUD widgets --- grab current HUD info
		renderer->HandleObjectStateChanges(pDriver);

		//send over all the buffered events...
		renderer->SendRenderDeviceEvents(pDriver);

		//render the scene data
		renderer->Draw(pDriver);

		sLastTick=currentTime;

		//Log("render fps - %f \n", fElapsedTime);
	}
	
	//free dx driver
	delete pDriver;

	SceneData::SetDriver(NULL);

	//notification we exited render thread
	renderer->OnExitRenderThread();

	return 0;
}


Renderer::Renderer(DeviceCreation& deviceCreation)
: m_bStartedRenderThread(false)
, m_eventRenderObject(NULL)
, m_renderList(NULL)
{
	//used for signals....
	m_eventRenderObject = CreateEvent(NULL,false,false,NULL);
	m_eventRenderObjectDone = CreateEvent(NULL,false,false,NULL);

	m_deviceCreationParameters = deviceCreation;

	if (m_deviceCreationParameters.width == 0 || m_deviceCreationParameters.height == 0)
	{
		m_deviceCreationParameters.width= GetSystemMetrics(SM_CXSCREEN);
		m_deviceCreationParameters.height = GetSystemMetrics(SM_CYSCREEN);
	}
	
	m_deviceCreationParameters.m_renderer = this;
}

void Renderer::DoRenderPass()
{
	Future<IDriver*>& driverFuture = SceneData::GetDriver();
	if(driverFuture.isReady()) //wait til ready
	{
		IDriver* pDriver = driverFuture.getValue();

		//update HUD widgets --- grab current HUD info
		HandleObjectStateChanges(pDriver);

		//send over all the buffered events...
		SendRenderDeviceEvents(pDriver);

		//render the scene data
		Draw(pDriver);
	}
}

void Renderer::SetViewBoxSize(float size)
{
	m_renderData.SetViewBoxSize(size);
}

void Renderer::DrawNodeTree(SceneInfo& sceneInfo,SceneNode *nodeTree)
{

}


void Renderer::Draw(IDriver* Driver)
{
#ifdef TRACK_PROFILER_RENDERER
	m_profileRenderer.profilerBeginTime();
#endif

	SceneInfo sceneInfo;
	sceneInfo.m_renderData = &m_renderData;
	sceneInfo.m_pDriver = Driver;

	//initialize the actor node info
	sceneInfo.m_geomInfo = NULL;

	sceneInfo.m_bUnlit_Shadow_Pass = false;

	//active viewport display
	{
		ViewportInfo* viewInfo = m_renderData.GetActiveViewport();

		if (viewInfo == NULL)
		{
			return;
		}

		//Get matching HWND
		ViewportInterface* pViewportInterface = Driver->GetViewportByWindow(viewInfo->GetWindow());

		godzassert(pViewportInterface != NULL);
				
		//actual rendering pass for this viewport

		//This is the main rendering loop...

#ifdef TRACK_PROFILER_RENDERER
		Log("\n"); //space between profiles
#endif




		//build a scene info structure that will allow child nodes to 
		//render to the scene.
		sceneInfo.m_pViewport = pViewportInterface;

		godzassert(Driver!=NULL); //, "There is no DRIVER set for this device. See AddDevice(HWND)");

		CameraInfo* pCamera = m_renderData.GetMainCameraInfo();
		if (pCamera != NULL)
		{
			sceneInfo.m_camera = pCamera;
			RendererHelper::Render(*viewInfo, sceneInfo, Driver, m_deviceCreationParameters.m_isEditor);
		}
		else
		{
			Driver->Clear();
			Driver->BeginScene();	
			//this can happen during loading screen
			Vector3 up(0,1,0);
			Vector3 eye(63.0f, -150.0f, -180.0f);
			Vector3 look(63.0f, -150.0f, -180.0f);
			Driver->SetViewMatrix(eye, look, up);
//			RenderStats(sceneInfo, Driver, pViewportInterface);

			Driver->EndScene(sceneInfo); //present the buffer
		}
	} //loop

	//Update Statistics
	FPSCounter *fpsCounter = FPSCounter::GetInstance();	
	fpsCounter->UpdateStats();

#ifdef TRACK_PROFILER_RENDERER
	m_profileRenderer.profilerEndTime("Renderer::Draw");
#endif
}

void Renderer::OnExitRenderThread()
{
	//notification the render thread has exited
	CloseHandle(m_renderThread);
	m_renderThread=0;

	//thread safe signal that this thread is done
	SetEvent(m_eventRenderObjectDone);

}

//public call to close this thread
void Renderer::ExitThreads()
{
	//signal to the worker threads that they should stop processing and close
	BOOL bSet = SetEvent(m_eventRenderObject);
	godzassert(bSet == TRUE);

	//wait for the threads to signal that they are done
	if (m_bStartedRenderThread)
	{
		WaitForSingleObject(m_eventRenderObjectDone, INFINITE);
		m_bStartedRenderThread=false;
	}

	CloseHandle(m_eventRenderObjectDone);
}

void Renderer::StartRenderThread()
{
	m_renderData.AcquireRenderQueue();

	unsigned int threadId = NULL;

	//create the game logic thread. We cannot invoke this in the constructor because it starts up the thread
	//before the SceneManager singleton is properly created causing thread data-races
	m_renderThread = (HANDLE)_beginthreadex(NULL, 0, DoRenderThread, &m_deviceCreationParameters, 0, &threadId);
	if ( m_renderThread != 0 )
	{
		SetThreadName( threadId, "RenderThread" );
	}
	m_bStartedRenderThread = true;
}

void Renderer::SendRenderDeviceEvents(IDriver* driver)
{
#ifdef TRACK_PROFILER_RENDERER
	m_profileDeviceEvents.profilerBeginTime();
#endif

	//Render thread code......
	std::vector<RenderDeviceEvent*> localEvents;
	SceneData::RetrieveRenderDeviceEvents(localEvents);
	
	//now use our local scope resource array.....
	size_t len = localEvents.size();
	for(size_t i=0; i < len; i++)
	{
		RenderDeviceEvent* event = localEvents[i];
		driver->HandleRenderDeviceEvent(event);
		delete event;
	}

#ifdef TRACK_PROFILER_RENDERER
	m_profileDeviceEvents.profilerEndTime("Renderer::SendRenderDeviceEvents");
#endif
}

void Renderer::ParseObjectStateChanges(IDriver* driver, ObjectStateChangeEventList& list)
{
	size_t num = list.GetNumObjects();
	for(size_t i=0;i<num;i++)
	{
		ObjectStateChangeEvent* changeEvent = list[i];

		//it's valid to receive empty slots
		if (changeEvent == NULL)
			continue;

		bool handledEvent = true;

		switch(changeEvent->mType)
		{
		default:
			{
				//any types we don't handle will be sent directly to the Driver
				handledEvent = driver->HandleObjectStateChangeEvent(changeEvent, &m_renderData);
			}
			break;
		case ObjectStateChangeEventID_MaterialCreate:
			{
				MaterialCreationEvent* event = (MaterialCreationEvent*)changeEvent;
				m_renderData.AddMaterialInfo(event->mInfo);
			}
			break;
		case ObjectStateChangeEventID_MaterialUpdate:
			{
				MaterialUpdateEvent* event = (MaterialUpdateEvent*)changeEvent;
				RenderMaterialInfo* mat = m_renderData.FindMaterial(event->m_id);
				mat->matInfo.m_pShader = event->m_pShader;
			}
			break;
		case ObjectStateChangeEventID_MaterialDestroy:
			{
				MaterialDestructionEvent* event = (MaterialDestructionEvent*)changeEvent;
				m_renderData.RemoveMaterial(event->m_id);
			}
			break;
		case ObjectStateChangeEventID_MaterialTextureUpdate:
			{
				MaterialTexUpdateEvent* event = (MaterialTexUpdateEvent*)changeEvent;
				RenderMaterialInfo* mat = m_renderData.FindMaterial(event->m_id);

				ParameterInfo* p = mat->matInfo.GetParameter(event->mName);
				if (p == NULL)
				{
					mat->matInfo.AddTexture( event->mName, event->mTex.m_tex );
				}
				else
				{
					TextureParameterInfo* fp = (TextureParameterInfo*)p;
					fp->m_tex = event->mTex.m_tex;
				}
			}
			break;
		case ObjectStateChangeEventID_MaterialFloatUpdate:
			{
				MaterialFloatUpdateEvent* event = (MaterialFloatUpdateEvent*)changeEvent;
				RenderMaterialInfo* mat = m_renderData.FindMaterial(event->m_id);
				ParameterInfo* p = mat->matInfo.GetParameter(event->mName);
				if (p == NULL)
				{
					mat->matInfo.AddFloatParameter(event->mName, event->mFloatParameter);
				}
				else
				{
					//update....
					FloatParameterInfo* fp = (FloatParameterInfo*)p;
					*fp = event->mFloatParameter;
				}
			}
			break;
		case ObjectStateChangeEventID_MaterialQuadCreate:
			{
				MaterialQuadCreationEvent* event = (MaterialQuadCreationEvent*)changeEvent;
				m_renderData.AddMaterialQuad(event->m_quad);
			}
			break;
		case ObjectStateChangeEventID_MaterialQuadsClear:
			{
				MaterialQuadClearEvent* event = (MaterialQuadClearEvent*)changeEvent;
				m_renderData.ClearMaterialQuads();
			}
			break;

		case ObjectStateChangeEventID_ViewportCreation:
			{
				ViewportCreationEvent* ev = (ViewportCreationEvent*)changeEvent;
				m_renderData.AddViewport(ev->mViewportInfo);

				ViewportInterface* pViewportInterface = driver->GetViewportByWindow(ev->mViewportInfo.GetWindow());
				driver->SetViewport(pViewportInterface);
			}
			break;
		case ObjectStateChangeEventID_ViewportDestroy:
			{
				ViewportDestroyEvent* ev = (ViewportDestroyEvent*)changeEvent;
				m_renderData.RemoveViewport(ev->m_id);
			}
			break;
		case ObjectStateChangeEventID_ViewportActivate:
			{
				ViewportActivateEvent* ev = (ViewportActivateEvent*)changeEvent;
				m_renderData.SetViewport(ev->m_id);
				ViewportInterface* pViewportInterface = driver->GetViewportByWindow(ev->m_id);
				driver->SetViewport(pViewportInterface);
			}
			break;
		case ObjectStateChangeEventID_ViewportSetBackgroundColor:
			{
				ViewportSetBackgroundColorEvent* ev = (ViewportSetBackgroundColorEvent*)changeEvent;

				ViewportInterface* pViewportInterface = driver->GetViewportByWindow(ev->m_id);
				pViewportInterface->SetBackgroundColor( ev->r, ev->g, ev->b );
			}
			break;

		case ObjectStateChangeEventID_LightCreateUpdate:
			{
				LightCreationEvent* ev = (LightCreationEvent*)changeEvent;
				m_renderData.AddUpdateSceneLight(ev->m_light);
			}
			break;
		case ObjectStateChangeEventID_LightDestroy:
			{
				LightDestroyEvent* ev = (LightDestroyEvent*)changeEvent;
				m_renderData.RemoveLight(ev->m_id);
			}
			break;

		case ObjectStateChangeEventID_LightActivate:
			{
				SunLightActivateEvent* ev = (SunLightActivateEvent*)changeEvent;
				m_renderData.SetActiveSun(ev->m_light);
			}
			break;

		case ObjectStateChangeEventID_CameraCreateUpdate:
			{
				CameraCreationEvent* ev = (CameraCreationEvent*)changeEvent;
				m_renderData.AddCam(ev->m_info);
			}
			break;
		case ObjectStateChangeEventID_CameraActivate:
			{
				CameraActivateEvent* ev = (CameraActivateEvent*)changeEvent;
				m_renderData.SetActiveCam(ev->m_id);
			}
			break;
		case ObjectStateChangeEventID_CameraDestroy:
			{
				CameraDestroyEvent* ev = (CameraDestroyEvent*)changeEvent;
				m_renderData.RemoveCam(ev->m_id);
			}
			break;
		case ObjectStateChangeEventID_CameraFocusUpdate:
			{
				CameraFocusEvent* focusEvent = (CameraFocusEvent*)changeEvent;

				//update the transform for our view target
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(focusEvent->mFocusActorID);
				renderInfo->m_info.m_transform = focusEvent->m_SOT;

				
				m_renderData.AddCam(focusEvent->m_info);
			}
			break;

		case ObjectStateChangeEventID_EntityCreate:
			{
				EntityCreationEvent* ev = (EntityCreationEvent*)changeEvent;
				m_renderData.CreateEntity(ev->m_info);
			}
			break;
		case ObjectStateChangeEventID_EntityDestroy:
			{
				EntityDestroyEvent* ev = (EntityDestroyEvent*)changeEvent;
				m_renderData.ClearMaterialGroups(ev->m_id);
				m_renderData.RemoveEntity(ev->m_id);
			}
			break;
		case ObjectStateChangeEventID_EntityUpdate:
			{
				EntityUpdateEvent* ev = (EntityUpdateEvent*)changeEvent;
				m_renderData.UpdateEntity(ev->m_info);
			}
			break;
		case ObjectStateChangeEventID_EntityUpdateBones:
			{
				EntityBonesUpdateEvent* ev = (EntityBonesUpdateEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				GeometryInfo* info = &renderInfo->m_info;

				info->ClearBones();
				info->ReserveBones(ev->m_bones.size());

				size_t num = ev->m_bones.size();
				for(size_t i = 0; i < num; i++)
				{
					info->AddBone(ev->m_bones[i]);
				}
			}
			break;
		case ObjectStateChangeEventID_EntityUpdateTransform:
			{
				EntityTransformUpdateEvent* ev = (EntityTransformUpdateEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				renderInfo->m_info.m_transform = ev->m_mat;
			}
			break;
		case ObjectStateChangeEventID_EntityStatusChangeEvent:
			{
				EntityStatusChangedEvent* ev = (EntityStatusChangedEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);

				switch(ev->mEvent)
				{
				case EntityStatusChangedEvent::kEntityChangeEvent_ClearEditorMaterial:
					{
						renderInfo->m_info.m_editorMaterial = 0;
					}
					break;
				}				
			}
			break;
		case ObjectStateChangeEventID_EntityRenderDeviceEvent:
			{
				EntityRenderDeviceUpdateEvent* ev = (EntityRenderDeviceUpdateEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				renderInfo->m_info.mMap = ev->mMap;
			}
			break;
		case ObjectStateChangeEventID_EntityMaterialsChanged:
			{
				EntityMaterialsUpdateEvent* ev = (EntityMaterialsUpdateEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				
				if (renderInfo != NULL)
				{
					renderInfo->m_info.mMap = ev->mMap;

					GeometryInfo* info = &renderInfo->m_info;

					m_renderData.ClearMaterialGroups(*renderInfo);

					info->ClearMaterials();

					//Clear out all the cached renderboxes which were bound per material
					renderInfo->mVisibleDeviceObjs.clear();
					
					//Cache the material* and register the GeometryInfo as an observer of the materialinfo
					size_t num = ev->m_materials.size();
					info->ReserveMaterials(num);

					for(size_t i = 0; i < num; i++)
					{
						info->AddMaterial(ev->m_materials[i]);
					}

					m_renderData.SetupMaterialGroups(*renderInfo);
				}
			}
			break;

		case ObjectStateChangeEventID_EntityVisibilityPass:
			{
				EntityVisibilityPassEvent* ev = (EntityVisibilityPassEvent*)changeEvent;
				size_t num = ev->m_actors.size();

				//clear visibility for all elements
				{
					for (size_t i = 0; i < m_renderData.GetNumElements(); i++)
					{
						RenderableGeometryInfo* info = m_renderData.GetEntityAt(i);
						if (info != NULL)
						{
							info->m_isVisible = false;
							info->mVisibleDeviceObjs.clear();
						}
					}
				}

				for (size_t i = 0; i < num; i++)
				{
					RenderableGeometryInfo* info = m_renderData.GetEntity( ev->m_actors[i].m_id );
					godzassert( info != NULL );

					info->m_isVisible = true;

					size_t numObjs = ev->m_actors[i].m_visibleResources.size();
					for(size_t j=0; j<numObjs; j++)
					{
						RenderDeviceBox rb = ev->m_actors[i].m_visibleResources[j];
						info->mVisibleDeviceObjs.push_back(rb);
					}
				}

				m_renderData.SetSunlightView(ev->mSunlightView);
				m_renderData.SetSunlightFrustum(ev->mLightFrustum);
			}
			break;

		case ObjectStateChangeEventID_EntitySelected:
			{
				EntitySelectedEvent* ev = (EntitySelectedEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				renderInfo->m_isSelected = true;
				renderInfo->mActiveAxis = GizmoAxis_None;

				if (ev->m_PreviousID.IsValid())
				{
					RenderableGeometryInfo* oldRenderInfo = m_renderData.GetEntity(ev->m_PreviousID);
					if (oldRenderInfo != NULL)
					{
						oldRenderInfo->m_isSelected = false;
						oldRenderInfo->mActiveAxis = GizmoAxis_None;
					}
				}
			}
			break;

		case ObjectStateChangeEventID_EntityDeselected:
			{
				EntitySelectedEvent* ev = (EntitySelectedEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				renderInfo->m_isSelected = false;
				renderInfo->mActiveAxis = GizmoAxis_None;
			}
			break;

		case ObjectStateChangeEventID_EntityAxisChanged:
			{
				EntityAxisChangedEvent* ev = (EntityAxisChangedEvent*)changeEvent;
				RenderableGeometryInfo* renderInfo = m_renderData.GetEntity(ev->m_id);
				if (renderInfo != NULL)
				{
					renderInfo->mActiveAxis = ev->m_Axis;
				}
			}
			break;

		case ObjectStateChangeEventID_EntityScreenPick:
			{
				EntityScreenRayEvent* ev = (EntityScreenRayEvent*)changeEvent;
				ev->mFuture.setValue(driver->ComputePickingRay(ev->mX, ev->mY));
			}
			break;

		case ObjectStateChangeEventID_Collection:
			{
				ObjectStateEventCollection* ev = (ObjectStateEventCollection*)changeEvent;
				ParseObjectStateChanges(driver, ev->mList);
			}
			break;

		case ObjectStateChangeEventID_RenderSwitch:
			{
				EditorRenderSwitchEvent* ev = (EditorRenderSwitchEvent*)changeEvent;

				RenderOptions* opt = RenderOptions::GetInstance();

				//enable / disable deferred rendering
				if (ev->mMode == EditorRenderSwitchEvent::RenderSwitch_Forward)
					opt->m_bUseDeferred = false;
				else
					opt->m_bUseDeferred = true;
			}
			break;

		case ObjectStateChangeEventID_CreateSkyDome:
			{
				CreateUpdateSkyDomeEvent* ev = (CreateUpdateSkyDomeEvent*)changeEvent;
				m_renderData.SetSkyDome(ev->mSky);
			}
			break;
		case ObjectStateChangeEventID_RemoveSkyDome:
			{
				RemoveSkyDomeEvent* ev = (RemoveSkyDomeEvent*)changeEvent;
				m_renderData.RemoveSkyDome( ev->mSky );
			}
			break;
		}

		//unrecognized / unhandled event type. Need to add a handler for the missing type 
		godzassert(handledEvent);

#ifdef LIST_EVENTS
		Log("%s\n", changeEvent->getDebug().c_str());
#endif

		//release the event
		delete changeEvent;
	}
}

void Renderer::HandleObjectStateChanges(IDriver* driver)
{
#ifdef TRACK_PROFILER_RENDERER
	m_profileHandleWidgets.profilerBeginTime();
#endif

	ObjectStateChangeEventList list;
	SceneData::RetrieveObjectStateChangeEvents(list, 0);

#ifdef LIST_EVENTS
	Log("------------------------------------------\n");
#endif

	ParseObjectStateChanges(driver, list);

#ifdef TRACK_PROFILER_RENDERER
	m_profileHandleWidgets.profilerEndTime("Renderer::HandleWidgets");
#endif
}


} //namespace