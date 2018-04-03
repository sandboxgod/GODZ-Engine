
#include "RenderList.h"
#include "MeshInstance.h"
#include <Windows.h>



namespace GODZ
{

void CameraList::ClearQueue() 
{ 
	m_CameraList.clear(); 
}

void CameraList::Merge(CameraList& queue)
{
	size_t num = queue.GetNumCameras();
	for(size_t i=0;i<num;i++)
	{
		m_CameraList.push_back(queue.GetCameraInfo(i));
	}
}

CameraInfo*	CameraList::GetMainCameraInfo()
{
	size_t num = GetNumCameras();
	for(size_t i=0;i<num;i++)
	{
		if (m_CameraList[i].mType == CameraType_Main)
		{
			return &m_CameraList[i];
		}
	}

	return NULL;
}


void CameraList::AddCameraInfo(CameraInfo& info)
{
	m_CameraList.push_back(info);
}

CameraInfo&	CameraList::GetCameraInfo(size_t index)
{
	godzassert(index < m_CameraList.size());
	return m_CameraList[index];
}


size_t CameraList::GetNumCameras()
{
	return m_CameraList.size();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void LightArray::AddSceneLight(const SceneLight& light)
{
	m_sceneLights.push_back(light);
}

SceneLight&	LightArray::GetSceneLight(size_t index)
{
	godzassert(index < m_sceneLights.size());
	return m_sceneLights[index];
}

SceneLight*	LightArray::GetSunLight()
{
	std::vector<SceneLight>::iterator iter;
	for(iter = m_sceneLights.begin(); iter != m_sceneLights.end(); ++iter)
	{
		SceneLight& light = (*iter);
		if (light.m_lightType == LT_SunLight)
		{
			return &light;
		}
	}

	return NULL;
}

SceneLight*	LightArray::FindSceneLight(ObjectID id)
{
	std::vector<SceneLight>::iterator iter;
	for(iter = m_sceneLights.begin(); iter != m_sceneLights.end(); ++iter)
	{
		SceneLight& light = (*iter);
		if (light.m_id == id)
		{
			return &light;
		}
	}

	return NULL;
}

size_t	LightArray::GetNumLights() const
{
	return m_sceneLights.size();
}

void LightArray::ClearQueue()
{
	m_sceneLights.clear();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t ViewportInfo::m_viewId = 0;

ViewportInfo::ViewportInfo()
: m_window(0)
, m_camera(0)
, m_level(0)
, m_bUseVisibilityFlags(false)
, mIsActive(true)	//viewports are active by default
, m_projMatrix(1)
{
	m_id = m_viewId;
	m_viewId++;
}

void ViewportInfo::Init(GODZHWND window)
{
	m_window = window;
	tagRECT lpRect;

	HWND hwnd = (HWND)window;
	BOOL hr =  GetWindowRect(hwnd,&lpRect);
	godzassert(hr == TRUE);
	m_width = lpRect.right - lpRect.left;
	m_height = lpRect.bottom - lpRect.top;

	m_projMatInfo.aspectRatio = float(m_width) / float(m_height);
	m_projMatInfo.far_plane = FARPLANE; ///so that we know this projection is not initialized
	m_projMatInfo.near_plane = NEARPLANE;
	m_projMatInfo.fov = DEG2RAD(45);
	ComputePerspectiveFovLH(m_projMatInfo,m_projMatrix);
	m_camera = INVALID_ID;
	m_level = INVALID_ID;
	m_bUseVisibilityFlags = false;
}

void ViewportInfo::SetProjectionMatrix(const WMatrix& projMat)
{
	m_projMatrix = projMat;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////


void Viewports::AddViewport(const ViewportInfo& viewport)
{
	m_viewports.push_back(viewport);
}

void Viewports::RemoveViewport(GODZHWND window)
{
	std::vector<ViewportInfo>::iterator iter;
	for(iter=m_viewports.begin();iter != m_viewports.end(); ++iter)
	{
		ViewportInfo& vinfo = (*iter);
		if (vinfo.GetWindow() == window)
		{
			m_viewports.erase(iter);
			break;
		}
	}
}

ViewportInfo& Viewports::GetViewportInfo(size_t index)
{
	return m_viewports[index];
}

ViewportInfo* Viewports::GetWindow(GODZHWND window)
{
	std::vector<ViewportInfo>::iterator iter;
	for(iter=m_viewports.begin();iter != m_viewports.end(); ++iter)
	{
		ViewportInfo& vinfo = (*iter);
		if (vinfo.GetWindow() == window)
		{
			return &vinfo;
		}
	}

	return NULL;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderList::Clear()
{
	m_lightArray.clear();
	m_CameraList.ClearQueue();
	m_viewports.Clear();
}

CameraList& RenderList::GetCameraList() 
{ 
	return m_CameraList; 
}

LightArray& RenderList::AddLightArray(ObjectID id)
{
	LightArray pArray;
	pArray.SetId(id);
	m_lightArray.push_back(pArray);

	return m_lightArray[ m_lightArray.size() - 1];
}

void RenderList::AddLightArray(LightArray& pArray)
{
	m_lightArray.push_back(pArray);
}

/*void RenderList::AddGeometryList(GeometryList& plist)
{
	m_geomList.push_back(plist);
}*/

LightArray&	RenderList::GetLightArray()
{
	//TODO: hardcoded for 1 LEVEL
	size_t num = m_lightArray.size();
	for(size_t i=0;i<num;i++)
	{
		//if (m_lightArray[i].GetId() == pLevel)
		{
			return m_lightArray[i];
		}
	}

	//create new light array
	return AddLightArray(0);
}

/*
GeometryList& RenderList::GetGeometryList(ObjectID pLevel)
{
	std::vector<GeometryList>::iterator iter;
	for(iter = m_geomList.begin();iter != m_geomList.end(); ++iter)
	{
		GeometryList& geomArray = (*iter);

		if (geomArray.GetId() == pLevel)
		{
			return geomArray;
		}
	}

	GeometryList pArray;
	pArray.SetId(pLevel);
	m_geomList.push_back(pArray);

	return m_geomList[ m_geomList.size() - 1];
}*/

void RenderList::SetViewports(Viewports& viewports)
{
	m_viewports = viewports;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////


SceneElement::SceneElement()
{
}


SceneElement::SceneElement(GeometryInfo& obj)
: actor(obj)
{
}

void SceneElement::Render(SceneInfo *sceneInfo)
{
}

bool SceneElement::CanCastShadow()
{
	return actor.mIsShadowCaster;
}


SceneElement::operator GeometryInfo*()
{
	return &actor;
}



} //namespace GODZ