
#include "ShaderResource.h"
#include "SceneManager.h"
#include "RenderDeviceEvent.h"
#include "SceneData.h"


using namespace GODZ;

ShaderInfo::ShaderInfo()
: m_pMat(NULL)
{
	for(int i=0;i<MAX_PARAMETERS;i++)
	{
		m_bUploadedParamThisFrame[i] = false;
	}
}

ShaderResource::ShaderResource(HashCodeID hash)
: ResourceObject(hash)
{
	m_bAlwaysUploadConstants=true;
	m_bUpdateConstants=true;
	m_Flag = ResourceObject::RF_Shader;
	m_bUploadOnce=true;
	m_bHasDrawRoutine=0;
}

void ShaderResource::CreateRenderDeviceObject()
{
	RenderDeviceEvent* event = CreateShaderEvent(m_renderDeviceObject, m_techniqueList, GetName());
	SceneData::AddRenderDeviceEvent(event);
}

size_t ShaderResource::GetNumTechniques()
{
	godzassert(m_techniqueList.isReady());
	return m_techniqueList.getValue().GetNumTechniques();
}


const char* ShaderResource::GetTechniqueName(size_t index)
{
	godzassert(m_techniqueList.isReady());
	godzassert(index < m_techniqueList.getValue().GetNumTechniques());
	return m_techniqueList.getValue().GetTechniqueName(index);
}

bool ShaderResource::SetTechnique(udword index)
{
	return false;
}

bool ShaderResource::HasDrawRoutine()
{
	return m_bHasDrawRoutine;
}

bool ShaderResource::IsAutoUploadConstants()
{
	return m_bAlwaysUploadConstants;
}


void ShaderResource::UploadConstants()
{
	m_bUpdateConstants=true;
}