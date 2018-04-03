
#include "AssetStreamEntityComponent.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Core/ClassRegistry.h>
#include <CoreFramework/Core/GenericPackage.h>
#include <CoreFramework/Core/Mesh.h>
#include <CoreFramework/Core/WEntity.h>


namespace GODZ
{

AssetStreamEntityComponent::AssetStreamEntityComponent(WEntity* owner, Future<GenericPackage*> future, HString name)
: EntityComponent(owner)
, mFuture(future)
, mResourceName(name)
{
}

HashCodeID AssetStreamEntityComponent::GetComponentName()
{
	return COMPONENT_NAME; //AssetStreamEntityComponent
}

void AssetStreamEntityComponent::OnTick(float dt)
{
	if (mFuture.isReady())
	{
		GenericPackage* p = mFuture.getValue();
		atomic_ptr<GenericObject> objPtr = p->FindObjectRef(mResourceName);

		if (objPtr == NULL)
		{
			Log("AssetStreamEntityComponent: Missing asset %s\n", mResourceName.c_str());
		}
		else
		{
			atomic_ptr<Mesh> meshPtr( objPtr );
			mOwner->SetMesh(meshPtr);
		}

		mOwner->RemoveEntityComponent(this);
	}
}

}