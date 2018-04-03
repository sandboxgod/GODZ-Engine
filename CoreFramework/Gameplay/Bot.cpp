
#include "Bot.h"
#include "WCharacter.h"
#include "MTActorComponent.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Core/JobManager.h>
#include <CoreFramework/Core/SceneManager.h>
#include <CoreFramework/Core/Layer.h>
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Core/MActor.h>

namespace GODZ
{

ImplementGeneric(Bot)

Bot::Bot()
: m_jobStarted(false)
{
}

Bot::~Bot()
{
	SceneManager* scenemngr = SceneManager::GetInstance();
	JobManager& mngr = scenemngr->GetJobManager();
	mngr.removeJob(&m_bot);
}

void Bot::PossessCharacter(WCharacter * character)
{
	if (!m_jobStarted)
	{
		SceneManager* scenemngr = SceneManager::GetInstance();
		JobManager& mngr = scenemngr->GetJobManager();

		character->SetController(this);
		MTActorComponent* mtComp = (MTActorComponent*)character->GetEntityComponent(MTActorComponent::COMPONENT_NAME);
		if ( mtComp != NULL)
		{
			atomic_ptr<MActor> multiThreadActor_ptr = mtComp->GetMActorPtr();
			mActorProxy = multiThreadActor_ptr;

			//Initialize the MBot before we start it....
			m_bot.SetEntity(multiThreadActor_ptr);
			m_bot.SetNavMesh(&character->GetLayer()->GetNavigationMesh());

			//Install this job on the current jobmngr
			mngr.addJob(&m_bot);
			m_jobStarted = true;
		}
	}
	else
	{
		godzassert(0); //bots cannot switch characters yet...
	}
}

void Bot::SetTarget(WEntity* target)
{
	MTActorComponent* mtComp = (MTActorComponent*)target->GetEntityComponent(MTActorComponent::COMPONENT_NAME);
	if (mtComp != NULL)
	{
		m_bot.SetTarget(mtComp->GetMActorPtr());
	}
}

void Bot::OnCharacterDestroyed()
{
	SceneManager* scenemngr = SceneManager::GetInstance();
	JobManager& mngr = scenemngr->GetJobManager();
	mngr.removeJob(&m_bot);
}

void Bot::OnTick(float dt)
{
	atomic_ptr<WCharacter> character = m_pActor.lock();
	atomic_ptr<MActor> mactor_ptr = mActorProxy.lock();

	if( mactor_ptr == NULL || character == NULL)
	{
		return;
	}

	if (mactor_ptr->IsRotationDirty())
	{
		Quaternion rot;
		mactor_ptr->GetRotation( rot );

		WMatrix mrot;
		QuaternionToMatrix(rot, mrot);

		godzassert(!IsNaN(rot.q.x));
		godzassert(!IsNaN(rot.q.y));
		godzassert(!IsNaN(rot.q.z));
		godzassert(!IsNaN(rot.q.w));

		character->SetOrientationMatrix( mrot );
		mactor_ptr->UnsetRotation();
	}

	if (mactor_ptr->IsOrientationDirty())
	{
		WMatrix3 rot;
		mactor_ptr->GetOrientation( rot );

		character->SetOrientationMatrix( rot );
		mactor_ptr->UnsetOrientation();
	}

	if (mactor_ptr->IsAnimationDirty())
	{
		AnimData anim;
		mactor_ptr->GetAnimation(anim);
		character->GetAnimController()->PlayAnim(anim.m_id, anim.m_ease);
		mactor_ptr->UnsetAnimation();
	}
}



}