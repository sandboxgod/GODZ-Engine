
//shadowmap ------------ http://en.wikipedia.org/wiki/Shadow_mapping
//The first step renders the scene from the light's point of view. For a point light source, the view 
//should be a perspective projection as wide as its desired angle of effect (it will be a sort of 
//square spotlight). For directional light (e.g. that from the Sun), an orthographic projection should be used.

#include "WSunLight.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include "SceneData.h"
#include "SceneManager.h"

namespace GODZ
{

ImplementGenericFlags(WSunLight, ClassFlag_Placeable)

WSunLight::WSunLight()
: WDynamicLight()
{
	m_light.m_lightType = LT_SunLight;
}

void WSunLight::OnAddToWorld()
{
	WDynamicLight::OnAddToWorld();

	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetSunLight(m_light.m_id);
}


Vector3	WSunLight::GetTarget()
{
	return m_orientation.GetForward();
}

Vector3 WSunLight::GetEye()
{
	// Sunlight does not return position...
	return Vector3::Zero();
}

} //namespace