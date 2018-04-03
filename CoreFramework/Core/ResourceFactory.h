/* ===========================================================
	ResourceFactory

	John 15:15
	Jesus called his disciples "friends"

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__ResourceFactory_H__)
#define __ResourceFactory_H__

#include "Godz.h"
#include <hash_map>
#include "GodzAtomics.h"
#include "ResourceObject.h"



namespace GODZ
{
	/**
	*	All function calls are thread safe. Manages resources based on their CRC32 value
	*/
	class GODZ_API ResourceFactory : public IReferenceCountObserver
	{
	public:
		//Returns a pointer to the new resource
		GenericResource					GetNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* fileName=NULL);

		TextureResource*				CreateTexture(HashCodeID hash, const char* fileName);

		//Creates a new instance without checking to see if it exists first.
		GenericResource					CreateNewResource(ResourceObject::ResourceFlag ResourceType, HashCodeID hash, const char* filename=NULL, bool makeClone = false);
	};
}

#endif
