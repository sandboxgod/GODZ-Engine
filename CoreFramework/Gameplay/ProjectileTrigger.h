#pragma once

#include <CoreFramework/Animation/AnimationTrigger.h>
#include <CoreFramework/Core/atomic_ref.h>

namespace GODZ
{
	class ProjectileTemplate;

	class GODZ_API ProjectileTrigger : public AnimationTrigger
	{
		DeclareGeneric(ProjectileTrigger, AnimationTrigger)

	public:
		void SetProjectileType(atomic_ptr<ProjectileTemplate>& projTemplate) { mTemplate = projTemplate; }
		atomic_ptr<ProjectileTemplate> GetProjectileType() { return mTemplate; }

		void SetRefNode(HString name) { mRefNode = name; }
		HString GetRefNode() { return mRefNode; }

		virtual void apply(WEntity* entity);

	protected:
		atomic_ptr<ProjectileTemplate> mTemplate;
		HString mRefNode;
	};
}
