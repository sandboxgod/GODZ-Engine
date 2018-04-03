
//shadowmap ------------ http://en.wikipedia.org/wiki/Shadow_mapping
//The first step renders the scene from the light's point of view. For a point light source, the view 
//should be a perspective projection as wide as its desired angle of effect (it will be a sort of 
//square spotlight). For directional light (e.g. that from the Sun), an orthographic projection should be used.

#include "WDynamicLight.h"
#include "Mesh.h"
#include "IDriver.h"
#include "PropertyNames.h"
#include "LevelNode.h"
#include "EntityEvents.h"
#include "SceneData.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Gameplay/WCharacter.h>
#include <CoreFramework/Gameplay/WPlayerController.h>

namespace GODZ
{

ImplementGeneric(WDynamicLight)
REGISTER_COLOR(WDynamicLight, Color)

WDynamicLight::WDynamicLight()
{
	m_bVisible=false;
	m_bMouseLook=false; //Turn MouseLook off, do not rotate the character with this light

	m_light.m_id = mCameraInfo.m_id;
	m_light.m_visualID = m_id;

	//default color is white...
	m_light.m_lightColor.r = 1.0f;
	m_light.m_lightColor.g = 0.7f;
	m_light.m_lightColor.b = 0.0f;
	m_light.m_lightColor.a = 1.0f;

	m_light.m_lightRange = 1; //scale = 1

	//default light settings
	m_orientation._11 = 0.99557745f;
	m_orientation._12 = 0.0;
	m_orientation._13 = 0.093945421f;
	//m_orientation._14 = 0;
	m_orientation._21 = -0.039819099f;
	m_orientation._22 = 0.90573084f;
	m_orientation._23 = 0.42197901f;

	m_orientation._31 = -0.08508922f;
	m_orientation._32 = -0.42385352f;
	m_orientation._33 = 0.90172499f;
}

void WDynamicLight::UpdateGeometryInfo(GeometryInfo& info)
{
	info.m_type = GeometryBaseInfo::VisualType_ReferenceOnly;
}

WDynamicLight::~WDynamicLight()
{

}

void WDynamicLight::OnAddToWorld()
{
	WCamera::OnAddToWorld();

	LightCreationEvent* ev = new LightCreationEvent(m_light);
	m_pLevel->AddObjectStateChangeEvent(ev);
}

//invoked when this entity is removed from a layer
void WDynamicLight::OnRemoved()
{
	WCamera::OnRemoved();

	LightDestroyEvent* ev = new LightDestroyEvent(m_light.m_id);
	m_pLevel->AddObjectStateChangeEvent(ev);
}

void WDynamicLight::OnTick(float dt)
{
	WCamera::OnTick(dt);

	//for now we push camera data every frame :(
	PushCameraInfo(GetEye(), GetTarget());

	//update view matrix
	m_light.m_viewMatrixInfo.eye = GetEye();
	m_light.m_viewMatrixInfo.up = UP_VECTOR;
	m_light.m_viewMatrixInfo.lookAt = GetTarget();

	// Get vector from eye to target (used for lighting by shaders)
	m_light.m_lightDir = m_light.m_viewMatrixInfo.lookAt - m_light.m_viewMatrixInfo.eye;
	m_light.m_lightDir.Normalize();

	m_light.m_forward = m_orientation.GetForward();

	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	if (viewTarget != NULL)
	{
		m_light.m_viewerPosition = viewTarget->GetLocation();
	}
	else
	{
		m_light.m_viewerPosition.MakeZero();
	}

	//update info for this light
	LightCreationEvent* ev = new LightCreationEvent(m_light);
	m_pLevel->AddObjectStateChangeEvent(ev);
}

const Color4f& WDynamicLight::GetColor() const
{
	return m_light.m_lightColor;
}

void WDynamicLight::SetColor(const Color4f& color)
{
	m_light.m_lightColor = color;
}

LightType	WDynamicLight::GetLightType()
{
	return m_light.m_lightType;
}

} //namespace