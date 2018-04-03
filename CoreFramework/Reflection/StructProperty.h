/* =================================================================

	===============================================================
*/

#pragma once

#include "PropertyConvertReference.h"
#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/GenericReferenceCounter.h>
#include <hash_map>

namespace GODZ
{
#define REGISTER_STRUCT_PROPERTY(c, p)					\
	class PrivateStructProperty##c##p : public StructProperty		\
	{															\
	public:														\
		PrivateStructProperty##c##p(const char* propertyName)		\
		: StructProperty(propertyName)			\
		{														\
		}														\
		void SetProperty(void* myStruct, GenericReference<PropertyValue> value)						\
		{														\
			SetValue( ((c*)myStruct)->##p, value );			\
		}														\
		GenericReference<PropertyValue> GetProperty(void* myStruct)									\
		{														\
			return GetValue( ((c*)myStruct)->##p );							\
		}														\
		EPropertyType GetType(void* myStruct)									\
		{														\
			return GetPropertyType( ((c*)myStruct)->##p );		\
		}														\
		virtual void Register()									\
		{														\
			g_registerStruct##c.AddProperty(this);		\
		}														\
	};															\
	static PrivateStructProperty##c##p g_registerProperty##c##p(""#p""); \

	class GODZ_API StructProperty
	{
	public:
		StructProperty(const char* name);
		
		//Registers all the pending properties
		static void RegisterProperties();

		virtual void Register() = 0;
		virtual void SetProperty(void* myStruct, GenericReference<PropertyValue> value) = 0;
		virtual GenericReference<PropertyValue> GetProperty(void* myStruct) = 0;
		virtual EPropertyType GetType(void* myStruct) = 0;

		const char* GetName() { return m_name.c_str(); }

	private:
		rstring m_name;
	};

}

