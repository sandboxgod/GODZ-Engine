// GodzGame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GodzGame.h"
#include "ShaderManager.h"

//We always need to include this header file in order to access the resources provided
//by the Engine.
#include "GodzExports.h"
#include "SpriteWidget.h"
#include "SkeletalMesh.h"
#include "GSAFileImporter.h"
#include "WDynamicLight.h"
#include "AABBTree.h"
#include "AnimationController.h"
#include "SkyDome.h"
#include "Bot.h"
#include "SpriteWidget.h"
#include "RenderOptions.h"
#include "ViewportInterface.h"
#include "HUD.h"
#include "Material.h"




using namespace GODZ;

float g_fAngTurn=0;
WVector lightPos;
WCharacter *wc=0;
Bot* seraphBot = 0;

SpriteWidget g_pIntroWidget;			//intro image


Mesh *levelMesh = NULL;	
WCamera* cam = 0;
SceneNode* pScene = 0;
LevelNode* pLevel = NULL;
Mesh *skymesh = NULL;	
WEntity* ogCharacter = 0;
WEntity* ogCharacter2 = 0;
WEntity* pFocus=0;		//current character that has focus
WPlayerController* wpc = 0;
WDynamicLight* g_pMainLight = NULL;
WVector offset;
WVector lightDir;
HUD myHUD;


class GameHUD : public IHud
{
public:
	void Render(GODZ::ICanvas *pCanvas)
	{
		//if (g_pIntroWidget)
		{
			pCanvas->DrawWidget(&g_pIntroWidget);
		}

		/*
		if (!wc)
			return;

		WVector vel = wc->GetVelocity();
		AnimController *cntrl = wc->GetAnimController();
		rstring text = GetString("Vel %f %f %f Fire %d Time %f",vel.x,vel.y,vel.z,wc->IsFiring(),cntrl->GetTime() );
		const char* ptext = text;
		pCanvas->DrawText(ptext, 10,65, 400, 65);

		WVector X,Y,Z;
		WMatrix16f &mat = wc->GetMatrix();
		mat.GetAxes(X,Y,Z);

		vel.Normalize();
		WVector axis(vel.Dot(X), vel.Dot(Y), vel.Dot(Z));
		axis.Normalize();
		rstring text2 = GetString("X %f Y %f Z %f",axis.x, axis.y, axis.z );
		const char* ptext2 = text2;
		pCanvas->DrawText(ptext2, 10,75, 400, 65);
		*/

		/*
		if (cam && wc)
		{
			const WMatrix16f& pMat = wc->GetMatrix();
			const WMatrix16f& camMat = cam->GetMatrix();
			//rstring text1 = GetString("%f %f %f %f", pMat._11, pMat._12, pMat._13, pMat._14);
			//rstring text2 = GetString("%f %f %f %f", pMat._21, pMat._22, pMat._23, pMat._24);
			//rstring text3 = GetString("%f %f %f %f", pMat._31, pMat._32, pMat._33, pMat._34);

			WVector vel = wc->GetVelocity();
			AnimController *cntrl = wc->GetAnimController();
			float fTime = 0;
			if (cntrl)
			{
				fTime = cntrl->GetTime();
			}

			RenderOptions *ro=RenderOptions::GetInstance();
			rstring text1 = "Keys 1-9 toggle the Number of Split Maps";			
			const char* ptext = text1;
			pCanvas->DrawText(ptext, 10,65, 400, 65);



			rstring text4 = GetString("%f %f %f %f", pMat._41, pMat._42, pMat._43, pMat._44);
			const char* ptext4 = text4;
			pCanvas->DrawText(ptext4, 10,95, 400, 65);

			if (g_pMainLight)
			{
//				lightDir = g_pMainLight->GetLightDir();
//				rstring text5 = GetString("light dir : %f %f %f %f", lightDir.x, lightDir.y, lightDir.z, 1);
//				const char* ptext5 = text5;
//				pCanvas->DrawText(ptext5, 10,135, 400, 65);
			}



			WCamera *activeCam = cam;
			if (activeCam)
			{
				//
				//if (!activeCam->HasFocus() && pLevel)
				//{
				//	activeCam  = pLevel->GetActiveCamera();
				//}
				

				WVector eye = activeCam->GetEye();
				rstring text5 = GetString("eye : %f %f %f %f", eye.x, eye.y, eye.z, 1);
				const char* ptext5 = text5;
				pCanvas->DrawText(ptext5, 10,155, 400, 65);

				WVector target = activeCam->GetTarget();
				text5 = GetString("target : %f %f %f %f", target.x, target.y, target.z, 1);
				ptext5 = text5;
				pCanvas->DrawText(ptext5, 10,175, 400, 65);

				WVector camDir = target-eye;
				camDir.Normalize();
				text5 = GetString("camDir : %f %f %f %f", camDir.x, camDir.y, camDir.z, 1);
				ptext5 = text5;
				pCanvas->DrawText(ptext5, 10,195, 400, 65);
			}
		}

		if (pFocus)
		{
			IWindow* WI = WindowManager::GetInstance();

			int mouse_x = 0, mouse_y = 0;
			WI->GetMousePos(mouse_x,mouse_y);


			WVector pos = pFocus->GetLocation();
			rstring text1 = GetString("pos: %d %d", mouse_x, mouse_y);
			const char* ptext4 = text1;
			pCanvas->DrawText(ptext4, 10,115, 400, 65);
		}
		*/

		/*
		if (seraphBot)
		{
			const WVector& loc = wc->GetLocation();
			rstring text3 = GetString("%f %f %f", seraphBot->cross.x, seraphBot->cross.y, seraphBot->cross.z);
			const char* ptext3 = text3;
			pCanvas->DrawText(ptext3, 10,85, 400, 65);
		}*/

	}
};

class SkelInput : public GODZ::InputListener
{
public:
	virtual void ReceiveInputEvent(const InputBroadcast &event)
	{
		InputSystem *input = InputSystem::GetInstance();
		RenderOptions *ro = RenderOptions::GetInstance();

		if ( event.m_kType == IC_KeyPressed && event.m_keyState.m_state == KS_KeyDown)
		{
			//parallel split - change num of view frustum spits
			for (int i=1;i<9;i++)
			{
				VirtualKeyCode newKey = '0' + i;
				if (event.m_keyState.m_key == newKey)
				{
					ro->m_numSplits = i;
				}
			}

			for (int i=0;i<9;i++)
			{
				VirtualKeyCode newKey = VK_NUMPAD0 + i;
				if (event.m_keyState.m_key == newKey)
				{
					ro->m_nSplitToRender = i;
				}
			}
		}


		
	}
};

class MyHUDRenderDeviceListener : public RenderDeviceListener
{
public:
	MyHUDRenderDeviceListener(SceneManager* sceneMngr)
	{
		
		gdzTexPackage = LoadPackage("GodzTex", PF_GDZ);
		GenericObjectIterator gdzTexIter = GetAllObjects(Material::GetClass(), gdzTexPackage);
		pMat = SafeCast<Material>(gdzTexIter[0]);
		//g_pIntroWidget = BuildObject<SpriteWidget>();

		//We have to register ourself as a RenderDeviceListener after the textures have registered themselves
		RenderDeviceListenerInfo info;
		info.m_id = RenderDevice_DEFAULT_DEVICE_ID;
		info.m_listener = this;
		sceneMngr->AddRenderDeviceListener(info);
	}

	virtual void		OnDeviceReady(DeviceReadyEvent& event)
	{
		
		//load intro image				
		g_pIntroWidget.SetMaterial(pMat);

		myHUD.AddWidget(&g_pIntroWidget);
		event.pDriver->AddHud(&myHUD);


		float width= GetSystemMetrics(SM_CXSCREEN);
		float height = GetSystemMetrics(SM_CYSCREEN);
		TextureResource *pTex = pMat->GetTexture();
		int imgW, imgH;
		pTex->GetUSize(imgW,imgH);
		g_pIntroWidget.SetScale( width/float(imgW), height/float(imgH)); //screen resolution / image dimension
		
	}

protected:
	GenericPackage*					gdzTexPackage;
	Material *pMat;

};

class MyHUDRemoveWidgetRenderDeviceListener : public RenderDeviceListener
{
public:
	MyHUDRemoveWidgetRenderDeviceListener(SceneManager* sceneMngr)
	{
		RenderDeviceListenerInfo info;
		info.m_id = RenderDevice_DEFAULT_DEVICE_ID;
		info.m_listener = this;
		sceneMngr->AddRenderDeviceListener(info);
	}

	//Implements RenderDeviceListener interface
	virtual void		OnDeviceReady(DeviceReadyEvent& event)
	{
		myHUD.RemoveWidget(&g_pIntroWidget);			//clear the intro image from the HUD

		//set the foreground color of this application
		event.pDriver->SetBackgroundColor(0,0,0);
		event.pDriver->SetForegroundColor(200,120,120,20);
	}


};

void LoadResources()
{

	Mesh* mushroom = 0;
	SceneManager *smgr = SceneManager::GetInstance();	

	
	//Load the package containing our mushrooms!
	GenericPackage *decoPkg = LoadPackage("mushy", PF_GDZ);

	//Load the package containing the meshes for the level
	GenericPackage *boxPackage = LoadPackage("Overguard", PF_GDZ); 
	assert(boxPackage!=NULL);
	
	
	GenericPackage *levelPackage = LoadPackage("rubtest2", PF_GDZ); //floor //rubtest2

	//Ask the engine to return all mesh objects belonging to our mushroom package.
	{
		GenericObjectIterator iter = GetAllObjects(Mesh::GetClass(), levelPackage);
		levelMesh = SafeCast<Mesh>(iter[0]);

		assert(levelMesh!=NULL);


		//get my mushroom
		GenericObjectIterator iter2 = GetAllObjects(Mesh::GetClass(), decoPkg);
		mushroom = SafeCast<Mesh>(iter2[0]);

		assert(mushroom!=NULL);
	}

	//---------------------------------------------------------------------------------
	GenericPackage *domePackage = LoadPackage("skydome", PF_GDZ);

	

	{

		//Ask the engine to return all mesh objects belonging to our mushroom package.
		GenericObjectIterator iter = GetAllObjects(Mesh::GetClass(), domePackage);
	
		size_t size=iter.GetNumObjects();
		skymesh = SafeCast<Mesh>(iter[0]);
	}

	assert(skymesh!=NULL);




	//Define the world boundary constraint. 
	float uniformScale=6;
	WBox box= levelMesh->GetBounds();
	WBox nonScaledBox = box;


	

	box.ExpandBy(uniformScale);
	//box.ExpandBy(5);		//expand the world so we can fit the level in

	pLevel = smgr->AddLevel();
	LevelNode *Level=pLevel;
	Level->CreateSceneLight();


	//Add child scene nodes that breaks up the world into regions to optimize performance
	//and manage various areas within the world.
	pScene = pLevel->AddScene(SNF_Divider, box); //adds a static collidee tree
	assert(0); //change to regular octree brushes not supported atm
	pScene->AddModel(levelMesh->GetMeshInstance(0)->GetModelResource(0));
	//smgr->AddOctreeSceneNode(box, SNF_Spatial); //adds a dynamic collision tree

	
	//Add the Camera to the scene after octree has been created.
	//cam = smgr->AddFPSCamera(0,WVector(63.0f, -150.0f, -180.0f));
	cam = SafeCast<WCamera>(pLevel->SpawnActor(WCamera::GetClass(),WVector(63.0f, -150.0f, -180.0f)));
	cam->OnFocusGained();

	Log("Starting the engine example....\n");

	//Get the Level from the scene manager
	
	



	//WEntity* floor = Level->SpawnActor(WEntity::GetClass(), WVector(784, 150, -2638));
	//floor->SetMesh(levelMesh);
	//floor->SetCanCastShadow(false);



	//Ask the engine to return all mesh objects belonging to our simple level package.
	GenericObjectIterator iter = GetAllObjects(SkeletalMesh::GetClass(), boxPackage);
	SkeletalMesh *skMesh = NULL;	
	int size=iter.GetNumObjects();
	for(int i=0;i<size;i++)
	{
		if (strcmp(iter[i]->GetName(),"OG")==0)
		{
			skMesh = SafeCast<SkeletalMesh>(iter[i]);
		}
	}
	assert(skMesh!=NULL);

	
	//Get the Level from the scene manager
	WEntity *playerC = Level->SpawnActor(WPlayerController::GetClass());
	wpc = SafeCast<WPlayerController>(playerC);
	cam->SetCameraMode(WCamera::CT_BehindView);
	wpc->SetCamera(cam);
	Level->SetClientPlayer(wpc);
	

	
	ogCharacter2 = Level->SpawnActor(WCharacter::GetClass(), WVector(690, 780, -2208)); // test slope //180
	ogCharacter2->SetRotation(WRotator(3.14f,0,0));
	ogCharacter2->SetMesh(skMesh);	
	wc = SafeCast<WCharacter>(ogCharacter2);
	wc->Initialize();
	pFocus = ogCharacter2;

	//possess the character
	wpc->PossessCharacter(wc);
	

	//won't have gravity not a Moby
	WEntity *mushy = Level->SpawnActor(WEntity::GetClass(), WVector(540, -200, -1192)); 
	mushy->SetMesh(mushroom);	
	mushy->SetDrawScale(WVector(19,19,19));

	WEntity *mushy2 = Level->SpawnActor(WEntity::GetClass(), WVector(-200, -300, -1192)); 
	mushy2->SetMesh(mushroom);	
	mushy2->SetDrawScale(WVector(9,9,9));

	//WEntity *ogDecoration = Level->SpawnActor(WEntity::GetClass(), WVector(540, -400, -1192)); 
	//ogDecoration->SetMesh(skMesh);	

	
	//Add some lights to the scene
	//z - back away from character
	//z = -2208 center on character
	// z= -3500 back away look at character
	//WEntity* pent = Level->SpawnActor(WDynamicLight::GetClass(), WVector(745, 1000, -4500)); //WVector(745, 350, -1200)
	

	//WVector lightLoc = WVector(745, 1000, -3500);
	//pLevel->SetLightPosition(lightLoc);
	//WVector playerLoc = ogCharacter2->GetLocation();
	//offset.x = lightLoc.x - playerLoc.x;
	//offset.y = lightLoc.y - playerLoc.y;
	//offset.z = lightLoc.z - playerLoc.z;


	//old light source
	//lightDir.x = -0.2f;
	//lightDir.y = 0.6f;
	//lightDir.z = -0.9f;

	//light source staring down (directly overhead)
	lightDir.x = -0.028f;
	lightDir.y = 0.99f;
	lightDir.z = -0.12f;

	//light source is right behind the viewer
	lightDir.x = -0.0f;
	lightDir.y = 0.23f;
	lightDir.z = -0.97f;

	// PSSM demo
	//lightDir.x = 0.42874649f;
	//lightDir.y = -0.51449573f;
	//lightDir.z = 0.74261063f;
	
	WDynamicLight* pLight = pLevel->GetSceneLight();
	pLight->SetLightDir(lightDir);
	pLight->SetFocus(ogCharacter2);
	pLight->SetMouseLook(false);

	
	//g_pMainLight->m_lightrotation = DEG2RAD(-150);
	//g_pMainLight->SetLightDir(lightDir);
	//g_pMainLight->SetFar(10000); //new test

	//g_pMainLight->SetLightColor(0.8,0.6,1.0);
	//g_pMainLight->SetLightRange(600,600,600);
	

	WVector center = nonScaledBox.GetCenter();
	ViewMatrixInfo vmi;
	vmi.up=WVector(0,1,0);
	vmi.lookAt=WVector(center.x,0,center.z);
	vmi.eye=WVector(center.x,350,center.z);
	//g_pMainLight->SetViewMatrixInfo(vmi);

	WVector loc = center;
	loc.y = 1200;
	//g_pMainLight->SetLocation(loc);
	

	//ViewportInterface* pDefaultViewport = pDriver->GetViewport(0);
	//pDefaultViewport->SetActiveCamera(cam->GetId());
	ViewportInfo& vinfo = smgr->GetViewport(0);
	vinfo.SetCamera(cam->GetId());
	vinfo.SetLevel(Level->GetId());
	

}

//We could've used int main() but I like to use this version so we don't get
//the console window.
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//Register a window with the engine DLL.
	IWindow *device = WindowManager::GetInstance();
	device->RegisterWindow("GODZ");	

	//Get the scene manager and start preparing the scene.
	SceneManager *smgr = SceneManager::GetInstance();		

	//register listener
	SkelInput pInput;
	InputSystem *is = InputSystem::GetInstance();
	is->AddInputListener(&pInput);


	MyHUDRenderDeviceListener myHUD(smgr);
	
	//display the window now that everything is prepared offscreen
	device->Present();
	
	
	smgr->StartRenderThread();
	LoadResources();

	
	MyHUDRemoveWidgetRenderDeviceListener removeLoadingScreen(smgr);
	
	
	//smgr->StartLogicThread();

	while(device->Run())
	{		
		smgr->RunGameLogic();
	}

	smgr->ExitThreads();

	//Performs cleanup
	GenericObject::DestroyAllObjects();

	return 0;
}
