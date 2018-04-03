
#include "ResourceObject.h"
#include "crc32.h"
#include "SceneManager.h"
#include "SceneData.h"
#include "GenericPackage.h"

namespace GODZ
{



ResourceObject::ResourceObject(HashCodeID hash)
: m_Flag(RF_None)
, m_hashCode(hash)
, m_package(NULL)
{
	m_sFilename="";
}

ResourceObject::~ResourceObject()
{
	//Now that resources are ref counted we should no longer need to pass around these type
	//of events
	//RenderDeviceEvent* event = CreateDestroyEvent(m_renderDeviceObject);
	//SceneData::AddRenderDeviceEvent(event);
}

//Returns true if this resource is the requested type
bool ResourceObject::IsType(ResourceFlag type)
{
	return (m_Flag == type);
}

ResourceObject::ResourceFlag ResourceObject::GetType()
{
	return m_Flag;
}

void ResourceObject::SetType(ResourceFlag Flags)
{
	m_Flag = Flags;
}

Future<GenericReference<RenderDeviceObject> > ResourceObject::GetRenderDeviceObject()
{
	return m_renderDeviceObject;
}

void ResourceObject::Destroy()
{
}

const char* ResourceObject::GetName()
{
	return m_sFilename;
}

//note: called from serialize thread
void ResourceObject::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	ar << m_sFilename;
	ar << m_hashCode;
}

void ResourceObject::SetName(const char* filename)
{
	m_sFilename=filename;

	if (filename != NULL)
	{
		m_hashCode = ECRC32::Get_CRC(filename);
	}
	
}

void ResourceObject::SetPackage(GenericPackage* pkg) 
{
	m_package = pkg;
	m_package->AddResource(this);
}

} //namespace GODZ