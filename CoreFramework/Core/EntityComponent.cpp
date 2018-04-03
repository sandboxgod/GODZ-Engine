
#include "EntityComponent.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{

ImplementAbstractClass(EntityComponent)

HashCodeID EntityComponent::GetComponentName()
{
	return GetRuntimeClass()->GetHashCode();
}

void EntityComponent::SetOwner(WEntity* owner)
{
	mOwner = owner;
}


}


