
#include "StructProperty.h"

namespace GODZ
{
static const int MAX_NUM_STRUCT_PROPERTIES = 64;
static unsigned int struct_property_count = 0;
StructProperty* g_structPropertyList[MAX_NUM_STRUCT_PROPERTIES];

StructProperty::StructProperty(const char* name)
: m_name(name)
{
	godzassert(struct_property_count < MAX_NUM_STRUCT_PROPERTIES);
	g_structPropertyList[struct_property_count] = this;
	struct_property_count++;
}

void StructProperty::RegisterProperties()
{
	for (UInt32 i = 0; i < struct_property_count; i++)
	{
		g_structPropertyList[i]->Register();
	}
}

}
