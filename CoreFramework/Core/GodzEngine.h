/* ===========================================================

	Created Feb 21, '10 by Richard Osborne
	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__GodzEngine_H__)
#define __GodzEngine_H__

#include "CoreFramework.h"
#include "GenericSingleton.h"
#include "GodzAtomics.h"
#include "AsyncFileObserver.h"
#include <CoreFramework/Math/WGeometry.h>

namespace GODZ
{
	class Renderer;
	class LevelNode;
	class WEntity;
	class Mesh;
	class WCamera;
	class Layer;
	class WDynamicLight;

	struct GODZ_API ClassTreeNode
	{
		unsigned int mHashCode;			//my class hash code
		bool mIsPackage;
		const char* mName;
		unsigned int mIndex;			//array index #
		unsigned int mPackageHash;		//This package's hash code
	};

	struct GODZ_API ClassPropertyInfo
	{
		const char* mName;
		const char* mText;
	};

	/*
	* Exists as a bridge between C# and C++ (exposes stuff to the editor)
	*/
	class GODZ_API GodzEngine 
		: public GenericSingleton<GodzEngine>
	{	
	public:
		GodzEngine();
		~GodzEngine();

		void CreateRenderThread(bool isEditor, bool runAsyncRenderThread);
		void EndRenderThread();
		void  DoRenderPass();

	private:
		Renderer* m_render;
	};


	//C# exports
	GODZ_API void  DoRenderPass();
	GODZ_API void  StartRenderThread(bool isEditor, bool runAsyncRenderThread);
	GODZ_API void  ExitRenderThread();
	GODZ_API void  Close();

	//Returns all the current loaded classes / packages to populate a tree control
	GODZ_API void  BuildPackageTree();
	GODZ_API unsigned int  GetNumTreeNodes();
	GODZ_API void  GetClassTreeNode(unsigned int index, ClassTreeNode& n);

	//Level
	GODZ_API LevelNode*  AddLevel();
	GODZ_API Layer*  AddLayer(LevelNode* ptr, const char* layerName);
	GODZ_API WEntity*  PickEntityAtPoint(LevelNode* ptr, Point& p);
	GODZ_API WEntity*  SpawnActor(LevelNode* world, const char* classname, Vector3& pos, Vector3& rot, Layer* layerIndex);
	GODZ_API WEntity*  SpawnActorByHash(LevelNode* world, unsigned int hash, Vector3& pos, Vector3& rot, Layer* layerIndex);

	//---------------------------------------------------------------------------------------------------
	//Object functions
	GODZ_API bool  IsA(GenericObject* obj, const char* classname);
	GODZ_API bool  IsObjectDerivedFrom(const char* objectClassName, const char* classname);
	GODZ_API bool  IsClassDerivedFrom(UInt32 classNameA, const char* classnameB);
	GODZ_API void  SetClassPropertyAt(GenericObject* obj, UInt32 index, const char* text);

	//---------------------------------------------------------------------------------------------------
	//Package functions
	GODZ_API GenericPackage*  FindPackage(UInt32 packageName);

	//checks to see how many packages have been loaded since the last check. Intended to work inconjuction with StreamPackages()
	GODZ_API UInt32  GetNumPackagesLoaded();

	//---------------------------------------------------------------------------------------------------
	//Entity functions
	GODZ_API void  GetEntityLocation(WEntity* ptr, Vector3& vec);

	GODZ_API void  GetForwardVector(WEntity* wentity, Vector3& forward);

	
	

	//---------------------------------------------------------------------------------------------------
	//Camera
	GODZ_API void UpdateCameraView(WCamera* ptr, int mouseX, int mouseY, int width, int height, float dt);
	GODZ_API void MoveCameraForward(WCamera* ptr, float mousedelta);
	

	//scenemngr
	GODZ_API void AddViewport(GODZHWND viewportId);
	GODZ_API void RemoveViewport(GODZHWND viewportId);
	GODZ_API void SetViewport(GODZHWND viewportId);
	GODZ_API void SetBackgroundColor(GODZHWND viewportId, UInt8 r, UInt8 g, UInt8 b);

	GODZ_API float RunGameLogic();
	GODZ_API void SetSunLight(WDynamicLight* sun);
	GODZ_API void ResetSunLight(); //clears sunlight
}

#endif //__GodzEngine_H__