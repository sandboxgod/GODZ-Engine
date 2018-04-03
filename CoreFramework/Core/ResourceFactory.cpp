
#include "ResourceFactory.h"
#include "ModelResource.h"
#include "TextureResource.h"


namespace GODZ
{


GenericResource ResourceFactory::GetNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* filename)
{
	//NOTE: This function is thread safe 
	return CreateNewResource(ResourceType, hash, filename);
}

GenericResource ResourceFactory::CreateNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* filename, bool makeClone)
{
	//NOTE: This function is thread safe but never add unprotected access to resource list...


	//Pretty much all we do here is just create a core representation of the resource here.
	//The resource will then need to secure a render device object ID for itself
	switch(ResourceType)
	{
	default:
		{
		}
		break;
	case ResourceObject::RF_Model:
	case ResourceObject::RF_AnimatedModel:
	case ResourceObject::RF_Skin:
		{
			EVertexType vtype = VT_PerPixel;

			if (ResourceType == ResourceObject::RF_Skin)
			{
				vtype = VT_BlendVertex;
			}

			ResourceObject* resource = new ModelResource(hash, vtype);

			if (filename != NULL)
			{
				resource->SetName(filename);
			}
			

			resource->SetType(ResourceType);


			return resource;
		}
		break;		

	case ResourceObject::RF_Shader:
		{
		}
		break;

	case ResourceObject::RF_Texture:
		{
			return CreateTexture(hash, filename);
		}
		break;
	}

	return NULL;
}

TextureResource* ResourceFactory::CreateTexture(HashCodeID hash, const char* filename)
{
	TextureResource* resource = new TextureResource(hash);
	resource->SetName(filename);

	return resource;
}


} //namespace GODZ
