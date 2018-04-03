
#include "AnimationController.h"
#include <CoreFramework/Core/WEntity.h>

using namespace GODZ;

AnimController::AnimController()
: m_fTime(0)
, m_fPreviousTime(0)
, m_nCurrentAnimFrame(0)
, m_nOldAnimFrame(-1)
, m_fAnimRate(0)
, m_nSequenceIndex(-1)
, m_nLastSequenceIndex(-1)
, animInstance(0)
, mEase(0.0f)
, mEaseMax(0.0f)
, m_pEnt(NULL)
, mIsFrozen(false)
{
}

// Should be queried by Bones, etc if they should update
bool AnimController::IsAnimationAdvanced(float dt)
{
	//compute local anim rate
	m_fTime += dt * m_fAnimRate;

	mEase -= dt;

	
	m_nCurrentAnimFrame=m_fTime;
	
	if (m_nCurrentAnimFrame!=m_nOldAnimFrame)
	{
		m_nOldAnimFrame=m_nCurrentAnimFrame;
		return true;
	}

	return false;
}

// Allows an external caller to advance the animation
void AnimController::AdvanceFrame(float dt)
{
	m_fTime += dt * m_fAnimRate;
}

void AnimController::StopAnimation()
{
	m_nSequenceIndex = -1;
}

int AnimController::GetFPS()
{
	return m_nCurrentAnimFrame;
}

int AnimController::GetSequence()
{
	return m_nSequenceIndex;
}

float AnimController::GetTime()
{
	return m_fTime;
}

void AnimController::SetTime(float time)
{
	m_fTime = time * m_fAnimRate;
}

void AnimController::OnAnimEnd(AnimSequence* pSequence)
{
	m_pEnt->OnAnimEnd(pSequence);
}

void AnimController::PlayAnim(size_t sequenceIndex)
{
	static const float EASE = 0.25f;
	AnimSequence* sequence = animInstance->GetSequence(sequenceIndex);
	PlayAnim(sequenceIndex, EASE, sequence->GetFPS());
}

void AnimController::PlayAnim(size_t sequenceIndex, float ease, float playbackRate)
{
	mEaseMax = ease;
	m_fPreviousTime = m_fTime;
	m_nLastSequenceIndex = m_nSequenceIndex; //backup old sequence
	m_nSequenceIndex=static_cast<int>(sequenceIndex);

	mEase = ease;
	m_fAnimRate=playbackRate;

	//reset to the beginning of the animation...
	m_fTime=0.0f;
}

void AnimController::SetAnimationInstance(AnimationInstance* animInstance)
{
	this->animInstance=animInstance;
}

void AnimController::SetEntity(WEntity* ent)
{
	m_pEnt = ent;
}

void AnimController::Update(Bone* bone, float dt, udword boneIndex)
{
	if (animInstance != NULL && m_nSequenceIndex >= 0)
	{
		animInstance->Update(this, bone, boneIndex);
	}
}

void AnimController::Reset(AnimSequence* pSequence)
{
	//reset to the beginning of the animation...
	m_fTime=0.0f;
	OnAnimEnd(pSequence);

	m_nCurrentAnimFrame=0;
	m_nOldAnimFrame=-1;
}
