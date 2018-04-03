
#include "CharacterTemplate.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{
ImplementGeneric(CharacterTemplate)
REGISTER_FLOAT(CharacterTemplate, MaxHealth)

CharacterTemplate::CharacterTemplate()
: mMaxHealth(100)
{
}

}