#pragma once

#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/GodzVector.h>
#include <CoreFramework/Core/atomic_ref.h>
#include "AnimationTrigger.h"

namespace GODZ
{
	class GODZ_API AnimationDesc : public GenericObject
	{
		DeclareGeneric(AnimationDesc, GenericObject)

	public:
		GodzVector<atomic_ptr<AnimationTrigger> >& GetAnimTriggers() { return mTriggers; }

		HString GetAnimationName() { return mAnimationName; }
		void SetAnimationName(HString anim) { mAnimationName = anim; }

	protected:
		HString mAnimationName;

		//stores list of Animation Triggers....
		GodzVector<atomic_ptr<AnimationTrigger> > mTriggers;
	};
}
