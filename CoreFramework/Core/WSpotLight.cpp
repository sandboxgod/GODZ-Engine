
#include "WSpotLight.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include "Material.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "PackageList.h"
#include "GenericObjData.h"
#include "RenderDeviceEvent.h"
#include "SceneData.h"
#include "EntityEvents.h"
#include "LevelNode.h"

namespace GODZ
{

ImplementGenericFlags(WSpotLight, ClassFlag_Placeable)
REGISTER_FLOAT(WSpotLight, LightRange)
REGISTER_FLOAT(WSpotLight, Far)
REGISTER_FLOAT(WSpotLight, ConeRadius)
REGISTER_BOOL(WSpotLight, ZAxisFlip)

WSpotLight::WSpotLight()
: WDynamicLight()
, mConeRadius(700)
, mZAxisFlip(false)
{
	m_light.m_lightType = LT_SpotLight;

	m_light.m_lightRange = 320;

	//create the material
	mMaterial = new Material();
}


void WSpotLight::OnAddToWorld()
{
	WDynamicLight::OnAddToWorld();

	
	//TODO: fire off request for a UNIQUE cylinder mesh
	mModelResource = new ModelResource(0); //use invalid hash

	float length = getLength();
	//m_light.m_lightRange = length;

	Vector3 pos = GetEye();

	//giving it a placeholder boundary
	WBox box(Vector3(-10,-10,-10),Vector3(length,length,length));
	mModelResource->SetBounds(box);

	float coneradius1 = 0;
	float coneradius2 = mConeRadius;

	if (mZAxisFlip)
	{
		coneradius1 = mConeRadius;
		coneradius2 = 0;
	}

	//The created cylinder (Cone) is centered at the origin, and its axis is aligned with the z-axis.
	RenderDeviceEvent* event = CreateCylinder(mModelResource->GetRenderDeviceObject(), coneradius1, coneradius2, length, 24, 5);
	SceneData::AddRenderDeviceEvent(event);

	UpdateWorldScaleOffset();

}


Vector3	WSpotLight::GetTarget()
{
	//return m_targetPosition;
	return WDynamicLight::GetTarget();
}

Vector3 WSpotLight::GetEye()
{
	return m_pos;
}

float WSpotLight::getLength()
{
	return m_light.mProj.far_plane;
}

void WSpotLight::OnTick(float dt)
{
	if (mesh == NULL && mModelResource->GetRenderDeviceObject().isReady())
	{
		//Log("SpotLight initialized!\n");

		//create new mesh
		atomic_ptr<Mesh> newMeshPtr( new Mesh() );

		//cache off default instance
		atomic_ptr<MeshInstance> meshPtr( new MeshInstance() );
		meshPtr->AddMaterial(mMaterial);
		newMeshPtr->SetDefaultInstance(meshPtr);
		
		meshPtr->AddModelResource(mMaterial, mModelResource);
		newMeshPtr->ComputeBounds();

		//finally, we are ready to set our new mesh
		SetMesh(newMeshPtr);
	}

	//this should be last, after we set all of our desired properties we want to send....
	WDynamicLight::OnTick(dt);
}


void WSpotLight::UpdateWorldScaleOffset()
{
	float length = m_light.mProj.far_plane - m_light.mProj.near_plane;
	Matrix4 m;
	WMatrix16f tm(1);
	tm.SetTranslation(GetEye());

	//combine scale / rotation / translation matrices
	WMatrixMul(m_orientation,tm,m);

	//move the volume forward by length/2 so it will *BEGIN* at the origin (by default a cylinder mesh
	//is centered at the origin)
	Vector3 dir = m.GetForward();
	dir.Normalize();
	dir *= length/2;

	if (mZAxisFlip)
	{
		dir.Invert();
	}

	m.SetTranslation( m.GetTranslation() + dir );

	if (m_pLevel!=NULL)
	{
		EntityTransformUpdateEvent* etu = new EntityTransformUpdateEvent(m_id, m);
		m_pLevel->AddObjectStateChangeEvent(etu);
	}
}

}