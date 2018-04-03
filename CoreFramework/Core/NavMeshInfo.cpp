
#include "NavMeshInfo.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{
ImplementGeneric(NavMeshInfo)

void NavMeshInfo::Serialize(GDZArchive& ar)
{
	m_navMesh.Serialize(ar);
}

}