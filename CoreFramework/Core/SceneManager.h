/* ===========================================================
	Scene Manager

	Psalm 55:22
	"Cast your cares on the Lord and he will sustain you; he
	will never let the righteous fall."

	Created Jan 2, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_SCENEMANAGER_H_)
#define _SCENEMANAGER_H_

#include "SceneNode.h"
#include "GenericSingleton.h"
#include "SceneInfo.h"
#include "JobManager.h"
#include "AsyncFileLoader.h"
#include "EntityPass.h"
#include "atomic_ref.h"
#include "LevelNode.h"
#include <CoreFramework/Math/WBox.h>

namespace GODZ
{	
	static const char* SCENEMANAGER = "SceneManager";

	//Forward Declarations
	class WEntity;
	class IDriver;
	class ModelResource;
	class Win32Interface;
	class ViewportInterface;
	class WModel;
	class WCamera;
	class ShadowMapManager;
	class HUD;
	class SkillDatabase;
	class WDynamicLight;
	
	struct GODZ_API SceneManagerConfigData
	{
		bool m_bUseHierarchicalAnim;	//enables hierachial animation techniques
		bool m_bUseSavedSkelFrames;		//use saved motion data instead of computing vertex/normal/tangents every frame
		bool m_bUseHWSkinning;		//use hardware skinning?
	};


	class GODZ_API SceneManager 
		: public GenericSingleton<SceneManager>
	{	
	public:		
		SceneManager();
		~SceneManager();				

		JobManager& GetJobManager();

		GODZHWND				GetHWND() { return m_viewportId; }

		void				SetSunLight(LightID id);

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Level Management 
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		LevelNode*			AddLevel();						//creates a new level and returns a pointer to it
		void				DestroyLevel(LevelNode* node);	//destroys the level
		void				SetRenderLevel(LevelNode* node);

		//queries the current viewport for the Level its currently looking at
		LevelNode*			GetCurrentLevel();

		//tick all the Level Instances
		void				DoTick(float dt);

		void				Shutdown();

		////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Renders all the active viewports
		void				Draw(IDriver* Driver);

		//Returns true if scene manager is in editor mode
		bool				IsEditorMode();
		void				SetEditorMode(bool isEditor);

		//returns true if user has control of mouse cursor (editor mode)
		bool				GetAlwaysCenterMouseCursor() const;
		
		//sets the engine to editor mode (display mouse cursor)
		void				SetAlwaysCenterMouseCursor(bool bAlwaysCenter);

		void				ComputePickingRay(Point p, WRay& ray, const WMatrix& viewMatrix);

		//Loads the selected rendering device from the configuration file
		void				SetDevice(GODZHWND g_hWnd, udword width, udword height);

		//Returns pointer to the skill database
		SkillDatabase*		GetSkillDatabase() { return m_skillDatabase; }


		//loads a Package File asynchronously...
		bool				StreamPackage(const char* packageName, Future<GenericPackage*>& future, AsyncFileObserver* observer = NULL);

		void				RemoveAsyncFileObserver(AsyncFileObserver* observer);

		const SceneManagerConfigData& GetSceneManagerConfigData() { return m_config; }

		void				AddHUD(HUD& hud);
		

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Viewports
		//////////////////////////////////////////////////////////////////////////////////////////////

		void				AddViewport(const ViewportInfo& info);
		void				RemoveViewport(GODZHWND window);
		void				SetActiveViewport(GODZHWND window);
		void				SetViewportBackgroundColor(GODZHWND window, UInt8 r, UInt8 g, UInt8 b);

		//Sets the viewport that has the current focus
		void				SetActiveViewport(const ViewportInfo& info);
		ViewportInfo&		GetActiveViewport();
		ViewportInfo&		GetViewport(ViewportID id);

		//returns the INITIAL viewport -- for Frame 0. These settings will be copied over to the render thread and after that
		//point you should call GetViewport()
		ViewportInfo&		GetDefaultViewport();

		//////////////////////////////////////////////////////////////////////////////////////////////
		// Logic Threads
		//////////////////////////////////////////////////////////////////////////////////////////////

		float				RunGameLogic();

		
		void				Flush();									//do cleanup outside of the game logic thread

		void				OnThreadAttached();							//notification that a thread has been attached

	
		bool				IsMultiThreaded() { return true; }


		///////////////////////////////////////////////////////////////////////////////////////////////

	protected:

		AsyncFileManager&	GetAsyncFileManager() { return m_asyncFileManager; }



	private:
		bool							m_isEditor;
		ViewportInfo					mDefaultView;			//returns the INITIAL viewport -- for Frame 0.

		std::vector<HUD*>				m_displays;				//List of HUDs
		JobManager						m_jobManager;			//Job Manager (manages tasks; multi-core)
		AsyncFileManager				m_asyncFileManager;
		bool							m_bIsMultiThreaded;
		
		Mesh *							pActorIcon;				//3d icon - represents an actor in editor
		
		SkillDatabase*					m_skillDatabase;
		GODZHWND						m_viewportId;
		Viewports						m_viewports;			//local copy of viewports
		
		std::vector<EntityPassList> m_visibleActors;

		// ==========================================================================
		// config vars
		// ==========================================================================
		SceneManagerConfigData			m_config;

		// ==========================================================================
		// Scene Management vars
		// ==========================================================================
		typedef std::vector<atomic_ptr<LevelNode> > LevelList;
		LevelList			m_pLevelInstances;			//All the levels that exist within this game
		LevelNode*						m_renderLevel;
		size_t							m_numLevels;
		

		// ==========================================================================
		// Rendering vars
		// ==========================================================================
		
		std::vector<SceneElement>		m_elements;				//elements that were computed within the view frustum for this frame

		// ==========================================================================
		// Editor mode vars
		// ==========================================================================
		bool							m_centerMouseCursor;		//Set to false if editor mode is active
	};




	#define TheSceneManager SceneManager::GetInstance()
}

#endif