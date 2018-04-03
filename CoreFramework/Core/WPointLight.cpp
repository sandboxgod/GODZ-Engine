
#include "WPointLight.h"
#include "PackageList.h"
#include "GenericObjData.h"
#include "Mesh.h"
#include "LevelNode.h"
#include "SceneData.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

namespace GODZ
{

ImplementGenericFlags(WPointLight, ClassFlag_Placeable)

WPointLight::WPointLight()
{
	m_light.m_id = SceneData::CreateRuntimeID();
	m_light.m_visualID = m_id;

	m_light.m_lightType = LT_PointLight; //set lighting type
	m_light.m_lightRange = 1; //default lightrange
	m_bVisible = true;

	//default color is white...
	m_light.m_lightColor.r = 1.0f;
	m_light.m_lightColor.g = 0.7f;
	m_light.m_lightColor.b = 0.0f;
	m_light.m_lightColor.a = 1.0f;

	SetCanCollideWithWorld(false);
	SetIsShadowCaster(false);

	//to get pass debug....
	m_light.m_forward.MakeZero();
	m_light.m_lightDir.MakeZero();
	m_light.m_viewerPosition.MakeZero();
	m_light.m_viewMatrixInfo.eye.MakeZero();
	m_light.m_viewMatrixInfo.lookAt.MakeZero();
	m_light.m_viewMatrixInfo.up.MakeZero();
}

void WPointLight::UpdateGeometryInfo(GeometryInfo& info)
{
	info.m_type = GeometryBaseInfo::VisualType_ReferenceOnly;
}

void WPointLight::OnAddToWorld()
{
	WEntity::OnAddToWorld();

	if (mesh == NULL)
	{
		//load the point light mesh for culling...
		GenericPackage* package = GenericObjData::m_packageList.FindPackageByName("Lights");
		godzassert(package != NULL);

		if (package != NULL)
		{
			const unsigned int sphere01_hash = 3861174648; //"Sphere01"
			atomic_ptr<GenericObject> objPtr(package->FindObjectRef(sphere01_hash));
			atomic_ptr<Mesh> meshPtr( objPtr );
			SetMesh( meshPtr );
		}

		LightCreationEvent* ev = new LightCreationEvent(m_light);
		m_pLevel->AddObjectStateChangeEvent(ev);
	}
}

//invoked when this entity is removed from a layer
void WPointLight::OnRemoved()
{
	WEntity::OnRemoved();

	LightDestroyEvent* ev = new LightDestroyEvent(m_light.m_id);
	m_pLevel->AddObjectStateChangeEvent(ev);
}

void WPointLight::OnTick(float dt)
{
	//Note: point lights require uniform scale
	if (!Equals(m_light.m_lightRange, m_vScale.x))
	{
		m_light.m_lightRange = m_vScale.x;

		//update info for this light
		LightCreationEvent* ev = new LightCreationEvent(m_light);
		m_pLevel->AddObjectStateChangeEvent(ev);
	}

	//this should be last, after we set all of our desired properties we want to send....
	WEntity::OnTick(dt);
}


const Color4f& WPointLight::GetColor() const
{
	return m_light.m_lightColor;
}

void WPointLight::SetColor(const Color4f& color)
{
	m_light.m_lightColor = color;
}

}