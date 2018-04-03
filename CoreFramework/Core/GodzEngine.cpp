
#include "GodzEngine.h"
#include "DeviceCreation.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "GenericObject.h"
#include "GenericObjData.h"
#include "PackageList.h"
#include "GenericPackage.h"
#include "WEntity.h"
#include "LevelNode.h"
#include "Layer.h"
#include "Mesh.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include "GenericObjData.h"
#include "Mesh.h"
#include <CoreFramework/Reflection/ClassProperty.h>
#include "WDynamicLight.h"
#include "SceneData.h"
#include "IDriver.h"
#include "ResourceManager.h"
#include "RenderOptions.h"

namespace GODZ
{

////////////////////////////////////////////////////////////////////////////////////////////////

GODZ_API void  AddViewport(GODZHWND viewportId)
{
	SceneManager* smgr = SceneManager::GetInstance();
	ViewportInfo view;
	view.Init(viewportId);
	smgr->AddViewport(view);
}

GODZ_API void  RemoveViewport(GODZHWND viewportId)
{
	SceneManager* smgr = SceneManager::GetInstance();
	smgr->RemoveViewport(viewportId);
}

GODZ_API void  SetViewport(GODZHWND viewportId)
{
	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetActiveViewport(viewportId);
}

GODZ_API void SetBackgroundColor(GODZHWND viewportId, UInt8 r, UInt8 g, UInt8 b)
{
	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetViewportBackgroundColor(viewportId, r,g,b);
}

GODZ_API float  RunGameLogic()
{
	SceneManager* smgr = SceneManager::GetInstance();
	return smgr->RunGameLogic();
}

GODZ_API void  SetSunLight(WDynamicLight* sun)
{
	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetSunLight(sun->GetLightID());
}

//clears the global outdoor scene light
GODZ_API void  ResetSunLight()
{
	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetSunLight(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////

GODZ_API void  StartRenderThread(bool isEditor, bool runAsyncRenderThread)
{
	GodzEngine* eng = GodzEngine::GetInstance();
	eng->CreateRenderThread(isEditor, runAsyncRenderThread);
}

GODZ_API void  ExitRenderThread()
{
	GodzEngine* engine = GodzEngine::GetInstance();
	engine->EndRenderThread();
}

GODZ_API void  DoRenderPass()
{
	GodzEngine* engine = GodzEngine::GetInstance();
	engine->DoRenderPass();
}

GODZ_API void  Close()
{
	//Performs cleanup
	GenericObject::DestroyAllObjects();
}

GODZ_API LevelNode*  AddLevel()
{
	SceneManager* smgr = SceneManager::GetInstance();
	return smgr->AddLevel();
}

GODZ_API WEntity*  SpawnActor(LevelNode* world, const char* classname, Vector3& pos, Vector3& rot, Layer* p)
{
	HString s(classname);
	GenericClass* cls = StaticLoadObject(s);

	if (cls != NULL)
	{
		return p->SpawnActor(cls, pos, Quaternion::getIdentity());
	}

	return NULL;
}

GODZ_API WEntity*  SpawnActorByHash(LevelNode* world, unsigned int hash, Vector3& pos, Vector3& rot, Layer* p)
{
	HString s(hash);
	GenericClass* cls = StaticLoadObject(s);

	if (cls != NULL)
	{
		return p->SpawnActor(cls, pos, Quaternion::getIdentity());
	}

	return NULL;
}

GODZ_API bool  IsA(GenericObject* obj, const char* classname)
{
	HString s(classname);
	GenericClass* cls = StaticLoadObject(s);

	if (cls != NULL)
	{
		return obj->IsA(cls);
	}

	return false;
}

GODZ_API bool  IsObjectDerivedFrom(const char* objA, const char* objB)
{
	HString sA(objA);
	GenericClass* classA = StaticLoadObject(sA);

	HString sB(objB);
	GenericClass* classB = StaticLoadObject(sB);

	return classA->IsA(classB);
}

GODZ_API bool  IsClassDerivedFrom(UInt32 classNameA, const char* classnameB)
{
	HString sA(classNameA);
	GenericClass* classA = StaticLoadObject(sA);

	HString sB(classnameB);
	GenericClass* classB = StaticLoadObject(sB);

	return classA->IsA(classB);
}

GODZ_API GenericPackage*  FindPackage(UInt32 packageName)
{
	HString s(packageName);
	return GenericObjData::m_packageList.FindPackageByName(s);
}

GODZ_API void  GetEntityLocation(WEntity* ptr, Vector3& vec)
{
	vec = ptr->GetLocation();
}

GODZ_API void  UpdateCameraView(WCamera* ptr, int mouseX, int mouseY, int width, int height, float dt)
{
	ptr->UpdateView(mouseX, mouseY, width, height, dt);
}

GODZ_API void  MoveCameraForward(WCamera* ptr, float mousedelta)
{
	InputBroadcast broadcast;
	broadcast.m_fDelta = mousedelta;
	broadcast.m_kType = IC_KeyPressed;

	if (mousedelta < 0.0f)
	{
		broadcast.m_keyState.m_kAction = KA_MoveDown;
	}
	else
		broadcast.m_keyState.m_kAction = KA_MoveUp;

	ptr->ReceiveInputEvent(broadcast);
}

GODZ_API void  GetForwardVector(WEntity* wentity, Vector3& forward)
{
	const WMatrix3& m = wentity->GetOrientationMatrix();
	forward = m.GetForward();
}

GODZ_API WEntity*  PickEntityAtPoint(LevelNode* ptr, Point& p)
{
	SceneManager* smgr = SceneManager::GetInstance();
	WRay ray;
	smgr->ComputePickingRay(p, ray, ptr->GetActiveCamera()->GetViewMatrix());

	//now send this ray to the Level, ask for which entity we hit
	return ptr->PickEntity(ray);
}

GODZ_API void  SetClassPropertyAt(GenericObject* obj, UInt32 index, const char* text)
{
	GenericClass* c = obj->GetRuntimeClass();

	std::vector<ClassProperty*> properties;
	c->GetProperties(properties);
	properties[index]->SetText(obj, text);
}

GODZ_API Layer*  AddLayer(LevelNode* ptr, const char* name)
{
	return ptr->AddLayer(name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GodzEngine::GodzEngine()
: GenericSingleton<GodzEngine>(*this)
, m_render(NULL)
{
}

GodzEngine::~GodzEngine()
{
	//Log("godzengine close render thread\n");
	EndRenderThread();
}

void GodzEngine::CreateRenderThread(bool isEditor, bool runAsyncRenderThread)
{
	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetEditorMode(isEditor);

	//Get the active viewport and use that
	GODZ::DeviceCreation dc;
	dc.m_deviceWindow = (HWND)smgr->GetHWND();

	tagRECT lpRect;
	BOOL hr =  GetWindowRect(dc.m_deviceWindow,&lpRect);
	godzassert(hr == TRUE);
	int width = lpRect.right - lpRect.left;
	int height = lpRect.bottom - lpRect.top;

	//Log("godzengine w:%d h:%d \n", width, height);

	dc.height = height;
	dc.width = width;
	dc.m_isEditor = isEditor;

	m_render = new Renderer(dc);

	if ( runAsyncRenderThread )
	{
		m_render->StartRenderThread();
	}
	else
	{
		RenderOptions* opt = RenderOptions::GetInstance();
		m_render->SetViewBoxSize(opt->m_viewerBoxSize);

		// Perform the RenderThread init in this thread. Set it up for
		// running in the *MAIN* thread
		ResourceManager* renderDeviceResourceMngr = ResourceManager::GetInstance();
		Future<IDriver*>& driverFuture = SceneData::GetDriver();
		godzassert(driverFuture.isReady()); //wait til ready

		IDriver* pDriver = driverFuture.getValue();

		dc.m_pRenderDeviceResMngr = renderDeviceResourceMngr;
		pDriver->CreateDevice(dc);
	}
}

void GodzEngine::EndRenderThread()
{
	if (m_render != NULL)
	{
		m_render->ExitThreads();
		delete m_render;
		m_render = NULL;
	}
}

void  GodzEngine::DoRenderPass()
{
	if ( m_render )
	{
		m_render->DoRenderPass();
	}
}



}
