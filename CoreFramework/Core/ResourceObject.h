/* ===========================================================
	Resource metaclass

	Created Jan 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_RESOURCE_H_)
#define _RESOURCE_H_

#include "CoreFramework.h"
#include "GenericReferenceCounter.h"
#include "GodzAtomics.h"
#include "GDZArchive.h"

namespace GODZ
{
	class GenericPackage;
	class RenderDeviceObject;

	class GODZ_API ResourceObject : public GenericReferenceCounter
	{
	public:
		//Resource Types
		enum ResourceFlag
		{
			RF_None,
			RF_Texture,		//texture resource
			RF_Model,		//model resource
			RF_Reserved,	//no longer used but kept around for legacy; can be replaced with a new type
			RF_Shader,		//shader
			RF_AnimatedModel,
			RF_Skin,		//skeletal mesh
		};

		ResourceObject(HashCodeID hash);
		virtual ~ResourceObject();

		//Notification that this resource is being destroyed. Note, this
		//is not called during engine shutdown so references to the video
		//render interface, etc should still be valid.
		virtual void Destroy();

		void SetPackage(GenericPackage* pkg);
		GenericPackage* GetPackage() { return m_package; }

		//Serializes this object to the writer interface
		virtual void Serialize(GDZArchive& ar);

		HashCodeID GetHashCode() { return m_hashCode; }
		void SetCRC32(HashCodeID hash) { m_hashCode = hash; }
		
		void SetName(const char* filename);	 //Sets the filename of this resource.
		const char* GetName(); //Returns the name of this resource

		// Sets the flags for this resource (see EResourceFlag).
		void SetType(ResourceFlag Flags);
		ResourceFlag GetType();
		bool IsType(ResourceFlag type);		//Returns true if this resource is the requested type

		Future<GenericReference<RenderDeviceObject> > GetRenderDeviceObject();


		//operators...........
		ResourceObject& operator=(const ResourceObject &rsc)
		{
			m_Flag = rsc.m_Flag;
			m_sFilename = rsc.m_sFilename;
			m_hashCode = rsc.m_hashCode;

			return *this;
		}

	protected:
		ResourceFlag	m_Flag;				//resource flags
		rstring			m_sFilename;
		HashCodeID		m_hashCode;
		GenericPackage* m_package;				//my package

		//Interface to the Render Device Object which is used by the Render pipeline exclusively
		Future<GenericReference<RenderDeviceObject> >	m_renderDeviceObject;
	};

	//Shared Resource Pointer (allows reference counting)
	typedef GenericReference<ResourceObject> GenericResource;
}

#endif