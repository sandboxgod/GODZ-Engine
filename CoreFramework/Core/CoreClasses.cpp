
#include "CoreClasses.h"
#include "GenericObject.h"
#include "Material.h"
#include "IDriver.h"
#include "IGenericConfig.h"
#include "LevelNode.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "ResourceManager.h"
#include "SkeletalMesh.h"
#include "SkelMeshInstance.h"
#include "ShaderResource.h"
#include "GUIControl.h"
#include "GUIMenu.h"
#include "WEntity.h"
#include "WCamera.h"
#include "WDynamicLight.h"
#include "WSunlight.h"
#include "WSpotLight.h"
#include "WShadowSpotLight.h"
#include "WShadowPointLight.h"
#include "WPointLight.h"
#include "WSkyDome.h"
#include "WPhysicsVolume.h"
#include "EntityTemplate.h"
#include "NavMeshInfo.h"
#include "VisualComponent.h"

#include <CoreFramework/Animation/AnimationTrigger.h>
#include <CoreFramework/Animation/AnimationDesc.h>

using namespace GODZ;


void GODZ::LoadCoreClasses()
{
	NavMeshInfo::GetClass();
	Material::GetClass();
	MaterialParameter::GetClass();
	TextureMaterialParameter::GetClass();
	FloatMaterialParameter::GetClass();

	IGenericConfig::GetClass();
	LevelNode::GetClass();
	
	Mesh::GetClass();
	MeshInstance::GetClass();
	ModelContainer::GetClass();

	WEntity::GetClass();
	ResourceManager::GetClass();
	SkeletalMesh::GetClass();
	SkelMeshInstance::GetClass();
	WCamera::GetClass();
	WDynamicLight::GetClass();
	WSpotLight::GetClass();
	WShadowSpotLight::GetClass();
	WShadowPointLight::GetClass();
	WPointLight::GetClass();
	WSunLight::GetClass();
	EntityTemplate::GetClass();
	EntityComponent::GetClass();
	VisualComponent::GetClass();

	WPhysicsVolume::GetClass();
	GUIMenu::GetClass();
	
	WSkyDome::GetClass();

	AnimationTrigger::GetClass();
	AnimationDesc::GetClass();


	//TODO: All new classes need to be listed here to enable 'object loading via strings'
}