
#include "WEntity.h"
#include "SceneManager.h"
#include "LevelNode.h"
#include "ModelResource.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "WCamera.h"
#include "ShaderResource.h"
#include <Process.h>
#include "GodzAtomics.h"
#include "HDRManager.h"
#include <CoreFramework/PlayerSkills/SkillDatabase.h>
#include "AsyncFileLoader.h"
#include "ShaderLoader.h"
#include "ShaderManager.h"
#include "RenderDeviceEvent.h"
#include "SceneData.h"
#include <CoreFramework/HUD/Hud.h>
#include "ViewportEvents.h"
#include "GenericObjectList.h"
#include "GenericPackage.h"
#include "EntityEvents.h"


namespace GODZ
{

SceneManager::SceneManager()
: GenericSingleton<SceneManager>(*this)
, m_centerMouseCursor(true)
, m_numLevels(0)
, m_viewportId(0)
, m_bIsMultiThreaded(false)
, m_skillDatabase(NULL)
, pActorIcon(NULL)
, m_renderLevel(NULL)
, m_isEditor(false)
{
	m_elements.reserve(50);

	m_config.m_bUseHWSkinning = GlobalConfig::m_pConfig->ReadBoolean("Engine","UseHWSkinning");
	m_config.m_bUseHierarchicalAnim = GlobalConfig::m_pConfig->ReadBoolean("Engine","UseHierarchicalAnimation");
	m_config.m_bUseSavedSkelFrames = GlobalConfig::m_pConfig->ReadBoolean("Engine","UseSavedSkelFrames");

	//load shaders first
	LoadShaders();

	//send event to renderer to create HDR (high dynamic range filter)
	ShaderManager* pMan = ShaderManager::GetInstance();
	ShaderResource* hdr = pMan->FindShader("Shaders\\hdr.fxo"); //get the shader 
	godzassert(hdr != NULL);

	RenderDeviceEvent* event = CreateHDREvent(hdr->GetRenderDeviceObject());
	SceneData::AddRenderDeviceEvent(event);

	m_skillDatabase = new SkillDatabase;


}

SceneManager::~SceneManager()
{
	if (m_skillDatabase != NULL)
	{
		delete m_skillDatabase;
		m_skillDatabase=NULL;
	}
}

void SceneManager::Shutdown()
{
	LevelList::iterator iter = m_pLevelInstances.begin();
	for (; iter != m_pLevelInstances.end(); iter++ )
	{
		LevelNode* ln = *iter;
		ln->Shutdown();
	}

	m_jobManager.cleanup();
}

bool SceneManager::StreamPackage(const char* packageName, Future<GenericPackage*>& future, AsyncFileObserver* observer)
{
	return m_asyncFileManager.streamPackage(packageName, &m_jobManager, future, observer);
}

void SceneManager::RemoveAsyncFileObserver(AsyncFileObserver* observer)
{
	m_asyncFileManager.removeAsyncFileObserver(observer);
}



void SceneManager::SetDevice(GODZHWND window,udword width,udword height)
{
	m_viewportId = window;
	
	mDefaultView.Init(window);
	m_viewports.AddViewport(mDefaultView);	

	ViewportCreationEvent* view = new ViewportCreationEvent(mDefaultView);
	SceneData::AddObjectStateChangeEvent(view);
}

float SceneManager::RunGameLogic()
{
	static Timer pTimer;
	float currentTime=pTimer.PerformanceTimer(TIMER_GETABSOLUTETIME);
	static float sLastTick=currentTime; //init to current time so we know no time elapsed
	static InputSystem *input = InputSystem::GetInstance();

	float fElapsedTime=currentTime-sLastTick;

	m_asyncFileManager.doTick(&m_jobManager);

	DoTick(fElapsedTime);
	Flush();		//flush game logic

	//broadcast input to the Camera / Player Controller, etc. Render thread
	//will never need direct access to these events
	input->BroadcastEvents();

	sLastTick=currentTime;

	return fElapsedTime;
}

//conduct activities outside of the game logic thread (for thread safety)
void SceneManager::Flush()
{

}



void SceneManager::OnThreadAttached()
{
}

void SceneManager::SetViewportBackgroundColor(GODZHWND window, UInt8 r, UInt8 g, UInt8 b)
{
	ViewportSetBackgroundColorEvent* viewEvent = new ViewportSetBackgroundColorEvent(window);
	viewEvent->r = r;
	viewEvent->g = g;
	viewEvent->b = b;

	SceneData::AddObjectStateChangeEvent(viewEvent);
}

void SceneManager::SetActiveViewport(GODZHWND window)
{
	m_viewportId = window;

	ViewportActivateEvent* view = new ViewportActivateEvent(window);
	SceneData::AddObjectStateChangeEvent(view);
}

void SceneManager::AddViewport(const ViewportInfo& info)
{
	m_viewports.AddViewport(info);

	//set new window as active
	m_viewportId = info.GetWindow();

	//add new viewport
	ViewportCreationEvent* view = new ViewportCreationEvent(info);
	SceneData::AddObjectStateChangeEvent(view);

	//We assume we have a valid window at this point
	SceneData::SetWindowStatus(true);
}

void SceneManager::RemoveViewport(GODZHWND window)
{
	ViewportInfo* viewInfo = m_viewports.GetWindow(window);
	ViewportID id = viewInfo->GetID();

	m_viewports.RemoveViewport(window);

	if (m_viewportId == window)
	{
		m_viewportId = NULL; //reset

		//set active viewport to something else
		size_t num = m_viewports.GetNumViewports();
		for(size_t i=0;i<num;i++)
		{
			ViewportInfo& view = m_viewports.GetViewportInfo(i);
			if (view.GetWindow() != window)
			{
				SetActiveViewport(view); //set to 1st eligible viewport
				break;
			}
		}
	}

	//send destroy event to renderer
	ViewportDestroyEvent* ev = new ViewportDestroyEvent(id);
	SceneData::AddObjectStateChangeEvent(ev);
}


void SceneManager::SetActiveViewport(const ViewportInfo& info) 
{ 
	m_viewportId = info.GetWindow();

	ViewportActivateEvent* view = new ViewportActivateEvent(info.GetWindow());
	SceneData::AddObjectStateChangeEvent(view);
}

ViewportInfo&	SceneManager::GetActiveViewport()
{
	return *m_viewports.GetWindow(m_viewportId);
}

ViewportInfo&	SceneManager::GetViewport(ViewportID id)
{
	return m_viewports.GetViewportInfo(id);
}

ViewportInfo&	SceneManager::GetDefaultViewport() 
{
	return mDefaultView;
}

void SceneManager::SetRenderLevel(LevelNode* node)
{
	m_renderLevel = node;
}

LevelNode* SceneManager::AddLevel()
{
	atomic_ptr<LevelNode> level_ptr( new LevelNode() );
	m_pLevelInstances.push_back(level_ptr);

	m_renderLevel = level_ptr;

	return level_ptr;
}

void SceneManager::DestroyLevel(LevelNode* pLevel)
{
	std::vector<atomic_ptr<LevelNode> >::iterator iter;
	for(iter=m_pLevelInstances.begin();iter!=m_pLevelInstances.end();++iter)
	{
		if ((*iter) == pLevel)
		{
			pLevel->Shutdown();
			m_pLevelInstances.erase(iter);
			break;
		}
	}
}

LevelNode*	SceneManager::GetCurrentLevel()
{
	//TODO: right now we only support 1 level
	return m_pLevelInstances[0];
}

void SceneManager::SetSunLight(LightID id)
{
	SunLightActivateEvent* ev = new SunLightActivateEvent(id);
	SceneData::AddObjectStateChangeEvent(ev);
}

bool SceneManager::GetAlwaysCenterMouseCursor() const
{
	return m_centerMouseCursor;
}


bool SceneManager::IsEditorMode()
{
	return m_isEditor;
}

void SceneManager::SetEditorMode(bool isEditor)
{
	m_isEditor = isEditor;
}

void SceneManager::DoTick(float dt)
{
	//create a new state change event in which will store all of our events for this logic frame
	//ObjectStateEventCollection* gameLogicPass = NULL;
	ObjectStateEventCollection* gameLogicPass = new ObjectStateEventCollection();

	//update HUD(s)
	std::vector<HUD*>::iterator hudIter;
	for(hudIter = m_displays.begin();hudIter != m_displays.end();hudIter++)
	{
		(*hudIter)->Update(dt);
	}


	//Multiple Levels exist within the editor....
	std::vector<atomic_ptr<LevelNode> >::iterator iter;
	for(iter=m_pLevelInstances.begin();iter!=m_pLevelInstances.end();++iter)
	{
		LevelNode* pLevel = (*iter);
		bool hasTicked = pLevel->DoTick(dt, gameLogicPass);

		WCamera* camera = pLevel->GetActiveCamera();

		//if the Level ticked, then grab all the entities for this pass...
		if (hasTicked && camera != NULL && m_renderLevel == pLevel)
		{
			ViewportInfo& view = GetActiveViewport();
			const WMatrix& v = camera->GetViewMatrix();
			const WMatrix& p = view.GetProjection();
			WMatrix combMat = v * p;
			camera->UpdateFrustum(combMat);

			//compute light frustum...
			WEntity* viewTarget = camera->GetFocus();

			Vector3 viewLoc;
			if (viewTarget != NULL)
			{
				viewLoc = viewTarget->GetLocation();
			}
			else
			{
				viewLoc = camera->GetLocation();
			}

			Vector3 lookAt = viewLoc + Vector3(0,50,0);
			Vector3 eye = lookAt + Vector3(0,7500,0);
			Vector3 forward = v.GetForward();
			forward.Normalize();
			forward *= -1500; //backup the sun just a taste
			eye += forward;

			Vector3 cameraUp;
			camera->ComputeUpVector(cameraUp);

			WMatrix lightView(1);
			ComputeViewMatrix(eye, lookAt, cameraUp, lightView);
			WMatrix lightVP = lightView * p;
			ViewFrustum lf;
			lf.ExtractPlanes(lightVP);

			EntityVisibilityPassEvent* event = new EntityVisibilityPassEvent();
			VisibilityChangeInfo visibility_state;

			//set sunlight view matrix for debugging
			event->mSunlightView = lightView;
			event->mLightFrustum = lf;

			event->m_actors.reserve(50);
			pLevel->GetRenderElements(event->m_actors, camera, lf, visibility_state); //make a pass through all elements in the scene

			//we only send only send over the delta changes for visibility passes
			bool exactSame = true;

			if (visibility_state.m_requiresPass)
			{
				exactSame = false;
			}
			else
			{
				if (m_visibleActors.size() != event->m_actors.size())
				{
					exactSame = false;
				}
				else
				{
					//check to see if they really are the same....
					for(size_t i=0;i<m_visibleActors.size();i++)
					{
						if ( !(event->m_actors[i] == m_visibleActors[i]) )
						{
							exactSame = false;
							break;
						}
					}
				}
			}

			if (!exactSame)
			{
				m_visibleActors = event->m_actors;
				gameLogicPass->mList.Add(event);
			}
			else
			{
				//free this unreferenced event
				delete event;
			}
		}
	}

	SceneData::AddObjectStateChangeEvent(gameLogicPass);

}


void SceneManager::SetAlwaysCenterMouseCursor(bool bAlwaysCenter)
{
	m_centerMouseCursor=bAlwaysCenter;
}

void SceneManager::AddHUD(HUD& hud)
{
	m_displays.push_back(&hud); 

	//send first tick so widgets can start drawing now...
	hud.Update(0.0f);
}

JobManager& SceneManager::GetJobManager()
{
	return m_jobManager;
}

void SceneManager::ComputePickingRay(Point p, WRay& ray, const WMatrix& viewMatrix)
{
	float px = 0.0f;
	float py = 0.0f;

	ViewportInfo& vp = GetActiveViewport();
	const ProjectionMatrixInfo& pm = vp.GetProjectionInfo();

	
	const WMatrix& proj = vp.GetProjection();

	
	px = ((( 2.0f*p.x) / (float)vp.m_width)  - 1.0f) / proj._11;
	py = (((-2.0f*p.y) / (float)vp.m_height) + 1.0f) / proj._22;

	// transform the ray to world space
	WMatrix m(1);
	viewMatrix.Inverse(m);

	// transform the ray's origin, w = 1.
	ray.origin    = Vector3(0.0f, 0.0f, 0.0f);
	Vector4 o(ray.origin);
	m.Mul(o);
	ray.origin.x = o.x;
	ray.origin.y = o.y;
	ray.origin.z = o.z;

	// transform the ray's direction, w = 0.
	Vector4 d(px, py, 1.0f, 0.0f);
	m.Mul(d);
	ray.dir.x = d.x;
	ray.dir.y = d.y;
	ray.dir.z = d.z;

	// normalize the direction
	ray.dir.Normalize();

	//Below is a way to get the screen ray from the renderer, but requires waiting on that thread to respond
	/*
	Future<WRay> future;
	EntityScreenRayEvent* ev = new EntityScreenRayEvent(p.x,p.y,future);
	SceneData::AddObjectStateChangeEvent(ev);
	while(!future.isReady());

	ray = future.getValue();
	*/
}



}



