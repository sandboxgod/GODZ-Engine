/* ===========================================================
	AssetStreamEntityComponent

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#pragma once

#include <CoreFramework/Core/EntityComponent.h>
#include <CoreFramework/Core/atomic_ref.h>
#include <CoreFramework/Core/MActor.h>


namespace GODZ
{
	class GenericPackage;
	class WEntity;

	/*
	*	Stores Multithreaded Actor which stores Navmesh polygon info, etc
	*/
	class GODZ_API MTActorComponent : public EntityComponent
	{
	public:
		static const UInt32 COMPONENT_NAME = 0x9B3F643A;

		MTActorComponent(WEntity* owner);
		~MTActorComponent();

		// *** EntityComponent interface  ***
		virtual void UpdatePhysics(CollisionStatus physicsState);
		virtual void OnSetPhysicsObject();
		virtual HashCodeID GetComponentName();
		// *** EntityComponent interface  ***

		MActor* GetMActor() { return mActor; }
		atomic_ptr<MActor>& GetMActorPtr() { return mActor; }

	private:
		atomic_ptr<MActor> mActor;
		size_t	m_polyIndex;			//navmesh polygon this entity standing on..
		bool m_standingOnPolygon;		//set to true if were on a navmesh polygon
	};
}

