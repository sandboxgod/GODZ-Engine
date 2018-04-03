
#include "AnimationTrigger.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{
ImplementAbstractClass(AnimationTrigger)
REGISTER_INT(AnimationTrigger, TriggerFrame)

AnimationTrigger::AnimationTrigger()
: mTriggerFrame(0)
{
}

}


