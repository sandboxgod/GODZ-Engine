/* ===========================================================


	Copyright (c) 2012
	========================================================== 
*/

#if !defined(__RENDERLIST__H__)
#define __RENDERLIST__H__

#include "GenericObject.h"
#include <CoreFramework/Collision/CollisionPrimitive.h>
#include "GeometryInfo.h"
#include "SceneLight.h"
#include "CameraInfo.h"
#include <CoreFramework/Math/ViewFrustum.h>
#include <CoreFramework/Math/WMatrix.h>

namespace GODZ
{
	class SceneNode;
	class MeshInstance;
	class ModelResource;
	class Brush;
	class CollisionPrimitive;
	class ICollisionCallback;
	struct SceneInfo;


	typedef size_t ViewportID;
	typedef size_t LevelID;			//level identifier


	enum ESceneElementType
	{
		ET_INVALID,
		ET_RESOURCE,
		ET_ENTITY,
		ET_BRUSH
	};

	//Intended to represent any object that will be rendered. Can probably just be replaced with GeometryInfo though
	class GODZ_API SceneElement
	{
	public:
		SceneElement();
		SceneElement(GeometryInfo& obj);

		void							Render(SceneInfo *sceneInfo);
		GeometryInfo&					GetGeometryInfo() { return actor; }

		bool							CanCastShadow();

		operator GeometryInfo*();

	protected:
		GeometryInfo		actor;
	};

	struct ActorGroupInfo
	{
		size_t								m_resource;
		RenderableGeometryInfo*				m_element;
		size_t								m_materialIndex;
	};

	//Represents a Viewport that we render to
	class GODZ_API ViewportInfo
	{
	public:
		ViewportInfo();
		void						Init(GODZHWND window);

		ViewportID					GetID() const { return m_id; }
		GODZHWND						GetWindow() const { return m_window; }
		const WMatrix&				GetProjection() const { return m_projMatrix; }
		ProjectionMatrixInfo&		GetProjectionInfo() { return m_projMatInfo; }
		void						SetProjectionMatrix(const WMatrix& projMat);


		/*
		//adds an element to the scene for this viewport
		void						AddSceneElement(GeometryInfo& element);

		//returns number of materials we have overalll in this scene
		size_t						GetNumMaterialGroups();

		//Every material group references all the ModelResources that uses it's material
		size_t						GetNumResourcesForGroup(size_t mat_group_index);

		//returns the resource index for the actor stored at the index for that group
		size_t						GetResource(size_t mat_group_index, size_t index);

		//returns the scene element index associated with the elementID
		size_t						GetElementForGroup(size_t mat_group_index, size_t elementIndex);	

		size_t						GetMatForGroupAt(size_t mat_group_index, size_t elementIndex);	

		//Returns the scene element
		GeometryInfo&				GetSceneElement(size_t index);
		size_t						GetNumElements() { return m_sceneElements.size(); }
		void						ClearSceneElements();
		MaterialInfo&				GetMatGroup(size_t index);
		*/


		void						SetVisibilityFlags(bool bVis) { m_bUseVisibilityFlags = bVis;}
		bool						IsVisibilityFlagsSet() { return m_bUseVisibilityFlags; }

		void						SetActive(bool isActive) { mIsActive = isActive; }
		bool						IsActive() { return mIsActive; }



		//Should check into this HWND pointer at somepoint... don't think renderer is even using this but not sure....
		GODZHWND						m_window;

		size_t						m_id;
		ProjectionMatrixInfo		m_projMatInfo;		//projection matrix (used by render device)
		WMatrix						m_projMatrix;		//projection matrix used by ViewFrustum
		ObjectID					m_camera;			//active camera in the scene
		LevelID						m_level;
		bool						m_bUseVisibilityFlags;		//set to true if resources have been culled this frame

		//std::vector<MaterialGroup>	m_materialGroups;
		//std::vector<GeometryInfo>	m_sceneElements;
		bool mIsActive;

		static size_t m_viewId;
		int m_width;
		int m_height;
	};


	class GODZ_API CameraList
	{
	public:
		void					AddCameraInfo(CameraInfo& info);
		CameraInfo&				GetCameraInfo(size_t index);

		//returns the main scene camera
		CameraInfo*				GetMainCameraInfo();

		size_t					GetNumCameras();
		void					ClearQueue();
		void					Merge(CameraList& queue);

	protected:
		std::vector<CameraInfo> m_CameraList;
	};

	class GODZ_API LightArray
	{
	public:
		void					AddSceneLight(const SceneLight& light);
		SceneLight&				GetSceneLight(size_t index);
		SceneLight*				GetSunLight();
		SceneLight*				FindSceneLight(ObjectID id);
		size_t					GetNumLights() const;
		void					ClearQueue();
		ObjectID				GetId() { return m_level; }
		void					SetId(ObjectID id) { m_level = id; }

	protected:
		std::vector<SceneLight> m_sceneLights;
		ObjectID				m_level;			//object id of the Level
	};

	class GODZ_API Viewports
	{
	public:
		void						AddViewport(const ViewportInfo& viewport);
		ViewportInfo&				GetViewportInfo(size_t index);
		ViewportInfo*				GetWindow(GODZHWND window);
		size_t						GetNumViewports() { return m_viewports.size(); }
		void						RemoveViewport(GODZHWND window);
		void						Clear() { m_viewports.clear(); }

	protected:
		std::vector<ViewportInfo>	m_viewports;		//list of viewports

	};

	class GODZ_API RenderList
	{
	public:
		
		void						SetViewports(Viewports& viewports);
		void						AddLightArray(LightArray& pArray);
		Viewports&					GetViewports() { return m_viewports; }
		CameraList&					GetCameraList();
		LightArray&					GetLightArray();
		void						Clear();



	protected:
		LightArray&					AddLightArray(ObjectID id);


		Viewports					m_viewports;
		CameraList					m_CameraList;		//All Cameras from all the scenes are merged directly into this list
		std::vector<LightArray>		m_lightArray;		//there is one array of light sources per level		

	};
}

#endif //__RENDERLIST__H__
