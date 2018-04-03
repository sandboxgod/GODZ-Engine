/* ===========================================================
	PropertyStruct

	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include "PropertyValue.h"


namespace GODZ
{
	class PropertyFactory;
	class Struct;

	class GODZ_API PropertyStruct : public PropertyValue
	{
	public:
		friend class PropertyFactory;

		PropertyStruct(Struct* value, void* structData, size_t sizeOf);
		~PropertyStruct();

		virtual void updateStruct(void* structValue);

		virtual void save(IWriter* writer);
		virtual void load(IReader* reader);
		virtual GenericReference<PropertyValue> clone();

	protected:
		PropertyStruct()
			: PropertyValue(PT_STRUCT)
			, m_value(NULL)
			, m_type(NULL)
			, m_sizeOf(0)
		{
		}

		//Structs are created on the stack when the application starts. So no need
		//to ref them or anything...
		Struct* m_type;
		UInt8* m_value;
		size_t m_sizeOf;
	};

	/*
	* Every new struct type thats added should declare a specialization of this template
	* to return their new type
	*/
	template <class T>
	class GetStructType
	{
	};

// Macro should be used in the .cpp file to declare a struct specialization
#define REGISTER_STRUCT_TYPE(c)							\
	template <class T> class GetStructType				\
	{				\
		operator Struct* { return g_registerStruct##c; }			\
	};				\

	template <class T>
	class ConvertToProperty
	{
	public:
		ConvertToProperty(T val)
		{
			m_value = new PropertyStruct( GetStructType<T>() );
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <class T>
	class ConvertFromProperty
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			//converts the PropertyStruct into the raw struct
			val->updateStruct(&m_value);
		}

		operator T()
		{
			return m_value;
		}

	private:
		T m_value;
	};

}
