
#include "GDZArchive.h"
#include "IWriter.h"
#include "IReader.h"
#include "GenericObject.h"
#include "TextureResource.h"
#include "ResourceManager.h"

using namespace GODZ;

GDZArchive::GDZArchive(IReader *reader)
: m_pReader(reader), m_pWriter(0), m_bIsSaving(0)
{
	m_nFileVersion=GODZ_SAVE_VERSION;
}

GDZArchive::GDZArchive(IWriter *writer)
: m_pReader(NULL)
, m_pWriter(writer)
, m_bIsSaving(true)
, m_nFileVersion(GODZ_SAVE_VERSION)
{
}

void GDZArchive::Close()
{
	if (m_bIsSaving)
	{
		m_pWriter->Close();
	}
	else
	{
		m_pReader->Close();
	}
}

bool GDZArchive::IsSaving()
{
	return m_bIsSaving;
}

int GDZArchive::GetFileVersion()
{
	return m_nFileVersion;
}

size_t GDZArchive::Serialize(void* ptr, size_t dataTypeSize, size_t numElements)
{
	if (m_bIsSaving)
	{
		return m_pWriter->WriteData(ptr,dataTypeSize,numElements);
	}
	else
	{
		return m_pReader->ReadData(ptr,dataTypeSize,numElements);
	}
}

GDZArchive& GDZArchive::operator<<(long &value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteLong(value);
	}
	else
	{
		m_pReader->ReadLong(value);
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(int &i)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteInt(i);
	}
	else
	{
		m_pReader->ReadInt(i);
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(float &f)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteFloat(f);
	}
	else
	{
		m_pReader->ReadFloat(f);
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(udword &value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteUDWORD(value);
	}
	else
	{
		m_pReader->ReadUDWORD(value);
	}

	return *this;
}


GDZArchive& GDZArchive::operator<<(u16 &value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteU16(value);
	}
	else
	{
		m_pReader->ReadU16(value);
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(bool &value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteBool(value);
	}
	else
	{
		m_pReader->ReadBool(value);
	}

	return *this;
}


GDZArchive& GDZArchive::operator<<(rstring& value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteString(value);
	}
	else
	{
		value = m_pReader->ReadString();
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(GUID& value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteGUID(value);
	}
	else
	{
		m_pReader->ReadGUID(value);
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(unsigned long &value)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteULONG(value);
	}
	else
	{
		m_pReader->ReadULONG(value);
	}

	return *this;
}


GDZArchive& GDZArchive::operator<<(u8& ch)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteU8(ch);
	}
	else
	{
		m_pReader->ReadU8(ch);
	}

	return *this;
}


GDZArchive& GDZArchive::operator<<(GenericObject*& generic)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteObject(generic);
	}
	else
	{
		GenericObject* gs = m_pReader->ReadObject();
		generic = *(GenericObject**)&gs;
	}

	return *this;
}

GDZArchive& GDZArchive::operator<<(ResourceObject*& resource)
{
	if (m_bIsSaving)
	{
		m_pWriter->WriteResource(resource);
	}
	else
	{
		ResourceObject* resourceObj = m_pReader->ReadResource();
		resource = *(ResourceObject**)&resourceObj;
	}

	return *this;
}