/* =================================================================

	===============================================================
*/

#pragma once

#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Core/GenericReferenceCounter.h>
#include <hash_map>

namespace GODZ
{
	class Struct;
	class StructProperty;

#define REGISTER_STRUCT(c)					\
	class PrivateStruct##c : public Struct		\
	{															\
	public:														\
		PrivateStruct##c(const char* Name)		\
		: Struct(Name)						\
		{														\
		}														\
	};															\
	static PrivateStruct##c g_registerStruct##c(""#c""); \

	struct GODZ_API StructPropertyPair
	{
		HashCodeID key;
		StructProperty* value;
	};

	// Represents a Struct...
	class GODZ_API Struct
	{
	public:
		Struct(const char* name);

		void AddProperty(StructProperty* value);
		void GeyKeys(std::vector<HashCodeID>& keys);
		void GetValues(std::vector<StructProperty*>& values);
		void GetPairs(std::vector<StructPropertyPair>& values);
		StructProperty* GetProperty(HashCodeID hash);
		HashCodeID GetHash() { return m_hash; }

		static Struct* FindStruct(HString name);

	protected:
		typedef stdext::hash_map<HashCodeID, StructProperty* > PropertyList;
		PropertyList				m_propertyList;

		HString m_hash;
		rstring m_name;
	};
}

