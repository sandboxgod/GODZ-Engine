/* ===========================================================

	========================================================== 
*/

#include "PropertyStruct.h"
#include "Struct.h"
#include "StructData.h"
#include "StructProperty.h"
#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/IWriter.h>
#include <CoreFramework/Core/IReader.h>

namespace GODZ
{
PropertyStruct::PropertyStruct(Struct* value, void* structData, size_t sizeOf)
: m_type(value)
, m_sizeOf(sizeOf)
, PropertyValue(PT_STRUCT)
{
	m_value = new UInt8[sizeOf];
	memcpy(m_value, structData, sizeOf);
}

PropertyStruct::~PropertyStruct()
{
	delete[] m_value;
}

void PropertyStruct::save(IWriter* writer)
{
	//save the struct
	writer->WriteStruct(m_type, m_value, m_sizeOf);
}

void PropertyStruct::load(IReader* reader)
{
	//read in the struct
	StructData data;
	if (reader->ReadStruct(data))
	{
		m_sizeOf = data.m_size;
		m_value = new UInt8[m_sizeOf];
		memcpy(m_value, data.m_data, m_sizeOf);
		m_type = data.m_struct;
	}
}

GenericReference<PropertyValue> PropertyStruct::clone()
{
	return new PropertyStruct(m_type, m_value, m_sizeOf);
}

void PropertyStruct::updateStruct(void* structValue)
{
	memcpy(structValue, m_value, m_sizeOf);
}


}

