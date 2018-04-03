
#include "GeometryInfo.h"
#include "SceneManager.h"
#include "IDriver.h"

namespace GODZ
{

void RenderDeviceContainer::add(RenderDeviceObjectInfo& info)
{
	mList.push_back(info);
}

RenderDeviceObjectInfo& RenderDeviceContainer::get(size_t index)
{
	godzassert(index < mList.size());
	return mList[index];
}

bool RenderDeviceContainer::getMaterial(RenderDeviceObject* obj, MaterialID& matID)
{
	size_t len = mList.size();
	for(size_t i=0; i<len; i++)
	{
		if (mList[i].mFuture.isReady() && mList[i].mFuture.getValue() == obj)
		{
			matID = mList[i].matID;
			return true;
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////

RenderableGeometryInfo::RenderableGeometryInfo()
: m_isSelected(false)
, mActiveAxis(GizmoAxis_None)
{
}

RenderableGeometryInfo::~RenderableGeometryInfo()
{
}

//render thread
void	RenderableGeometryInfo::Render(SceneInfo* sceneInfo, size_t matIndex)
{
	IDriver* pDriver = sceneInfo->m_pDriver;

	//set material index
	sceneInfo->m_geomInfo->m_info.m_nCurrentMaterial = matIndex;

	//get the render device object
	MaterialID matId = m_info.GetMaterial(matIndex);
	RenderDeviceContainer dc = m_info.mMap[matId.GetUInt64()];
	size_t num = dc.size();
	for(size_t i=0;i<num;i++)
	{
		RenderDeviceObjectInfo& info = dc.get(i);
		if (info.mIsVisible && info.mFuture.isReady())
		{
			pDriver->DrawModel(*sceneInfo, info.mFuture.getValue());
		}
	}
}

void RenderableGeometryInfo::Render(SceneInfo* sceneInfo, RenderDeviceObject* obj, size_t matIndex)
{
	IDriver* pDriver = sceneInfo->m_pDriver;

	sceneInfo->m_geomInfo = this;
	//set material index
	sceneInfo->m_geomInfo->m_info.m_nCurrentMaterial = matIndex;
	pDriver->DrawModel(*sceneInfo, obj);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
GeometryBaseInfo::GeometryBaseInfo()
: mIsShadowCaster(true)
, m_nCurrentMaterial(0)
, m_type(VisualType_Renderable)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

GeometryInfo::GeometryInfo()
	: m_transform(1)
{
}

void GeometryInfo::ClearRenderDeviceObjects()
{
	mMap.clear();
}

RenderDeviceContainer* GeometryInfo::GetContainer(const MaterialID& matID)
{
	RenderDeviceMap::iterator iter;
	iter = mMap.find(matID.GetUInt64());
	if (iter != mMap.end())
	{
		return &iter->second;
	}

	return NULL;
}

void GeometryInfo::AddMaterial(const MaterialID& mat)
{
	m_materials.push_back(mat);
}


MaterialID GeometryInfo::GetMaterial(size_t index)
{
	godzassert(index < m_materials.size());
	return m_materials[index];
}


size_t	GeometryInfo::GetNumMaterials() const
{
	return m_materials.size();
}

void GeometryInfo::ReserveMaterials(size_t num) 
{ 
	m_materials.reserve(num); 
}

void GeometryInfo::AddBone(const WMatrix16f& boneMat)
{
	m_boneTM.push_back(boneMat);
}

WMatrix16f&			GeometryInfo::GetBone(size_t index)
{
	godzassert(index < m_boneTM.size());
	return m_boneTM[index];
}
size_t				GeometryInfo::GetNumBones() const
{
	return m_boneTM.size();
}

void				GeometryInfo::ClearBones()
{
	m_boneTM.clear();
}

void GeometryInfo::ReserveBones(size_t numBones)
{
	m_boneTM.reserve(numBones);
}




} //namespace GODZ