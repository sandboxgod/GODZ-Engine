

#include "ShaderManager.h"
#include "ShaderResource.h"
#include "ResourceManager.h"
#include "crc32.h"
#include "GenericObjData.h"


using namespace GODZ;

ShaderManager::ShaderManager()
: GenericSingleton<ShaderManager>(*this)
, m_shaderPackage(NULL)
{
	//create a Base shaders package... 
	m_shaderPackage = new GenericPackage("BaseShaders", PF_DLL);
	GenericObjData::m_packageList.AddPackage(m_shaderPackage);
}

void ShaderManager::AddShaderDefinition(ShaderDefinition& pDef)
{
	pDef.m_pShader = LoadShader(&pDef);
	mShaders.push_back(pDef);
}

ShaderResource* ShaderManager::LoadShader(ShaderDefinition* pDef)
{
	HashCodeID hash = ECRC32::Get_CRC(pDef->filename);
	ShaderResource* resource = new ShaderResource(hash);
	mMap[hash] = resource;

	resource->SetName(pDef->filename);
	resource->CreateRenderDeviceObject();
	resource->SetPackage(m_shaderPackage);

	return resource;
}

ShaderResource* ShaderManager::FindShader(HString shaderName)
{
	return mMap[shaderName];
}

ShaderDefinition& ShaderManager::GetShaderDefinition(size_t index)
{
	return mShaders[index];
}

size_t ShaderManager::GetNumDefinitions()
{
	return mShaders.size();
}
