
#include "EntityTemplate.h"
#include <CoreFramework/Animation/AnimationDesc.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{
ImplementGeneric(EntityTemplate)
REGISTER_ARRAY(EntityTemplate, AnimationList, atomic_ptr<AnimationDesc> )
REGISTER_ARRAY(EntityTemplate, Components, atomic_ptr<EntityComponent> )

REGISTER_OBJECT(EntityTemplate, Model, Mesh) //TODO: Obsolete
}