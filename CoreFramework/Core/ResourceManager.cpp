
#include "ResourceManager.h"
#include "ModelResource.h"
#include "TextureResource.h"
#include "Material.h"
#include "ShaderManager.h"
#include "GodzAtomics.h"
#include "MeshInstance.h"
#include "StringTokenizer.h"
#include "crc32.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include "GenericObjData.h"

using namespace std;


namespace GODZ
{

///////////////////////////////////////////////////////////////////////////////////////////////////////
ImplementGeneric(ResourceManager)

ResourceManager* ResourceManager::Instance = NULL;

ResourceManager::ResourceManager()
: m_shaderPackage("BaseShaders")
{
}

ResourceManager::~ResourceManager()
{
}


GenericResource ResourceManager::GetNewResource(ResourceObject::ResourceFlag ResourceType, const char* text)
{
	unsigned long hash = 0;
	if (text != NULL)
		hash = ECRC32::Get_CRC(text);


	return m_factory.GetNewResource(ResourceType, hash, text);
}

GenericResource ResourceManager::GetNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* text)
{
	return m_factory.GetNewResource(ResourceType, hash, text);
}

MeshResource ResourceManager::CreateDynamicModel(std::vector<Vertex> &vertexBuffer, std::vector<WORD> &indexBuffer, std::vector<DWORD> &attributeBuffer, HashCodeID hash)
{
	GenericResource ref = GetNewResource(ResourceObject::RF_Skin, hash);
	ResourceObject* resource = &(*ref);
	ModelResource* model = static_cast<ModelResource*>(resource);
	model->CreateModelResource(vertexBuffer,indexBuffer,attributeBuffer);
	return model;
}

MeshResource	ResourceManager::CreateModelResource(std::vector<Vertex>& vertexBuffer,  std::vector<WORD>& indexBuffer, std::vector<DWORD>& attributeBuffer, HashCodeID hash)
{
	GenericResource ref = GetNewResource(ResourceObject::RF_Model, hash);
	ResourceObject* resource = &(*ref);
	ModelResource* model = static_cast<ModelResource*>(resource);
	model->CreateModelResource(vertexBuffer,indexBuffer,attributeBuffer);
	return model;
}

MeshResource	ResourceManager::CreateModelResource(std::vector<PerPixelVertex>& vertexBuffer,  std::vector<WORD>& indexBuffer, HashCodeID hash)
{
	GenericResource ref = GetNewResource(ResourceObject::RF_Model, hash);
	ResourceObject* resource = &(*ref);
	ModelResource* model = static_cast<ModelResource*>(resource);
	model->CreateModelResourceInMemory(vertexBuffer, indexBuffer);
	return model;
}

ShaderRef ResourceManager::GetShaderProgram(GenericClass* shaderClass)
{
	return 0;
}


ResourceManager* ResourceManager::GetInstance()
{
	
	//TODO: Ask the driver to return the preferred resource manager
	if (Instance == NULL)
	{
		Instance = new ResourceManager();
	}
	

	return Instance;
}

rstring ResourceManager::GetRelativePath(const char* filename, const char* folderName)
{
	const int bufsize = 1024;
	char buf[bufsize];
	char *relativeFilename = &buf[0];

	StringCopy(relativeFilename, filename, bufsize);
	_strupr_s(relativeFilename, bufsize); //convert to uppercase
	int index=(int)StringBuffer::StaticFindSubstring(relativeFilename, folderName);
	if (index != -1)
	{
		relativeFilename += index; //get the relative path
	}

	//Build the absolute path to the texture indicated by the .X file....
	rstring absolutePath = GetBaseDirectory();
	absolutePath += "\\";

	if (index == -1)
	{
		absolutePath += "Textures\\";
	}

	absolutePath += relativeFilename;

	return absolutePath;
}

ShaderRef ResourceManager::FindShader(HString hash)
{
	GenericPackage* shaderPack = GenericObjData::m_packageList.FindPackageByName(m_shaderPackage);
	ShaderResource* shader = static_cast<ShaderResource*>(shaderPack->findResource(hash));
	return shader;
}


GenericReference<TextureResource> ResourceManager::GetNewTextureResource(HashCodeID hash)
{
	GenericResource ref = GetNewResource(ResourceObject::RF_Texture, hash);
	GenericReference<TextureResource> tex = (TextureResource*)ref.m_ref;
	return tex;
}

GenericReference<TextureResource>	ResourceManager::GetNewTextureResource(const char* text)
{
	unsigned long hash = ECRC32::Get_CRC(text);
	GenericResource ref = m_factory.GetNewResource(ResourceObject::RF_Texture, hash, text);
	GenericReference<TextureResource> tex = (TextureResource*)ref.m_ref;
	return tex;
}

GenericResource ResourceManager::CreateNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* filename, bool makeClone)
{
	return m_factory.CreateNewResource(ResourceType, hash, filename, makeClone);
}

//expects <package name>.<resource name> --- note: it's case sensitive
ResourceObject*	ResourceManager::findResource(const char* text)
{
	StringTokenizer tk(text, ".");
	if (tk.size() != 2)
	{
		return NULL;
	}

	HString packageName(tk[0]);
	GenericPackage* p = GenericObjData::m_packageList.FindPackageByName(packageName);
	if (p != NULL)
	{
		HString resourceName(tk[1]);
		return p->findResource(resourceName);
	}

	return NULL;
}


} //namespace GODZ
