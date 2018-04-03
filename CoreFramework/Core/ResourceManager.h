/* ===========================================================
	Resource Management.

	Created Jan 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(RESOURCE_MANAGER_H)
#define RESOURCE_MANAGER_H

#include "GenericObject.h"
#include "GenericReferenceCounter.h"
#include "ModelResource.h"
#include "ShaderResource.h"
#include "ResourceFactory.h"



namespace GODZ
{	
	//Forward Declarations
	class TextureResource;
	class Shader;
	class Brush;
	class IMaterial;
	class SpriteResource;


	//Global, Thread-safe resource container
	class GODZ_API ResourceManager : public GenericObject
	{
		DeclareGeneric(ResourceManager, GenericObject)

	public:
		
		virtual ~ResourceManager();

		ResourceFactory&				GetResourceFactory() { return m_factory; }

		//Returns a pointer to the new mesh resource
		virtual MeshResource			CreateModelResource(std::vector<Vertex> &vertexBuffer,  std::vector<u16> &indexBuffer, std::vector<ulong> &attributeBuffer, HashCodeID hash);
		virtual MeshResource			CreateModelResource(std::vector<PerPixelVertex> &vertexBuffer,  std::vector<u16> &indexBuffer, HashCodeID hash);

		//Returns a ptr to the new animated resource
		// [in] vertexBuffer - vertices that composes this mesh
		// [in] indexBuffer - indicies
		// [in] hash CRC32 code for this resource
		virtual MeshResource			CreateDynamicModel(std::vector<Vertex> &vertexBuffer, std::vector<u16> &indexBuffer, std::vector<ulong> &attributeBuffer, HashCodeID hash);

		//Returns the resource manager
		static ResourceManager*			GetInstance();

		//Returns a new texture resource
		virtual GenericReference<TextureResource>		GetNewTextureResource(HashCodeID hash);
		GenericReference<TextureResource>				GetNewTextureResource(const char* text);

		//gets the relative path to a resource
		// [out] absolutePath - buffer that will store absolute path to the file
		// [in] filename - name of the file
		rstring							GetRelativePath(const char* filename, const char* folderName="TEXTURES");

		//Returns the desired GPU program.
		virtual ShaderRef				GetShaderProgram(GenericClass* shaderClass);

		//searches base shader package only
		virtual ShaderRef				FindShader(HString hash);


		
		GenericResource					GetNewResource(ResourceObject::ResourceFlag ResourceType, const char* text);
		GenericResource					GetNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* text = NULL);
		

		GenericResource					CreateNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* filename=NULL, bool makeClone = false);


		ResourceObject*					findResource(const char* text);

	private:		
		ResourceManager();

		static ResourceManager*				Instance;
		ResourceFactory						m_factory;
		HString								m_shaderPackage;
	};	


}

#endif