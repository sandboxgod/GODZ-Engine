
#include "IGenericConfig.h"
#include <CoreFramework/Reflection/GenericClass.h>

namespace GODZ
{

ImplementGeneric(IGenericConfig)

int ConfigSection::GetNumOfProperties()
{
	return (int)m_mapConfig.size();
}

Str_Pair ConfigSection::GetProperty(int index)
{
	int c=0;

	std::map<FName,FName>::iterator m1Iter;
	for(m1Iter=m_mapConfig.begin();m1Iter!=m_mapConfig.end();m1Iter++)
	{
		if (c==index)
		{
			return *m1Iter;
		}

		c++;
	}

	return *m1Iter;
}




} //namespace