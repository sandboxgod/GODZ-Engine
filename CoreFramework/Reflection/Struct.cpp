
#include "Struct.h"
#include "StructProperty.h"
#include "ClassProperty.h"


namespace GODZ
{
static const int MAX_NUM_STRUCTS = 64;
static unsigned int struct_count = 0;
Struct* g_structList[MAX_NUM_STRUCTS];

Struct::Struct(const char* name)
: m_hash(name)
, m_name(name)
{
	godzassert(struct_count < MAX_NUM_STRUCTS); //we have exceeded the struct limit!
	g_structList[struct_count] = this;
	struct_count++;
}

Struct* Struct::FindStruct(HString name)
{
	for (UInt32 i = 0; i < struct_count; i++)
	{
		if (g_structList[i]->m_hash == name)
		{
			return g_structList[i];
		}
	}

	return NULL;
}


void Struct::AddProperty(StructProperty* p)
{
	HString name(p->GetName());
	m_propertyList[name.getHashCode()] = p;
}

void Struct::GeyKeys(std::vector<HashCodeID>& keys)
{
	PropertyList::iterator iter;
	for (iter = m_propertyList.begin(); iter != m_propertyList.end(); iter++)
	{
		keys.push_back( iter->first );
	}
}

void Struct::GetValues(std::vector<StructProperty*>& values)
{
	PropertyList::iterator iter;
	for (iter = m_propertyList.begin(); iter != m_propertyList.end(); iter++)
	{
		values.push_back( iter->second );
	}
}

void Struct::GetPairs(std::vector<StructPropertyPair>& values)
{
	PropertyList::iterator iter;
	for (iter = m_propertyList.begin(); iter != m_propertyList.end(); iter++)
	{
		StructPropertyPair p;
		p.key = iter->first;
		p.value = iter->second;
		values.push_back( p );
	}
}

StructProperty* Struct::GetProperty(HashCodeID hash)
{
	PropertyList::iterator iter = m_propertyList.find(hash);
	if (iter != m_propertyList.end())
	{
		return iter->second;
	}

	return NULL;
}

}
