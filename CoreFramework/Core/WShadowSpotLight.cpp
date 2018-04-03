
#include "WShadowSpotLight.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>



namespace GODZ
{
ImplementGenericFlags(WShadowSpotLight, ClassFlag_Placeable)

REGISTER_FLOAT(WShadowSpotLight, Fov)
REGISTER_FLOAT(WShadowSpotLight, AspectRatio)
REGISTER_FLOAT(WShadowSpotLight, Near)


/////////////////////////////////////////////////////////////////////////////////////

WShadowSpotLight::WShadowSpotLight()
: WSpotLight()
{
	m_light.m_lightType = LT_ShadowSpotLight;

	m_light.mProj.aspectRatio = 1.0f;
	m_light.mProj.fov = DEG2RAD(90);
	m_light.mProj.near_plane = 0.0f;
	m_light.mProj.far_plane = 1000.0f;
}

float WShadowSpotLight::getLength()
{
	return m_light.mProj.far_plane - m_light.mProj.near_plane;
}

}