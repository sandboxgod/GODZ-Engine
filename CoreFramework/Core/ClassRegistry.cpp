
#include "ClassRegistry.h"
#include "GenericObjData.h"
#include <CoreFramework/Reflection/GenericClass.h>

namespace GODZ
{

atomic_ptr<GenericClass> ClassRegistry::findClass(HashCodeID hash)
{
	size_t num = GenericObjData::GetNumClasses();
	for(size_t i=0;i<num;i++)
	{
		atomic_ptr<GenericClass> gc = GenericObjData::GetClass(i);
		if (gc->GetHashCode() == hash)
		{
			return gc;
		}
	}

	return atomic_ptr<GenericClass>();
}



}