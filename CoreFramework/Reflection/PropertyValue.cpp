/* ===========================================================

	========================================================== 
*/

#include "PropertyValue.h"
#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/IWriter.h>
#include <CoreFramework/Core/IReader.h>

namespace GODZ
{
	void PropertyInt::save(IWriter* writer)
	{
		writer->WriteInt(m_value);
	}

	void PropertyInt::load(IReader* reader)
	{
		reader->ReadInt(m_value);
	}

	GenericReference<PropertyValue> PropertyInt::clone()
	{
		return new PropertyInt(m_value);
	}

	/////////////////////////////////////////////////////////////////////////////////

	void PropertyFloat::save(IWriter* writer)
	{
		writer->WriteFloat(m_value);
	}

	void PropertyFloat::load(IReader* reader)
	{
		reader->ReadFloat(m_value);
	}

	GenericReference<PropertyValue> PropertyFloat::clone()
	{
		return new PropertyFloat(m_value);
	}

	/////////////////////////////////////////////////////////////////////////////////

	void PropertyUInt32::save(IWriter* writer)
	{
		writer->WriteUInt32(m_value);
	}

	void PropertyUInt32::load(IReader* reader)
	{
		reader->ReadUInt32(m_value);
	}

	GenericReference<PropertyValue> PropertyUInt32::clone()
	{
		return new PropertyUInt32(m_value);
	}

	/////////////////////////////////////////////////////////////////////////////////

	PropertyObject::PropertyObject(const atomic_ptr<GenericObject>& value)
			: m_value(value)
			, PropertyValue(PT_OBJECT)
		{
		}

	atomic_ptr<GenericObject> PropertyObject::getObject() { return m_value; }
	void PropertyObject::setObject(GenericObject* value) { m_value = value; }

	void PropertyObject::save(IWriter* writer)
	{
		writer->WriteObject(m_value);
	}

	void PropertyObject::load(IReader* reader)
	{
		m_value = reader->ReadObject();
	}

	GenericReference<PropertyValue> PropertyObject::clone()
	{
		return new PropertyObject(m_value->GetClone());
	}

	/////////////////////////////////////////////////////////////////////////////////

	PropertyResource::PropertyResource(ResourceObject* value)
			: m_value(value)
			, PropertyValue(PT_RESOURCE)
		{
		}

	ResourceObject* PropertyResource::getResource() { return m_value; }
	void PropertyResource::setResource(ResourceObject* value) { m_value = value; }

	void PropertyResource::save(IWriter* writer)
	{
		writer->WriteResource(m_value);
	}

	void PropertyResource::load(IReader* reader)
	{
		m_value = reader->ReadResource();
	}

	GenericReference<PropertyValue> PropertyResource::clone()
	{
		//note: we don't clone resources....
		return new PropertyResource(m_value);
	}
}

