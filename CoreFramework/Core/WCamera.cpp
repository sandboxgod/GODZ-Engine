
#include <math.h>
#include "WCamera.h"
#include "SceneManager.h"
#include "InputSystem.h"
#include "Timer.h"
#include "WindowManager.h"
#include "IWindow.h"
#include "GodzAtomics.h"
#include "LevelNode.h"
#include "SceneData.h"
#include <CoreFramework/Collision/AABBCollisionPrimitive.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Math/vectormath.h>
#include <CoreFramework/Gameplay/PhysicsEntityComponent.h>
#include <CoreFramework/Gameplay/WPlayerController.h>

namespace GODZ
{

ImplementGeneric(WCamera)

const float WCamera::CHARACTER_ROT_OFFSET = 3.14f;

WCamera::WCamera()
: m_playerController(NULL)
, flyspeed(60400.0f)
, m_bUpdateCamera(false)
, m_fRotateSpeed(90.0f)
, m_cameradist(0)
, m_bMouseLook(true)
, m_transitionSpeed(8.5f)
, m_lookAtOffset(40.0f)
, m_state(CameraState_Normal)
, mLerpTime(0)
, m_CameraMode(CT_FPS)
, mIsFlying(false)
, m_offsetMatrix(1)
, m_viewMatrix(1)
, m_vanityQ(Quaternion::getIdentity())
, m_viewDist(Vector3::Zero())
{
	m_bEditable=false;
	const float defaultDist = 250;
	m_viewDist = Vector3(defaultDist,defaultDist,defaultDist);

	//set collision bounds -- camera does collision detection to make sure it doesnt go through walls....
	WBox box(Vector3(-10,-10,-10),Vector3(10,10,10));

	CreateCollisionModel();

	godzassert(m_model->GetCollisionType() == CollisionPrimitive::CollisionType_AABB);
	AABBCollisionPrimitive* aabb = (AABBCollisionPrimitive*)m_model.m_ref;
	aabb->SetBounds(box);

	m_bIgnoreOutOfBounds=true; //do not destroy the camera if it goes out-of-world
	m_bVisible=false; //camera isn't rendered
	SetCanCollideWithWorld(false);
	SetIsShadowCaster(false);

	m_lastEvent.Reset();

	m_bUpdateCamera=true;

	mCameraInfo.m_id = SceneData::CreateRuntimeID();
	mCameraInfo.mType = GetCameraType();

	m_mousePos.x = 0;
	m_mousePos.y = 0;

	const char* speed = GlobalConfig::m_pConfig->ReadNode("Camera","MoveSpeed");
	if (speed != NULL)
	{
		m_transitionSpeed = atof(speed);
	}

	//m_bMouseLook = GlobalConfig::m_pConfig->ReadBoolean("Camera", "MouseLook");

	const char* cameraSpeed = GlobalConfig::m_pConfig->ReadNode("Camera","Speed");
	if (cameraSpeed!=NULL)
	{
		flyspeed = atof(cameraSpeed);
	}
}



WCamera::~WCamera()
{
}

CameraID WCamera::GetCameraID()
{
	return mCameraInfo.m_id;
}

//invoked when this entity is removed from a layer
void WCamera::OnRemoved()
{
	WEntity::OnRemoved();

	CameraDestroyEvent* ev = new CameraDestroyEvent(mCameraInfo.m_id);
	m_pLevel->AddObjectStateChangeEvent(ev);

	m_pViewTarget.reset();
	if (m_playerController != NULL)
	{
		m_playerController->SetCamera(NULL);
		m_playerController=NULL;
	}
}

float WCamera::GetRotationSpeed()
{
	return m_fRotateSpeed;
}


void WCamera::CalcBehindView(float dt)
{
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();

	if (viewTarget == NULL || viewTarget->GetCollisionModel() == NULL || m_state == CameraState_Transition )
	{
		return;
	}

	//Be careful you will see screen flicker if you grab the "old" character position.
	//We must always instantly update the view matrix as soon as the view target computes
	//a new position

	Vector3 pos = viewTarget->GetLocation();

	CollisionPrimitive* pModel = viewTarget->GetCollisionModel();
	WBox box;
	pModel->GetBox(box);
	Vector3 extent = box.GetExtent();

	Vector3 up = viewTarget->GetOrientationMatrix().GetUp();
	up.Normalize();

	pos += up * extent.y;
	
	Vector3 dist = (pos - m_pos);
	Vector3 move = dist * dt * m_transitionSpeed;

	if (move.GetLengthSQ() > dist.GetLengthSQ())
	{
		move = dist;
	}

	m_pos += move;
	m_bUpdateCamera=true;
}

Vector3 WCamera::GetTarget()
{
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	//return lookAt vector - used as a component of the view matrix
	if (viewTarget != NULL)
	{
		Vector3 target = viewTarget->GetLocation();
		Vector3 up = viewTarget->GetOrientationMatrix().GetUp();
		up.Normalize();

		target += up * m_lookAtOffset;
		return target;
	}
	

	return m_orientation.GetForward() + m_pos;
}

Vector3 WCamera::GetEye()
{
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	if (viewTarget != NULL)
	{
		Vector3 target;
		Vector3 dir;

		if (mIsFlying)
		{
			//compute target & dir
			const WMatrix3& m = viewTarget->GetOrientationMatrix();
			dir = m.GetForward();
			dir.Normalize();
			target = viewTarget->GetLocation();

			if (!m_bMouseLook)
			{
				m_offsetMatrix.Mul(dir);
				
				//Quaternion must be unit when rotating a vector
				//m_vanityQ.normalize();
				//m_vanityQ.rotate(dir);
			}
		}
		else
		{
			target = m_pos;
			dir = m_orientation.GetForward();
			dir.Normalize();
			dir.Invert();
		}

		// Position eye so that's always "kCamDist" from target
		Vector3 dest = target + (dir * m_viewDist);

		Vector3 origin = viewTarget->GetLocation();

		Vector3 veldir = dest - origin;

		m_model->SetPosition(m_pos);

		//Note: if we remove code below everything seems to break with massive FPS drop :( ???
		//sweep from our origin (viewtarget) to the desired location where we want to be....
		Future<CollisionList> future;
		m_pLevel->Sweep(future, m_model, veldir, viewTarget->GetPhysicsObject());
		while(!future.isReady()); //SAD! Block until we get collision result...
		CollisionList& list = future.getValue();

		int numresults = list.GetNumResults();

		//Note: all the results are presorted which makes it easy to determine where to put the camera
		for (int i = 0; i < numresults; i++)
		{
			//Only collide with Level geo
			if (list[i].model.GetMotionType() == MotionType_Static)
			{
				//else pick someone along the veldir, we only care bout 1st result since everything should be sorted...
				float d = list[i].m_distance;
				WBox b;
				m_model->GetBox(b);
				d -= b.GetExtent().x;

				veldir.Normalize();
				dest = origin + (d * veldir);
				break;
			}
		}

		return dest;
	}
	else
	{
		return m_pos;
	}
}

void WCamera::OnTick(float dt)
{
	WEntity::OnTick(dt);

	if (m_state == CameraState_Transition )
	{
		/*
		//Rotate the camera to face the same direction the character is facing...
		Vector3 rot = m_pViewTarget->GetRotation();

		//hardcoded, characters are rotated to face the world (instead of FRONT VIEW like in 3dsmax)
		rot.z -= 3.14f;

		//compute destination rotation
		Quat4f destQ;
		destQ.fromYawPitchRoll(rot.x, rot.y, rot.z);

		Quat4f srcQ(m_orientation);

		//slerp to destination....
		Quat4f result;
		result = Quat4f::slerp(srcQ, destQ, Clip(mLerpTime,0,1));
		mLerpTime+=dt;

		QuaternionToMatrix(result, m_orientation);

		godzassert(!IsNaN(m_orientation._11));
		godzassert(!IsNaN(m_orientation._12));
		godzassert(!IsNaN(m_orientation._13));
		godzassert(!IsNaN(m_orientation._14));

		godzassert(!IsNaN(m_orientation._21));
		godzassert(!IsNaN(m_orientation._22));
		godzassert(!IsNaN(m_orientation._23));
		godzassert(!IsNaN(m_orientation._24));

		godzassert(!IsNaN(m_orientation._31));
		godzassert(!IsNaN(m_orientation._32));
		godzassert(!IsNaN(m_orientation._33));
		godzassert(!IsNaN(m_orientation._34));

		godzassert(!IsNaN(m_orientation._41));
		godzassert(!IsNaN(m_orientation._42));
		godzassert(!IsNaN(m_orientation._43));
		godzassert(!IsNaN(m_orientation._44));

		if (mLerpTime >= 1.0f)
		{
			m_state = CameraState_Normal;
		}
		*/
	}
}

void WCamera::OnCameraTick(float dt)
{
	if (m_lastEvent.m_kType == IC_KeyPressed && m_lastEvent.m_keyState.m_kAction!=KA_Unknown)
	{
		m_lastEvent.m_fDelta=dt; //store deltatime

		atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
		if (viewTarget == NULL)
		{
			Vector3 location = GetLocation();

			float moveSpeed=flyspeed * dt;
			m_bUpdateCamera=false;

			{
				switch(m_lastEvent.m_keyState.m_kAction)
				{
					case KA_MoveUp: 
						//move forward 
						location += m_orientation.GetForward() * moveSpeed;
						break;
					case KA_MoveDown:
						//move backward
						location -= m_orientation.GetForward() * moveSpeed;
						break;
					case KA_MoveLeft:
						//strafe left
						location -= m_orientation.GetRight() * moveSpeed;
						break;
					case KA_MoveRight:
						//strafe right
						location += m_orientation.GetRight() * moveSpeed;
						break;
					case KA_Crouch:
						location -= m_orientation.GetUp() * moveSpeed; //move down
						break;
					case KA_Jump:
						location += m_orientation.GetUp() * moveSpeed; //move up
						break;
				}

				SetLocation(location);
				
				m_pos=location;

				//update the translation for this camera's matrix
				m_bUpdateCamera=true;
				//m_orientation.SetTranslation(location);
			}
		}

		m_lastEvent.m_keyState.m_kAction = KA_Unknown;
	}

	UpdateCamera(dt);
	CalcBehindView(dt);

	//for now we push camera data every frame :(
	//TODO: make it so we only push changes.... although camera might always change per frame unless its inactive....
	PushCameraInfo(GetEye(), GetTarget());
}

void WCamera::PushCameraInfo(const Vector3& eye, const Vector3& target) 
{
	Vector3 up;
	ComputeUpVector(up);

	ComputeViewMatrix(eye,target, up, m_viewMatrix);

	mCameraInfo.m_viewMatrix = m_viewMatrix;
	mCameraInfo.m_viewMatrixInfo.eye = eye;
	mCameraInfo.m_viewMatrixInfo.lookAt = target;
	mCameraInfo.m_viewMatrixInfo.up = up;
	mCameraInfo.m_camLocation = eye;

	//needs to be here for the dynamic call to work
	mCameraInfo.mType = GetCameraType();

	CameraCreationEvent* ev = new CameraCreationEvent(mCameraInfo);
	m_pLevel->AddObjectStateChangeEvent(ev);
}

void WCamera::ComputeUpVector(Vector3& up)
{
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	if (viewTarget != NULL)
	{
		//Use character's UP vector
		up = viewTarget->GetOrientationMatrix().GetUp();
	}
	else
	{
		up = UP_VECTOR;
	}
}

WCamera::CameraMode WCamera::GetCameraMode()
{
	return m_CameraMode;
}


float WCamera::GetSpeed()
{
	return flyspeed;
}

void WCamera::SetSpeed(float speed)
{
	flyspeed=speed;
}


void WCamera::Pitch(float angle, bool applyPitchViewTarget)
{
	m_bUpdateCamera=true;

	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	if (viewTarget != NULL && applyPitchViewTarget)
	{
		if (mIsFlying)
		{
			Matrix3 m = viewTarget->GetOrientationMatrix();
			Vector3 dir = m.GetRight();
			dir.Normalize();

			Matrix3 tm(1);
			tm.RotateAroundAxis(dir, angle);

			//Quaternion t(dir, -angle);

			if (m_bMouseLook)
			{
				m *= tm;
				viewTarget->SetOrientationMatrix(m);

				/*
				Quaternion q = m_pViewTarget->GetRotation();
				q = q * t;
				m_pViewTarget->SetRotation(q);
				*/
			}
			else
			{
				//t.conjugate();
				//m_vanityQ = m_vanityQ * t;

				m_offsetMatrix *= tm;
			}
		}
	}
	
	
	if (!mIsFlying)
	{
		Vector3 right,forward,up;
		m_orientation.GetAxes(right,up,forward);

		WMatrix16f m(1);
		m.RotateAroundAxis(right, angle);
		m.Mul(forward);
		m.Mul(up);

		m_orientation._31 = forward.x;
		m_orientation._32 = forward.y;
		m_orientation._33 = forward.z;

		up = forward.Cross(right);
		m_orientation._21 = up.x;
		m_orientation._22 = up.y;
		m_orientation._23 = up.z;
	}
	
}

float WCamera::GetPitch()
{
	Vector3 up;
	ComputeUpVector(up);

	Vector3 forward = m_orientation.GetForward();
	return ComputeAngle(forward, up);
}

void WCamera::RotateY(float angle)
{
	m_bUpdateCamera=true;

	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	if (viewTarget != NULL)
	{
		WMatrix3 m = viewTarget->GetOrientationMatrix();
		Vector3 dir = m.GetUp();
		dir.Normalize();

		WMatrix3 tm(1);
		tm.RotateAroundAxis(dir, angle);
		
		//Quaternion t(dir, -angle);

		if (m_bMouseLook)
		{
			m *= tm;
			viewTarget->SetOrientationMatrix(m);

			/*
			Quaternion q = m_pViewTarget->GetRotation();
			q = q * t;
			m_pViewTarget->SetRotation(q);
			*/
		}
		else
		{
			//t.conjugate();
			//m_vanityQ = m_vanityQ * t;
			m_offsetMatrix *= tm;
		}
	}
	
	
	if (!mIsFlying)
	{
		WMatrix rot(1);
		rot.RotateY(angle);
		m_orientation *= rot;
	}
	
}

void WCamera::SetViewTargetRotation()
{
}

void WCamera::ResetSavedPitchHeadingToViewTarget()
{
	m_offsetMatrix.MakeIdentity();
	m_vanityQ = Quaternion::getIdentity();
}

void WCamera::AddSavedPitchHeadingToViewTarget()
{
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	if( viewTarget != NULL)
	{
		//apply saved rotations to the character
		WMatrix3 m = viewTarget->GetOrientationMatrix();
		m *= m_offsetMatrix;
		viewTarget->SetOrientationMatrix(m);

		/*
		Quaternion q = m_pViewTarget->GetRotation();

		if (!mIsFlying)
		{
			m_vanityQ.conjugate();
			//m_vanityQ.normalize();
		}
		q = q * m_vanityQ;
		m_pViewTarget->SetRotation(q);
		*/
	}
	

	//reset saved values
	m_offsetMatrix.MakeIdentity();
	//m_vanityQ = Quaternion::getIdentity();
}

void WCamera::ReceiveInputEvent(const InputBroadcast &event)
{	
	switch (event.m_kType)
	{
	case IC_MouseWheel:
		{
			m_viewDist += -event.m_fDelta * 0.5f;
			m_cameradist += -event.m_fDelta * 0.5f;
		}
		break;
	}

	m_lastEvent=event;
}

WEntity* WCamera::GetFocus()
{
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();
	return viewTarget;
}

void WCamera::SetFocus(WEntity* entity)
{
#ifdef _TEST_CAMERA_
	ent=NULL;
#endif

	m_pViewTarget=entity->GetWeakReference();
	atomic_ptr<WEntity> viewTarget = m_pViewTarget.lock();

	if (viewTarget != NULL)
	{
		m_orientation.MakeIdentity();

		const Matrix3& m = viewTarget->GetOrientationMatrix();
		Vector3 up = viewTarget->GetOrientationMatrix().GetUp();
		Quaternion rot = m;

		//Swing the camera to the back of the entity
		Quaternion q(up, CHARACTER_ROT_OFFSET);
		Quaternion result = q * rot;

		QuaternionToMatrix(result, m_orientation);
		//Note: used to invoke setorientationmatrix()

		Pitch(0.4f, false);

		//needs to be called otherwise first time the camera is moved everything gets jerked around
		IWindow* WI = WindowManager::GetInstance();
		WI->CenterCursor();

		PhysicsEntityComponent* phys = (PhysicsEntityComponent*)viewTarget->GetEntityComponent(PhysicsEntityComponent::COMPONENT_NAME);
		if (phys != NULL)
		{
			mIsFlying = (phys->GetPhysics() == PhysicsEntityComponent::PhysicsType_Flying);
		}
	}
}

void WCamera::ResetFocus()
{
	m_state = CameraState_Transition; //camera is lerping....
	mLerpTime = 0;
}

void WCamera::UpdateFrustum(const WMatrix& combMat)
{
	m_frustum.ExtractPlanes(combMat);
}

const WMatrix&	WCamera::GetViewMatrix() 
{ 
	return m_viewMatrix;
}

void WCamera::UpdateCamera(float dt)
{
	IWindow* WI = WindowManager::GetInstance();

	if (WI->IsMouseHidden() || WI->IsRightMouseButtonDown())
	{
		int mouse_x = 0, mouse_y = 0;
		WI->GetMousePos(mouse_x,mouse_y);

		if ( m_mousePos.x != mouse_x || m_mousePos.y != mouse_y)
		{
			UpdateView(mouse_x,mouse_y,WI->GetWidth() / 2, WI->GetHeight() / 2, dt);

			WI->CenterCursor();

			WI->GetMousePos(mouse_x,mouse_y);
			m_mousePos.x = mouse_x;
			m_mousePos.y = mouse_y;
		}
	}
}

void WCamera::UpdateView(int xPos, int yPos, int originW, int originH, float dt)
{

	const float deg_per_pixel = 6;
	const float kDegInc = DEG2RAD(m_fRotateSpeed); // Amount to rotate in degrees - 0.75f
	const int kCushion = 0; // "Cushion" amount from the center
									// of the window the user must move the mouse
								   // before we rotate their view 

	float xoff = 0, yoff = 0;

	if(xPos < (originW - kCushion)) // If they move the mouse left
	{
		xoff = originW - xPos;
		xoff /= deg_per_pixel;

		RotateY(xoff * -kDegInc * dt); 
	}
	else if(xPos > (originW + kCushion)) // If they move the mouse right
	{
		xoff = xPos - originW;
		xoff /= deg_per_pixel;

		RotateY(xoff * kDegInc * dt);
	}

	const float up_pitch = 2.62f;
	const float bottom_pitch = 1.2f;

	float pitch = GetPitch();

	if( (mIsFlying || (!mIsFlying && pitch < up_pitch)) && yPos < (originH - kCushion)) // If they move the mouse up
	{
		// If they haven't pitched up 88°, go ahead and pitch them up
		{
			yoff = (originH) - yPos;
			yoff /= deg_per_pixel;

			Pitch(yoff * kDegInc * dt);
		}
	}
	else if( (mIsFlying || (!mIsFlying && pitch > bottom_pitch)) && yPos > (originH + kCushion)) // If they move the mouse down
	{
		// If they haven't pitched down 88°, go ahead and pitch them down
		{
			yoff = yPos - (originH);
			yoff /= deg_per_pixel;

			Pitch(yoff * -kDegInc * dt);
		}
	}

}


void WCamera::SetCameraMode(CameraMode mode)
{
	m_CameraMode=mode;
}

void WCamera::UpdateWorldScaleOffset()
{
}


} //namespace