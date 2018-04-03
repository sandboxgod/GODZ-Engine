
#include "RenderData.h"
#include "SceneData.h"

namespace GODZ
{

RenderGeoInfoPtrList::RenderGeoInfoPtrList()
{
	mRenderableObjs.reserve(150);
}

RenderGeoInfoPtrList::RenderGeoData& RenderGeoInfoPtrList::get(size_t index)
{
	godzassert(index < mRenderableObjs.size());
	return mRenderableObjs[index];
}

void RenderGeoInfoPtrList::reserve(size_t size)
{
	mRenderableObjs.reserve(size);
}

RenderGeoInfoPtrList::RenderGeoData& RenderGeoInfoPtrList::operator[](size_t index)
{
	godzassert(index < mRenderableObjs.size());
	return mRenderableObjs[index];
}


/////////////////////////////////////////////////////////////////////////////

void VisualObjectList::add(const GeometryInfo& info)
{
	// Put the visual at the location it wants to go....
	if (info.m_id.mSlotNumber + 1 > mRenderableObjs.size())
	{
		int slotsToAdd = (info.m_id.mSlotNumber - mRenderableObjs.size()) + 1;
		for (int i = 0; i < slotsToAdd; i++)
		{
			RenderGeoData geo;
			geo.mInstanceCount = 1;
			mRenderableObjs.push_back(geo);
		}
	}

	mRenderableObjs[info.m_id.mSlotNumber].mInstanceCount = info.m_id.mInstanceCount;
	atomic_ptr<RenderableGeometryInfo> renderGeoInfoPtr(new RenderableGeometryInfo);
	renderGeoInfoPtr->m_info = info;
	mRenderableObjs[info.m_id.mSlotNumber].m_renderInfo = renderGeoInfoPtr;
	mRenderableObjs[info.m_id.mSlotNumber].mList.clear();
}

void VisualObjectList::erase(const VisualID& id, RenderQueue& queue)
{
	if(id.mInstanceCount == mRenderableObjs[id.mSlotNumber].mInstanceCount)
	{
		// free the Render data
		mRenderableObjs[id.mSlotNumber].m_renderInfo = NULL;

		// report this slot as available....
		VisualID newID = id;
		newID.mInstanceCount++;
		queue.mVisualFreeSlots.AddFreeSlot(newID);
	}
}

RenderableGeometryInfo* VisualObjectList::getInfo(const VisualID& id)
{
	if(id.mInstanceCount == mRenderableObjs[id.mSlotNumber].mInstanceCount)
	{
		return mRenderableObjs[id.mSlotNumber].m_renderInfo;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

RenderData::RenderData()
: mActiveView(0)
, mActiveCam(0)
, mActiveSun(0)
, mSunlightView(1)
{
	m_sky.m_id = 0; //set to invalid id
	m_materials.reserve(200);
}

RenderData::~RenderData()
{
	m_materials.clear();
}

void RenderData::AcquireRenderQueue()
{
	mQueue = &SceneData::GetFreeSlotQueue();
}

void RenderData::AddMaterialInfo(MaterialInfo& mat)
{
	// Put the Material at the location it wants to go....
	if (mat.m_id.mSlotNumber + 1 > m_materials.size())
	{
		int slotsToAdd = (mat.m_id.mSlotNumber - m_materials.size()) + 1;
		for (int i = 0; i < slotsToAdd; i++)
		{
			//we need to dynamically allocate MaterialInfos so that when the vector resizes 
			//the data is not destroyed....
			atomic_ptr<RenderMaterialInfo> matObject(new RenderMaterialInfo);
			matObject->mInstanceCount = 1;
			m_materials.push_back(matObject);
		}
	}

	godzassert(mat.m_id.mSlotNumber < m_materials.size());
	m_materials[mat.m_id.mSlotNumber]->matInfo = mat;
	m_materials[mat.m_id.mSlotNumber]->mInstanceCount = mat.m_id.mInstanceCount;
	m_materials[mat.m_id.mSlotNumber]->mIsValid = true;
}

void RenderData::UpdateMaterialInfo(MaterialInfo& mat)
{
	if (mat.m_id.mInstanceCount == m_materials[mat.m_id.mSlotNumber]->mInstanceCount)
	{
		m_materials[mat.m_id.mSlotNumber]->matInfo = mat;
	}
}

void RenderData::RemoveMaterial(const MaterialID& id)
{
	if (id.mInstanceCount == m_materials[id.mSlotNumber]->mInstanceCount)
	{
		// deactivate this material
		m_materials[id.mSlotNumber]->mIsValid = false;

		// drop refs
		m_materials[id.mSlotNumber]->Clear();

		// report this slot as available....
		VisualID newID = id;
		newID.mInstanceCount++;
		mQueue->mMaterialFreeSlots.AddFreeSlot(newID);
	}
}


RenderMaterialInfo*	RenderData::FindMaterial(const MaterialID& id)
{
	if (id.mInstanceCount == m_materials[id.mSlotNumber]->mInstanceCount && m_materials[id.mSlotNumber]->mIsValid)
	{
		godzassert(id.mSlotNumber < m_materials.size());
		godzassert(m_materials[id.mSlotNumber]->mIsValid);
		return m_materials[id.mSlotNumber];
	}

	return NULL;
}



void	RenderData::AddMaterialQuad(MaterialQuad& quad)
{
	m_materialQuads.push_back(quad);
}

size_t	RenderData::GetNumMaterialQuads()
{
	return m_materialQuads.size();
}

MaterialQuad&	RenderData::GetMaterialQuad(size_t index)
{
	MaterialQuad& mat = m_materialQuads[index];
	return mat;
}

void RenderData::ClearMaterialQuads()
{
	m_materialQuads.clear();
}

LightResult RenderData::AddUpdateSceneLight(const SceneLight& light)
{
	size_t num = m_sceneLights[light.m_lightType].size();
	std::vector<SceneLight>& list = m_sceneLights[light.m_lightType];
	for(size_t i=0; i<num;i++)
	{
		if (light.m_id == list[i].m_id)
		{
			list[i] = light;
			return LightUpdated;
		}
	}

	list.push_back(light);
	return LightAdded;
}

size_t RenderData::GetNumLights(LightType type)
{
	return m_sceneLights[type].size();
}

SceneLight* RenderData::GetLightAt(LightType type, size_t index)
{
	return &m_sceneLights[type][index];
}

void RenderData::RemoveLight(LightID id)
{
	for(int i=0; i < LT_MAX; i++)
	{
		std::vector<SceneLight>& list = m_sceneLights[i];

		std::vector<SceneLight>::iterator iter;
		for(iter = list.begin(); iter != list.end(); iter++)
		{
			if (iter->m_id == id)
			{
				list.erase(iter);
				return;
			}
		}
	}


}

SceneLight*	RenderData::GetSunLight()
{
	std::vector<SceneLight>::iterator iter;
	for(iter = m_sceneLights[LT_SunLight].begin(); iter != m_sceneLights[LT_SunLight].end(); ++iter)
	{
		SceneLight& light = (*iter);

		if (mActiveSun == light.m_id)
		{
			return &light;
		}
	}

	return NULL;
}

void RenderData::SetActiveSun(LightID id)
{
	mActiveSun = id;
}

void RenderData::AddViewport(const ViewportInfo& viewport)
{
	m_viewports.push_back(viewport);

	//make it the active viewport
	mActiveView = viewport.GetID();
}

void RenderData::RemoveViewport(ViewportID index)
{
	std::vector<ViewportInfo>::iterator iter;
	for(iter=m_viewports.begin();iter != m_viewports.end(); ++iter)
	{
		ViewportInfo& vinfo = (*iter);
		if (vinfo.m_id == index)
		{
			m_viewports.erase(iter);

			if (mActiveView == index)
			{
				//This situation shouldn't occur because the main thread should have us switched
				//before this event...
				mActiveView = 0;
			}

			break;
		}
	}
}

void RenderData::SetViewport(GODZHWND window)
{
	std::vector<ViewportInfo>::iterator iter;
	for(iter=m_viewports.begin();iter != m_viewports.end(); ++iter)
	{
		ViewportInfo& vinfo = (*iter);
		if (vinfo.GetWindow() == window)
		{
			mActiveView = vinfo.GetID();
			break;
		}
	}
}

ViewportInfo* RenderData::GetActiveViewport()
{
	std::vector<ViewportInfo>::iterator iter;
	for(iter=m_viewports.begin();iter != m_viewports.end(); ++iter)
	{
		ViewportInfo& vinfo = (*iter);
		if (vinfo.GetID() == mActiveView)
		{
			return &vinfo;
		}
	}

	return NULL;
}


ViewportInfo& RenderData::GetViewportInfo(size_t index)
{
	return m_viewports[index];
}


void RenderData::AddCam(CameraInfo& info)
{
	std::vector<CameraInfo>::iterator iter;
	for(iter = m_cameras.begin(); iter != m_cameras.end(); iter++)
	{
		if (iter->m_id == info.m_id)
		{
			*iter = info;
			return;
		}
	}


	m_cameras.push_back(info);
}

void RenderData::SetActiveCam(CameraID id)
{
	mActiveCam = id;
}

void RenderData::RemoveCam(CameraID id)
{
	std::vector<CameraInfo>::iterator iter;
	for(iter = m_cameras.begin(); iter != m_cameras.end(); iter++)
	{
		if (iter->m_id == id)
		{
			m_cameras.erase(iter);

			if (mActiveCam == id)
			{
				mActiveCam = 0;
			}

			return;
		}
	}
}

CameraInfo*	RenderData::GetCameraInfo(CameraID id)
{
	std::vector<CameraInfo>::iterator iter;
	for(iter = m_cameras.begin(); iter != m_cameras.end(); iter++)
	{
		if (iter->m_id == id)
		{
			CameraInfo& c = *iter;
			return &c;
		}
	}

	return NULL;
}

CameraInfo*	RenderData::GetMainCameraInfo()
{
	std::vector<CameraInfo>::iterator iter;
	for(iter = m_cameras.begin(); iter != m_cameras.end(); iter++)
	{
		if (iter->m_id == mActiveCam)
		{
			CameraInfo& c = *iter;
			return &c;
		}
	}

	return NULL;
}

void RenderData::CreateEntity(const GeometryInfo& info)
{
	m_actors.add(info);
}

void RenderData::UpdateEntity(const GeometryBaseInfo& info)
{
	RenderableGeometryInfo* ptr = m_actors.getInfo(info.m_id);
	if (ptr != NULL)
	{
		//only update the internal geometry info, leave cached materials & bones, etc be...
		info.CopyInto(ptr->m_info);
	}
}

void RenderData::RemoveEntity(const VisualID& id)
{
	godzassert(mQueue != NULL);
	m_actors.erase(id, *mQueue);
}

RenderableGeometryInfo* RenderData::GetEntity(const VisualID& id)
{
	return m_actors.getInfo(id);
}


RenderableGeometryInfo* RenderData::GetEntityAt(size_t index)
{
	godzassert(index < m_actors.size());
	return m_actors[index].m_renderInfo;
}

RenderGeoInfoPtrList::RenderGeoData& RenderData::GetRenderGeoData(size_t index)
{
	godzassert(index < m_actors.size());
	return m_actors[index];
}

void RenderData::ClearMaterialGroups(VisualID id)
{
	RenderableGeometryInfo* info = GetEntity(id);

	if (info != NULL)
	{
		ClearMaterialGroups( *info );
	}
}

//make sure there are no matGroups out there that already point to us
void RenderData::ClearMaterialGroups(RenderableGeometryInfo& info)
{
	size_t num = info.m_info.GetNumMaterials();
	for(size_t i=0; i < num; i++)
	{
		MaterialID id = info.m_info.GetMaterial(i);
		RenderMaterialInfo* renderMaterial = FindMaterial(id);

		bool hasFound = true;

		//keep looping til we have removed all groups that belong to this actor
		while (hasFound)
		{
			hasFound = false;

			std::vector<ActorGroupInfo>::iterator iter;
			for(iter = renderMaterial->m_actorGroups.begin(); iter != renderMaterial->m_actorGroups.end(); iter++)
			{
				if ( iter->m_element->m_info.m_id == info.m_info.m_id)
				{
					renderMaterial->m_actorGroups.erase(iter);
					hasFound = true;
					break;
				}
			}
		}
	}
}

//set material_groups for this entity
void RenderData::SetupMaterialGroups(RenderableGeometryInfo& info)
{
	//For every Material the actor has, we will find the matching materialGroup and add an entry for ourselves.
	//This allows the renderer to render resources per material
	ActorGroupInfo actorGroup;
	actorGroup.m_element = &info;

	size_t num = info.m_info.GetNumMaterials();
	for(size_t i=0; i < num; i++)
	{
		actorGroup.m_resource = i;
		actorGroup.m_materialIndex = i;

		MaterialID matId = info.m_info.GetMaterial(i);
		RenderMaterialInfo* renderMaterial = FindMaterial(matId);

		renderMaterial->m_actorGroups.push_back(actorGroup);
	}
}


void		RenderData::SetSkyDome(SkyDomeInfo& info)
{
	m_sky = info;
}

SkyDomeInfo&	RenderData::GetSkyDome()
{
	return m_sky;
}

void	RenderData::RemoveSkyDome(SkyDomeInfo& info)
{
	if (m_sky.m_id == info.m_id)
	{
		m_sky.m_id = 0; //set to invalid id
	}
}

} //namespace GODZ