
#include "WCharacter.h"
#include "WController.h"
#include "CharacterTemplate.h"
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Core/Mesh.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Core/Layer.h>
#include <CoreFramework/Core/MActor.h>
#include <CoreFramework/Gameplay/Bot.h>
#include <CoreFramework/Gameplay/PhysicsEntityComponent.h>
#include <CoreFramework/Gameplay/MTActorComponent.h>

//////////////////////////////////////////////////////////////////////////////



namespace GODZ
{
ImplementGeneric(WCharacter)
REGISTER_FLOAT(WCharacter, Health)

//hash codes for the animation sequence categories
static const HashCodeID sJump = 125937960;
static const HashCodeID sIdle = 2081823275;
static const HashCodeID sRunF = 1991970818;
static const HashCodeID sRunB = 1909904411;
static const HashCodeID sRunL = 2523855132;
static const HashCodeID sRunR = 1818346623;
static const HashCodeID sFiring = 1383692164;

WCharacter::WCharacter()
: m_health(100)
, m_pTable(NULL)
, m_bIsFiring(false)
{
}

void WCharacter::OnAddToWorld()
{
	WEntity::OnAddToWorld();

	//add a physics plugin for this entity
	AddEntityComponent( atomic_ptr<EntityComponent>( new PhysicsEntityComponent(this) ) );
	AddEntityComponent( atomic_ptr<EntityComponent>( new MTActorComponent(this) ) );
}

void WCharacter::OnRemoved()
{
	WEntity::OnRemoved();

	if (m_pController != NULL)
	{
		m_pController->OnCharacterDestroyed();
		m_pController = NULL;
	}
}


void WCharacter::SetHealth(float health)
{
	m_health = health;
}

float	WCharacter::GetHealth()
{
	return m_health;
}

void WCharacter::OnMeshChanged()
{
	WEntity::OnMeshChanged();

	if (mesh != NULL && m_animController != NULL)
	{
		AnimationInstance* pAnim = m_animController->GetAnimInstance();
		m_pTable = &pAnim->GetAnimationTable();
		PlayAnim(sIdle);
	}
}

bool WCharacter::IsFiring()
{
	return this->m_bIsFiring;
}


void WCharacter::OnAnimEnd(AnimSequence* sequence)
{
	WEntity::OnAnimEnd(sequence);

	if (sequence->GetAnimType() == sFiring )
	{
		PlayAnim(sIdle);
		m_bIsFiring=false;
	}
}

void WCharacter::OnTick(float dt)
{
	WEntity::OnTick(dt);

	if (m_pController != NULL)
	{
		m_pController->OnTick(dt);
	}

	AnimationTable* pTable = m_pTable;
	if (!pTable)
	{
		return; //make sure the animation table has been built - Initialize()
	}

	Vector3 vel = GetVelocity();

	/*
	if (m_physicsObject.IsValid())
	{
		//hack to get things looking smooth for the demo....
		m_physicsObject.GetDesiredVelocity(vel);
	}
	*/

	float len = vel.GetLength();

	/*
	if (m_pController->GetRuntimeClass() == Bot::GetClass())
	{
		Log("speed %f\n", len);
	}
	*/

	if (len > 0.0f)
	{

		Vector3 X,Y,Z;
		m_orientation.GetAxes(X,Y,Z);
		
		vel.Normalize();

		//Compute velocity direction
		Vector3 axis(vel.Dot(X), vel.Dot(Y), vel.Dot(Z));
		//Log("axis: %f %f %f\n", axis.x,axis.y,axis.z);

		const float fMin = 0.01f;	//min amount of velocity

		//we use 0.7 instead of zero- since the axis can have a number like 
		//(0.03 0.000 0.9). The smaller quantity is ignored.
		if (axis.y > 0.7f && fabsf(m_vel.y) > fMin ) 
		{		
			PlayAnim(sJump); //play jump up animation
		}
		else if (axis.z > 0.7f && fabsf(m_vel.z) > fMin ) 
		{		
			PlayAnim(sRunB);
		}
		else if (axis.z < -0.7f && fabsf(m_vel.z) > fMin ) 
		{		
			PlayAnim(sRunF);
		}

		else if (axis.x > 0.7f && fabsf(m_vel.x) > fMin )
		{		
			PlayAnim(sRunL);
		}
		else if (axis.x < -0.7f && fabsf(m_vel.x) > fMin )
		{		
			PlayAnim(sRunR);
		}
		else
		{
			PlayAnim(sIdle);
		}
	}
	else if (!m_bIsFiring)
	{
		//play still animation
		PlayAnim(sIdle);
	}
}

void WCharacter::PlayAnim(HashCodeID kType)
{
	if (m_pTable == NULL)
	{
		return;
	}

	//current sequence
	int currIndex = m_animController->GetSequence();
	AnimSequence* currSeq = NULL;
	
	if (currIndex > -1)
	{
		currSeq = m_animController->GetAnimInstance()->GetSequence(currIndex);
	}

	AnimInfo* seq = m_pTable->GetSequences(kType);

	if ( (currIndex == -1 && seq != NULL)
		|| (seq != NULL && currSeq != NULL && currSeq->GetAnimType() != seq->m_kAnimType)
		)
	{
		size_t index = seq->GetSequence();
		m_animController->PlayAnim(index);
	}
}

void WCharacter::PlayFiring()
{
	if (m_pTable )
	{
		m_bIsFiring=true;

		AnimInfo* pSeq = m_pTable->GetSequences(sFiring);
		int currIndex = m_animController->GetSequence();
		AnimSequence* currSeq = m_animController->GetAnimInstance()->GetSequence(currIndex);

		if (pSeq != NULL && currSeq->GetAnimType() != pSeq->m_kAnimType)
		{
			size_t index = pSeq->GetSequence();
			m_animController->PlayAnim(index);
		}
	}
}

void WCharacter::SetController(WController* controller)
{
	m_pController = controller;
}


void WCharacter::OnHit(float damage, WEntity* instigator)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		m_health = 0;

		//This character is dead....
	}
}



} //namespace GODZ