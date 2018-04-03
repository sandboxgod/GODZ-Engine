/* ===========================================================
	RenderData.h

	Defines structures used for "Game" thread to communicate
	with "Render"

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__RENDERDATA__H__)
#define __RENDERDATA__H__

#include "RenderList.h"
#include "RenderQueue.h"
#include "SkyDome.h"
#include "atomic_ref.h"

namespace GODZ
{
	class GODZ_API RenderGeoInfoPtrList
	{
	public:
		struct GODZ_API RenderGeoData
		{
			atomic_ptr<RenderableGeometryInfo> m_renderInfo;
			std::vector<RenderDeviceBox> mList;
			UInt32 mInstanceCount;
		};

	public:
		RenderGeoInfoPtrList();
		void add(RenderGeoData& obj) { mRenderableObjs.push_back(obj); }
		void clear() { mRenderableObjs.clear(); }
		size_t size() { return mRenderableObjs.size(); }
		RenderGeoData& get(size_t index);
		void reserve(size_t size);
		RenderGeoData& operator[](size_t index);

	protected:
		std::vector<RenderGeoData> mRenderableObjs;
	};

	// Adds & Removes objects based on their VisualID
	class GODZ_API VisualObjectList : public RenderGeoInfoPtrList
	{
	public:
		void add(const GeometryInfo& info);
		RenderableGeometryInfo* getInfo(const VisualID& index);
		void erase(const VisualID& id, RenderQueue& queue);
	};


	enum LightResult
	{
		LightAdded,
		LightUpdated
	};

	struct GODZ_API RenderMaterialInfo
	{
		MaterialInfo matInfo;
		int mInstanceCount;
		bool mIsValid;
		std::vector<ActorGroupInfo>			m_actorGroups;

		RenderMaterialInfo()
			: mIsValid(false)
		{
		}

		void Clear()
		{
			mIsValid = false;
			matInfo.Clear();
			m_actorGroups.clear();
		}
	};

	/*
	* Stores render's view of the game world (current level viewports are being rendered from). Since entity IDs are guaranted to be unique
	* the renderer doesn't care which 'level' a particular belongs to.
	*/
	class GODZ_API RenderData
	{
	public:
		RenderData();
		~RenderData();

		void						AcquireRenderQueue();

		void						AddMaterialInfo(MaterialInfo& mat);
		void						UpdateMaterialInfo(MaterialInfo& mat);
		void						RemoveMaterial(const MaterialID& id);
		RenderMaterialInfo*			FindMaterial(const MaterialID& id);

		void						AddMaterialQuad(MaterialQuad& quad);
		size_t						GetNumMaterialQuads();
		MaterialQuad&				GetMaterialQuad(size_t index);
		void						ClearMaterialQuads();

		void						AddViewport(const ViewportInfo& viewport);
		ViewportInfo&				GetViewportInfo(size_t index);
		void						RemoveViewport(ViewportID index);
		size_t						GetNumViewports() { return m_viewports.size(); }
		void						SetViewport(GODZHWND window);
		ViewportInfo*				GetActiveViewport();

		LightResult					AddUpdateSceneLight(const SceneLight& light);
		size_t						GetNumLights(LightType type);
		SceneLight*					GetLightAt(LightType type, size_t index);
		void						RemoveLight(LightID id);
		SceneLight*					GetSunLight();
		void						SetActiveSun(LightID id);

		void						AddCam(CameraInfo& info);
		void						RemoveCam(CameraID id);
		void						SetActiveCam(CameraID id);
		CameraInfo*					GetMainCameraInfo();
		CameraInfo*					GetCameraInfo(CameraID id);
		size_t						GetNumCameras() { return m_cameras.size(); }


		void						CreateEntity(const GeometryInfo& info);
		void						UpdateEntity(const GeometryBaseInfo& id);
		void						RemoveEntity(const VisualID& id);
		RenderableGeometryInfo*		GetEntity(const VisualID& id);

		RenderGeoInfoPtrList::RenderGeoData&		GetRenderGeoData(size_t idex);

		void						SetupMaterialGroups(RenderableGeometryInfo& id);
		void						ClearMaterialGroups(VisualID id);
		void						ClearMaterialGroups(RenderableGeometryInfo& info);
		size_t						GetNumElements() { return m_actors.size(); }
		RenderableGeometryInfo*		GetEntityAt(size_t index);	//retrieves from array directly (random access)

		// 2010/06/09 (WSK) - Temporarily exposed this
		RenderGeoInfoPtrList&		GetRenderableList() { return m_actors; }

		size_t						GetNumResourcesForGroup(size_t mat_group_index);

		void						SetSunlightView(const WMatrix& view) { mSunlightView = view; }
		WMatrix&					GetSunlightView() { return mSunlightView; }
		ViewFrustum&				GetSunlightFrustum() { return mLightFrustum; }
		void						SetSunlightFrustum(ViewFrustum& vf) { mLightFrustum = vf; }

		void						SetViewBoxSize(float size) { m_viewBoxSize = size; }
		float						GetViewBoxSize() { return m_viewBoxSize; }

		void						SetSkyDome(SkyDomeInfo& info);
		void						RemoveSkyDome(SkyDomeInfo& info);
		SkyDomeInfo&				GetSkyDome();

	private:
		RenderQueue*					mQueue;
		std::vector<MaterialQuad>		m_materialQuads;
		std::vector<ViewportInfo>	m_viewports;		//list of viewports
		std::vector<atomic_ptr<RenderMaterialInfo> >	m_materials;		//list of materials

		std::vector<SceneLight>		m_sceneLights[LT_MAX];		//list of lights
		std::vector<CameraInfo>		m_cameras;			//list of cameras
		
		VisualObjectList			m_actors;			//list of visuals in the scene

		ViewportID mActiveView;
		CameraID mActiveCam;
		LightID mActiveSun;								//active sunlight -- primary sun to use

		WMatrix mSunlightView; //debug
		ViewFrustum mLightFrustum;

		float m_viewBoxSize;							//shadows, viewBox that contains the scene
		SkyDomeInfo					m_sky;
	};
}

#endif //__RENDERDATA__H__
