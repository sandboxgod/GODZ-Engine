
#include "TextureResource.h"
#include <io.h>
#include "RenderDeviceEvent.h"
#include "SceneManager.h"
#include "SceneData.h"

using namespace GODZ;

TextureResource::TextureResource(HashCodeID hash)
: m_pImageData(NULL)
, m_numBytes(0)
, ResourceObject(hash)
{
	m_Flag = ResourceObject::RF_Texture;
}

TextureResource::~TextureResource()
{
	if (m_pImageData != NULL)
	{
		delete[] m_pImageData;
		m_pImageData = NULL;
	}
}

void TextureResource::LoadTextureIntoMem(rstring filename)
{
	FILE* stream=NULL;

	fileOpen(&stream, filename, FileOpenMode_ReadBinary);
	if( stream == NULL )
	{
		Log("Cannot open file %s", filename);
		return;
	}

	int fd = _fileno(stream);
	m_numBytes = _filelength(fd); 

	if (m_pImageData != NULL)
	{
		delete[] m_pImageData;
		m_pImageData=NULL;
	}

	m_pImageData = new UInt8[m_numBytes];
	size_t readfromStream = fread(m_pImageData, sizeof(UInt8), m_numBytes, stream);
	fclose(stream);

}

void TextureResource::CreateRenderDeviceObject()
{
	if(m_numBytes > 0) //missing texture?
	{
		RenderDeviceEvent* event = CreateTextureEvent(m_renderDeviceObject, m_textureInfo, m_pImageData, m_numBytes, GetName());
		SceneData::AddRenderDeviceEvent(event);
	}
	else
	{
		Log("Error: Texture (%s) not loaded\n", m_sFilename.c_str());
	}
}

//note: called from serialize thread
void TextureResource::Serialize(GDZArchive& ar)
{
	ResourceObject::Serialize(ar);

	ArchiveVersion version(1,1);
	ar << version;

	//serialize image data
	ar << m_numBytes;

	if (!ar.IsSaving())
	{
		if (m_pImageData != NULL)
		{
			delete[] m_pImageData;
			m_pImageData=NULL;
		}

		m_pImageData = new UInt8[m_numBytes];
	}

	ar.Serialize(m_pImageData,sizeof(UInt8),m_numBytes);

	if (!ar.IsSaving())
	{
		CreateRenderDeviceObject();
	}
} 


Future<GODZHANDLE>& TextureResource::GetBitmap()
{
	//send request to render pipe to populate our Future reference to a HBITMAP if its not set
	if (!m_hbitmap.isReady())
	{
		RenderDeviceEvent* event = CreateHBITMAPEvent(m_renderDeviceObject, m_hbitmap);
		SceneData::AddRenderDeviceEvent(event);
	}

	return m_hbitmap;
}

