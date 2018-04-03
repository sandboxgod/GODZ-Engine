/* ===========================================================

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__LAYER_H__)
#define __LAYER_H__

#include "Godz.h"
#include "GenericObjectList.h"
#include "NavigationMesh.h"
#include "EntityPass.h"
#include "atomic_ref.h"
#include "NavMeshInfo.h"
#include "SlotIndex.h"
#include <vector>
#include <queue>
#include <CoreFramework/Math/ViewFrustum.h>
#include <CoreFramework/Math/Quaternion.h>


namespace GODZ
{
	class LevelNode;
	class LevelEntityData;
	class WEntity;
	class WCamera;
	class GenericPackage;
	class NavMeshInfo;

	class GODZ_API Layer
	{
	public:
		Layer(HString name, LevelNode* level, LevelEntityData* data);
		~Layer();

		HString GetName() { return mName; }

		//adds the entity directly to this layer
		void AddActor(const atomic_ptr<WEntity>& entity_ref);
		size_t GetNumActors();
		WEntity* GetActor(size_t index);

		//Spawns a new actor (on the default layer by default).
		atomic_ptr<WEntity> SpawnActor(GenericClass* Class, const Vector3& location, const Quaternion& rotation);
		atomic_ptr<WEntity> SpawnActor(GenericClass* Class);

		void GetRenderElements(std::vector<EntityPassList>& elements, ViewFrustum& frustum, ViewFrustum& lightFrustum, VisibilityChangeInfo& status);
		void DoTick(float dt);
		void GetActors(GenericClass* classType, GenericObjectList& list);
		bool ReleaseEntity(WEntity* entity);
		WEntity* PickEntity(const WRay& ray, float& bestDist, WEntity* currentBest);
		void Exit();

		LevelNode* GetLevel() { return mLevel; }

		NavigationMesh&			GetNavigationMesh();
		atomic_ptr<NavMeshInfo>			GetNavMeshInfo() { return m_navMeshInfo; }

		GenericPackage* GetPackage();
		void SetPackage(GenericPackage* p);

		void SaveToPackage(const char* file);

		//single threaded; waits til package loads
		void LoadFromPackage(const char* file);

		//should be called if package object is known; this way the package can be streamed and we grab
		//all of our entities after...
		void LoadFromPackage(GenericPackage* package);
		
	private:
		LevelEntityData* mEntityData;
		typedef std::vector<EntityID> EntityList;
		EntityList m_actors;					//actors that exist within this layer
		HString mName;													//name of this layer
		atomic_ptr<NavMeshInfo> m_navMeshInfo;
		LevelNode* mLevel;
		GenericReference<GenericPackage> m_package;
		
	};
}

#endif