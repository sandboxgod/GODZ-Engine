
#include "WPlayerController.h"
#include "WCharacter.h"
#include "PhysicsEntityComponent.h"
#include "MTActorComponent.h"
#include <CoreFramework/Core/WCamera.h>
#include <CoreFramework/Core/SceneManager.h>
#include <CoreFramework/Core/WindowManager.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Core/MActor.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/PlayerSkills/SkillDatabase.h>

namespace GODZ
{

ImplementGenericFlags(WPlayerController, ClassFlag_Transient)


WPlayerController::WPlayerController()
: m_elapsedTime(99999.0f) //default to some high value
{
	//PlayerControllers are not supposed to be serialized when a Level is saved. Thus, unlike other entities,
	//it is ok to call this singleton object within our constructor...
	InputSystem* input = InputSystem::GetInstance();
	input->AddInputListener(this);
}

WPlayerController::~WPlayerController()
{
	m_pCamera.reset();

	InputSystem* input = InputSystem::GetInstance();
	input->RemoveListener(this);
}

void WPlayerController::PossessCharacter(WCharacter * character)
{
	WController::PossessCharacter(character);

	atomic_ptr<WCamera> camera = m_pCamera.lock();
	if (camera != NULL)
	{
		camera->SetFocus(character);
	}

	//Get the list of skills available (skills database)
	SkillDatabase* skillDB = TheSceneManager->GetSkillDatabase();

	//load the values for skills from the database
	for (unsigned int i=0; i < PlayerSkill_Max; i++)
	{
		PlayerSkillType type = static_cast<PlayerSkillType>(i);
		PlayerSkillInfo& skill = character->GetPlayerSkillInfo(type);

		//for now we just reset the skill til we're hooked into a DB. Once we get a DB of some sort 
		//going. Upon character creation we will set this
		PlayerSkill* playerSkillInfo = skillDB->GetSkill(type);
		godzassert(playerSkillInfo != NULL);
		skill.Init(playerSkillInfo);
	}
}

void WPlayerController::ReceiveInputEvent(const InputBroadcast &event)
{
	m_lastBroadcast = event;

	atomic_ptr<WCamera> camera = m_pCamera.lock();

	if (camera != NULL)
	{
		camera->ReceiveInputEvent(m_lastBroadcast);
	}

	if (event.m_kType == IC_KeyPressed && event.m_keyState.m_state == KS_KeyDown)
	{
		switch(event.m_keyState.m_kAction)
		{
		case KA_ActivateEd:
			{
				IWindow *wi = WindowManager::GetInstance();
				bool isHidden=wi->IsMouseHidden();
				if (isHidden)
				{
					wi->ShowMouseCursor(true);
				}
				else
				{
					wi->ShowMouseCursor(false);
				}
			}
			break;
		}
	}
}

void WPlayerController::OnTick(float dt)
{
	/*
	//test
	if (m_pActor != NULL)
	{
		float angle = 1.0f;
		Quaternion q = m_pActor->GetRotation();

		//x=pitch, y = yaw/heading, z = roll
		Quaternion q1(Vector3(1,0.0,0.0), angle * dt);
		Quaternion result = q1 * q;
		m_pActor->SetRotation(result);
	}
	*/
	
	atomic_ptr<WCharacter> character = m_pActor.lock();
	atomic_ptr<WCamera> camera = m_pCamera.lock();

	m_elapsedTime += dt;

	if ( m_lastBroadcast.IsKeyPressed()
		&& character != NULL
		&& camera != NULL
		)
	{
		PhysicsEntityComponent* phys = (PhysicsEntityComponent*)character->GetEntityComponent(PhysicsEntityComponent::COMPONENT_NAME);
		if (phys == NULL)
			return;
		MTActorComponent* mtComp = (MTActorComponent*)character->GetEntityComponent(MTActorComponent::COMPONENT_NAME);
		if (mtComp == NULL)
			return;

		Vector3 vec(0,0,0);

		float moveSpeed = mtComp->GetMActor()->GetMoveSpeed();

		const WMatrix3& mat = character->GetOrientationMatrix();
		const float ang = DEG2RAD(camera->GetRotationSpeed());

		PhysicsObjectProxy* proxy = character->GetPhysicsObject();

		bool bMoved=false;

		if (m_lastBroadcast.IsKeyActionActive(KA_MoveUp))
		{
			//move forward 
			vec -= mat.GetForward() * moveSpeed;
			bMoved=true;
		}
		if (m_lastBroadcast.IsKeyActionActive(KA_MoveDown))
		{
			//move backward
			vec += mat.GetForward() * moveSpeed ;
			bMoved=true;
		}
		if(m_lastBroadcast.IsKeyActionActive(KA_MoveLeft))
		{
			//strafe left
			vec += mat.GetRight() * moveSpeed ;
			bMoved=true;
		}
		if(m_lastBroadcast.IsKeyActionActive(KA_MoveRight))
		{
			//strafe right
			vec -= mat.GetRight() * moveSpeed ;
			bMoved=true;
		}

		//vanity camera
		if(m_lastBroadcast.m_kType == IC_RightMClick)
		{
			if (m_lastBroadcast.m_keyState.m_state == KS_KeyDown)
			{
				camera->SetMouseLook(false);
				camera->ResetSavedPitchHeadingToViewTarget();
			}
			else
			{
				camera->SetMouseLook(true);
				camera->AddSavedPitchHeadingToViewTarget();
			}
		}
		else if(m_lastBroadcast.m_kType == IC_LeftMClick && m_lastBroadcast.m_keyState.m_state == KS_KeyDown
			&& !character->IsFiring()
			)
		{
			//fire projectiles!
			character->PlayFiring();
		}

		if (m_lastBroadcast.IsKeyActionActive(KA_Crouch))
		{
			vec -= mat.GetUp() * moveSpeed ; //move down
			bMoved=true;
		}
		if (m_lastBroadcast.IsKeyActionActive(KA_Jump))
		{
			if (phys->GetPhysics() == PhysicsEntityComponent::PhysicsType_Flying)
			{
				vec += mat.GetUp() * moveSpeed ; //move down
				bMoved=true;
			}

			//disable jumping; it just looks weird atm with the missing anims
			/*
			else if (phys->GetPhysics() == PhysicsEntityComponent::PhysicsType_Walking && proxy->GetCollisionState() == CollisionStatus_BaseFound)
			{
				//set the initial velocity to the current desired direction
				vec = m_pActor->GetVelocity();

				//get the player skill for jumping
				PlayerSkillInfo& skill = m_pActor->GetPlayerSkillInfo(PlayerSkill_Jump);
				
				//multiply jump talent * some number 
				float scale = skill.GetPowerScaled() * 3;
				scale = max(scale, 0.1f);
				vec += mat.GetUp() * (moveSpeed * (3 + scale) ) ; //move up
				bMoved=true;

				//temp - add points for jumping
				skill.AddSkillUp(0.05f);

				if (phys != NULL)
					phys->SetPhysics(PhysicsEntityComponent::PhysicsType_Jumping);

				m_elapsedTime = 0.0f;
			}
			*/
		}

		//see if the user just canceled movement
		bool bKeyReleased = m_lastBroadcast.IsKeyJustReleased(KA_MoveUp) || m_lastBroadcast.IsKeyJustReleased(KA_MoveDown) 
			|| m_lastBroadcast.IsKeyJustReleased(KA_MoveLeft) || m_lastBroadcast.IsKeyJustReleased(KA_MoveRight)
			|| m_lastBroadcast.IsKeyJustReleased(KA_Crouch) || m_lastBroadcast.IsKeyJustReleased(KA_Jump);


		bool bWalking = phys->GetPhysics() == PhysicsEntityComponent::PhysicsType_Walking || phys->GetPhysics() == PhysicsEntityComponent::PhysicsType_Jumping;

		if (!bMoved  && bKeyReleased && phys->GetPhysics() != PhysicsEntityComponent::PhysicsType_Jumping) //&& m_pActor->GetPhysics() == PhysicsType_Walking
		{
			character->SetVelocity(ZERO_VECTOR);
		}
		else if (bMoved || bWalking)
		{
			//Log("WPlayerController::OnTick vec:%f %f %f movespeed:%f\n",vec.x,vec.y,vec.z,moveSpeed);
			character->SetVelocity(vec);
		}		

		m_lastBroadcast.Reset();
	} //if
		
}

void WPlayerController::SetCamera(WCamera *cam)
{
	m_pCamera = atomic_ptr_cast<WCamera>( cam->GetWeakReference() );
}



} // namespace