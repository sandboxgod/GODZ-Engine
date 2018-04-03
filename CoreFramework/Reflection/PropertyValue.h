/* ===========================================================
	PropertyValue

	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/ResourceObject.h>
#include <CoreFramework/Core/atomic_ref.h>

namespace GODZ
{
	class GenericObject;
	class IWriter;
	class IReader;
	class PropertyFactory;

	class GODZ_API PropertyValue : public GenericReferenceCounter
	{
	public:
		PropertyValue(EPropertyType type)
			: m_type(type)
		{
		}

		virtual ~PropertyValue()
		{
		}

		virtual int getInt() { return 0; }
		virtual void setInt(int value) {}

		virtual float getFloat() { return 0; }
		virtual void setFloat(float value) {}

		virtual ulong getULong() { return 0; }
		virtual void setULong(ulong value) {}

		virtual UInt32 getUInt32() { return 0; }
		virtual void setUInt32(UInt32 value) {}

		virtual atomic_ptr<GenericObject> getObject() { return atomic_ptr<GenericObject>(); }
		virtual void setObject(GenericObject* value) { }

		virtual ResourceObject* getResource() { return NULL; }
		virtual void setResource(ResourceObject* value) {}

		EPropertyType getType() { return m_type; }
		virtual void save(IWriter* writer) = 0;
		virtual void load(IReader* reader) = 0;
		virtual GenericReference<PropertyValue> clone() = 0;

		virtual void updateStruct(void* structValue) {}

	protected:
		EPropertyType m_type;
	};

	class GODZ_API PropertyInt : public PropertyValue
	{
	public:
		friend class PropertyFactory;

		PropertyInt(int value)
			: m_value(value)
			, PropertyValue(PT_INT)
		{
		}

		virtual int getInt() { return m_value; }
		virtual void setInt(int value) { m_value = value; }

		virtual void save(IWriter* writer);
		virtual void load(IReader* reader);
		virtual GenericReference<PropertyValue> clone();

	protected:
		PropertyInt()
			: PropertyValue(PT_INT)
		{
		}

		int m_value;
	};

	class GODZ_API PropertyUInt32 : public PropertyValue
	{
	public:
		friend class PropertyFactory;

		PropertyUInt32(UInt32 value)
			: m_value(value)
			, PropertyValue(PT_UINT32)
		{
		}

		virtual UInt32 getUInt32() { return m_value; }
		virtual void setUInt32(UInt32 value) { m_value = value; }

		virtual void save(IWriter* writer);
		virtual void load(IReader* reader);
		virtual GenericReference<PropertyValue> clone();

	protected:
		PropertyUInt32()
			: PropertyValue(PT_UINT32)
		{
		}

		UInt32 m_value;
	};

	class GODZ_API PropertyFloat : public PropertyValue
	{
	public:
		friend class PropertyFactory;

		PropertyFloat(float value)
			: m_value(value)
			, PropertyValue(PT_FLOAT)
		{
		}

		virtual float getFloat() { return m_value; }
		virtual void setFloat(float value) { m_value = value; }

		virtual void save(IWriter* writer);
		virtual void load(IReader* reader);
		virtual GenericReference<PropertyValue> clone();

	protected:
		PropertyFloat()
			: PropertyValue(PT_FLOAT)
		{
		}

		float m_value;
	};


	class GODZ_API PropertyObject : public PropertyValue
	{
	public:
		friend class PropertyFactory;

		PropertyObject(const atomic_ptr<GenericObject>& value);

		virtual atomic_ptr<GenericObject> getObject();
		virtual void setObject(GenericObject* value);

		virtual void save(IWriter* writer);
		virtual void load(IReader* reader);
		virtual GenericReference<PropertyValue> clone();
		atomic_ptr<GenericObject> getPtr() { return m_value; }

	protected:
		PropertyObject()
			: PropertyValue(PT_OBJECT)
		{
		}

		atomic_ptr<GenericObject> m_value;
	};

	class GODZ_API PropertyResource : public PropertyValue
	{
	public:
		friend class PropertyFactory;

		PropertyResource(ResourceObject* value);

		virtual ResourceObject* getResource();
		virtual void setResource(ResourceObject* value);

		virtual void save(IWriter* writer);
		virtual void load(IReader* reader);
		virtual GenericReference<PropertyValue> clone();

	protected:
		PropertyResource()
			: PropertyValue(PT_RESOURCE)
		{
		}

		GenericReference<ResourceObject> m_value;
	};


	/*
	// Override with the specific types to handle property conversion
	template <class T>
	class ConvertToProperty
	{
	};

	
	// Utility used to convert from a primitive to a PropertyValue
	template <class T>
	class ConvertFromProperty
	{
	};
	*/

}
