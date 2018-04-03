/* ==============================================================
	Animation Controller

	Animation system control. Instanced per entity.
 
	Special Thanks goes out to Chris Hargrove for his article
	on computing animation playback rate at 
	http://www.gamedev.net/community/forums/topic.asp?topic_id=81429

	Created Jun 5, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#include "AnimationInstance.h"

#if !defined(__ANIMATION_CONTROLLER__H__)
#define __ANIMATION_CONTROLLER__H__

namespace GODZ
{
	struct Bone;
	class WEntity;

	class GODZ_API AnimController
	{
	public:
		AnimController();

		//returns true if the skin bound to this controller should recompute
		//it's skin vertices and bone transformations.
		bool IsAnimationAdvanced(float dt);
		void AdvanceFrame(float dt);

		void Freeze(bool flag) { mIsFrozen = flag; }
		bool IsFrozen() { return mIsFrozen; }

		AnimationInstance* GetAnimInstance() { return animInstance; }
		void SetAnimationInstance(AnimationInstance* animInstance);

		//returns the current ease value
		float GetEase() { return mEase; }

		//Returns the original EASE value
		float GetEaseSetting() { return mEaseMax; }

		float GetTime();
		void SetTime(float time);

		//Returns the time from the previous anim we were playing
		float GetPreviousTime() { return m_fPreviousTime; }

		//returns the current fps metric
		int GetFPS();

		//returns the current animation sequence
		int GetSequence();

		//returns the previous animation sequence
		int GetPreviousSequence() { return m_nLastSequenceIndex; }

		void OnAnimEnd(AnimSequence* pSequence);

		void Reset(AnimSequence* pSequence);

		//Plays the current animation sequence.
		//[in] sequenceIndex - animation sequence to play
		void PlayAnim(size_t sequenceIndex);

		//Plays the current animation sequence.
		//[in] sequenceIndex - animation sequence to play
		//[in] Ease - time in seconds to blend between previous and current anim after the switch
		//[in] playback_rate - frames per sec at which animation should be played.
		//For example 30, stands for 30fps.
		void PlayAnim(size_t sequenceIndex, float ease, float playback_rate = 30.0f);

		void StopAnimation();
		
		void SetEntity(WEntity* entity);
		
		void Update(Bone* bone, float dt, udword boneIndex);

		int GetPreviousAnimFrame() { return m_nOldAnimFrame; }
		int GetCurrentAnimFrame() { return m_nCurrentAnimFrame; }

	protected:
		bool mIsFrozen;
		float mEase;
		float mEaseMax; //ease that was passed in (backup)
		float m_fTime;			  //current time in animation sequence
		float m_fPreviousTime;	  //time previous animation was at
		float m_fAnimRate;		  //playback rate
		int m_nCurrentAnimFrame;
		int m_nOldAnimFrame;
		int m_nSequenceIndex;
		int m_nLastSequenceIndex;
		AnimationInstance* animInstance;
		WEntity* m_pEnt;
	};
}

#endif