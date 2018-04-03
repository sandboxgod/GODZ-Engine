#pragma once

#include <CoreFramework/Core/GenericObject.h>

namespace GODZ
{
	class WEntity;

	class GODZ_API AnimationTrigger : public GenericObject
	{
		DeclareGenericBase(AnimationTrigger, GenericObject)

	public:
		int GetTriggerFrame() { return mTriggerFrame; }
		void SetTriggerFrame(int frame) { mTriggerFrame = frame; }

		virtual void apply(WEntity* entity) = 0;

	protected:
		AnimationTrigger();
		int mTriggerFrame;
	};
}
