/* ===========================================================
	AssetStreamEntityComponent

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#pragma once

#include <CoreFramework/Core/EntityComponent.h>
#include <CoreFramework/Core/GodzAtomics.h>
#include <CoreFramework/Core/HString.h>


namespace GODZ
{
	class GenericPackage;
	class WEntity;

	//Used to Stream meshes
	class GODZ_API AssetStreamEntityComponent : public EntityComponent
	{
	public:
		static const int COMPONENT_NAME = 2821720127; // AssetStreamEntityComponent


		AssetStreamEntityComponent(WEntity* owner, Future<GenericPackage*> future, HString name);
		virtual void OnTick(float dt);
		virtual HashCodeID GetComponentName();

	private:
		Future<GenericPackage*> mFuture;
		HString mResourceName;
	};
}

