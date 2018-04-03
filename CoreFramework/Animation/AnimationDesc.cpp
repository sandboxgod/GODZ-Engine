
#include "AnimationDesc.h"
#include "AnimationTrigger.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{
ImplementGeneric(AnimationDesc)
REGISTER_HASHSTRING(AnimationDesc, AnimationName)
REGISTER_ARRAY(AnimationDesc, AnimTriggers, atomic_ptr<AnimationTrigger> )
}


