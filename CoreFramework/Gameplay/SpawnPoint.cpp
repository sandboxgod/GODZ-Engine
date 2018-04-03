
#include "SpawnPoint.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Core/ClassRegistry.h>


namespace GODZ
{

ImplementGenericFlags(SpawnPoint, ClassFlag_Placeable)

REGISTER_HASHCODE(SpawnPoint, SpawnClass)

/////////////////////////////////////////////////////////////////////////////////////

SpawnPoint::SpawnPoint()
: mSpawnClass(NULL)
{
}

void SpawnPoint::SetSpawnClass(UInt32 className)
{
	//lookup this hash code in the Class Registry
	mSpawnClass = ClassRegistry::findClass(className);
}

UInt32 SpawnPoint::GetSpawnClass()
{
	if( mSpawnClass != NULL )
		return mSpawnClass->GetHashCode();

	return 0;
}


}