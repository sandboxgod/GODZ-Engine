
#include "WShadowPointLight.h"
#include "WShadowSpotLight.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include "GeometryInfo.h"
#include "Layer.h"


namespace GODZ
{

//Uses technique found in http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter09.html whereas
//we emulate cube shadow mapping (omni lighting) via 6 spotlights


ImplementGenericFlags(WShadowPointLight, ClassFlag_Placeable)
REGISTER_FLOAT(WShadowPointLight, LightRange)
REGISTER_FLOAT(WShadowPointLight, Far)
REGISTER_COLOR(WShadowPointLight, Color)

WShadowPointLight::WShadowPointLight()
: mFar(1000)
, mLightRange(320)
, mColor(1,0,1,1)
{
	//todo: need a model for culling???
}

void WShadowPointLight::UpdateGeometryInfo(GeometryInfo& info)
{
	info.m_type = GeometryBaseInfo::VisualType_ReferenceOnly;
}

void WShadowPointLight::OnAddToWorld()
{
	WEntity::OnAddToWorld();

	const float angle = 1.57f; //0.785f; //1.57f;

	Vector3 spotLightRot[6];
	spotLightRot[0] = Vector3(1,0,0);
	spotLightRot[1] = Vector3(-1,0,0);
	spotLightRot[2] = Vector3(0,1,0);
	spotLightRot[3] = Vector3(0,-1,0);
	spotLightRot[4] = Vector3(0,0,1);
	spotLightRot[5] = Vector3(0,0,-1); //cylinder already offset along Z+


	/*
	//for debugging....
	Color4f debugColor[6];
	debugColor[0] = Color4f(1,0,0,1);
	debugColor[1] = Color4f(0.2f,0,0,1);
	debugColor[2] = Color4f(0,1,0,1);
	debugColor[3] = Color4f(0.6f,0.0f,0.6f,1);
	debugColor[4] = Color4f(0,0,1,1);
	debugColor[5] = Color4f(0,1,1,1);
	*/

	//spawn 6 spotlights relative to our location & attach them to us. This way anytime we're moved,
	//our child actors will translate relative to our origin....
	for(int i = 0; i < 6; i++)
	{
		Quaternion q(spotLightRot[i], angle);
		Matrix3 m;
		QuaternionToMatrix(q, m);

		atomic_ptr<WShadowSpotLight> ptr( new WShadowSpotLight() );
		mLights[i] = ptr;
		ptr->SetLocation(GetLocation());

		//orient the spotlight in the desired direction (X+, X-, Y+, Y-, Z+, Z-)
		ptr->SetOrientationMatrix(m);
		ptr->SetColor(mColor);
		ptr->SetConeRadius(1000);
		ptr->SetFar(mFar);
		ptr->SetLightRange(mLightRange);

		if (i == 5)
		{
			//hack to get the darn cylinder to flip
			ptr->SetZAxisFlip(true);
		}

		atomic_ptr<WEntity> entity_ptr( ptr );
		GetLayer()->AddActor(entity_ptr);

		//Lastly, attach the spotlight to us....
		AttachActor(ptr);
	}
}

void WShadowPointLight::OnRemoved()
{
	WEntity::OnRemoved();

	//clear references
	for(int i = 0; i < 6; i++)
	{
		mLights[i].reset();
	}
}

void WShadowPointLight::SetFar(float far)
{
	mFar = far;
	for(int i = 0; i < 6; i++)
	{
		atomic_ptr<WShadowSpotLight> light = mLights[i].lock();
		if (light != NULL)
		{
			light->SetFar(far);
		}
	}
}

float WShadowPointLight::GetFar()
{
	return mFar;
}

void WShadowPointLight::SetLightRange(float r)
{
	mLightRange = r;
	for(int i = 0; i < 6; i++)
	{
		atomic_ptr<WShadowSpotLight> light = mLights[i].lock();
		if (light != NULL)
		{
			light->SetLightRange(r);
		}
	}
}

float WShadowPointLight::GetLightRange()
{
	return mLightRange;
}

const Color4f& WShadowPointLight::GetColor() const
{
	return mColor;
}

void WShadowPointLight::SetColor(const Color4f& rgba)
{
	mColor = rgba;
	for(int i = 0; i < 6; i++)
	{
		atomic_ptr<WShadowSpotLight> light = mLights[i].lock();
		if (light != NULL)
		{
			light->SetColor(rgba);
		}
	}
}

}