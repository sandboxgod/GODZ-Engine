/* ===========================================================
	World Space Camera that is controlled by a Player

	2 Chronicles 7:14
	"If my people, who are called by my name, will humble 
	themselves and pray and seek my face and turn from their
	wicked ways, then will I hear from heaven and will forgive
	their sin and will heal their land."

	Notes on creating a view matrix:
	[1] http://www.gamedev.net/community/forums/topic.asp?topic_id=253682&whichpage=1&#1531224
	[2] D3DXMatrixLookAtLH - see MSDN for DirectX 9.0

	Created Jan 2,'04 by Richard Osborne
	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(_WCAMERA_H_)
#define _WCAMERA_H_

#include "WEntity.h"
#include "CameraInfo.h"
#include "Timer.h"
#include "Inputsystem.h"
#include "atomic_ref.h"
#include <CoreFramework/Math/WMatrix3.h>
#include <CoreFramework/Math/ViewFrustum.h>

namespace GODZ
{
	class WPlayerController;


	class GODZ_API WCamera : public WEntity
	{
		DeclareGeneric(WCamera, WEntity)

	public:
		enum CameraMode
		{
			CT_FPS,			//first person camera...
			CT_BehindView	//3rd person - rotates the view target and forwards input events
		};

		enum CameraState
		{
			CameraState_Normal,
			CameraState_Transition			//transition to pos behind character
		};

		//hardcoded, characters are rotated to face the world (instead of FRONT VIEW like in 3dsmax)
		static const float CHARACTER_ROT_OFFSET;

	public:
		WCamera();
		~WCamera();

		CameraID			GetCameraID();

		//Updates camera perspective to follow character
		void				CalcBehindView(float dt);

		//returns camera speed
		float				GetSpeed();

		//sets camera speed
		void				SetSpeed(float speed);

		//Returns the camera's view target. If null is returned, the focus is the world.
		WEntity*			GetFocus();

		//aka lookAt vector
		virtual Vector3		GetTarget();

		//returns the target where this camera is looking at
		virtual Vector3		GetEye();

		//Returns camera perspective
		CameraMode			GetCameraMode();

		const WMatrix&		GetViewMatrix();

		virtual ECameraType GetCameraType() { return CameraType_Main; }

		//Camera/Character rotation speed
		float				GetRotationSpeed();

		virtual void		OnTick(float dt);
		virtual void		OnCameraTick(float dt);

		//Notification this entity has been removed from the level
		virtual void		OnRemoved();

		// Rotates around world Y-axis (Heading)
		void				RotateY(float angle);

		// Pitches camera by "angle"
		void				Pitch(float angle, bool applyPitchViewTarget = true);
		float				GetPitch();
		void				ComputeUpVector(Vector3& up);

		//Notification user has pressed keyboard
		void				ReceiveInputEvent(const InputBroadcast& event);

		//sets camera mode
		void				SetCameraMode(CameraMode mode);
		void				SetMouseLook(bool bLook) { m_bMouseLook = bLook; }
		bool				GetMouseLook() { return m_bMouseLook; }


		//view update -- origin (x,y) is the center of the screen. xPos / yPos is the mouse position
		void				UpdateView(int xPos, int YPos, int originX, int originY, float dt);
		void				UpdateCamera(float dt);

		//[in] combMat - concatenated View * projection matrix
		void				UpdateFrustum(const WMatrix& combMat);
		ViewFrustum&		GetViewFrustum() { return m_frustum; }
				
		//sets the view target
		void				SetFocus(WEntity* entity);
		void				ResetFocus();

		void				ResetSavedPitchHeadingToViewTarget();
		void				AddSavedPitchHeadingToViewTarget();

		//Returns true if this entity should display an icon in the Editor
		virtual bool			IsIconVisible() { return false; }

	protected:
		virtual void		UpdateWorldScaleOffset();
		void				PushCameraInfo(const Vector3& eye, const Vector3& target);
		void				SetViewTargetRotation();


		bool						m_bMouseLook;
		atomic_weak_ptr<WEntity>	m_pViewTarget;			//entity that has focus


		WMatrix					m_viewMatrix;
		ViewFrustum				m_frustum;
		bool					m_bUpdateCamera;
		WPlayerController*		m_playerController;				//listener attached to the camera
		
		InputBroadcast			m_lastEvent;			//last keyboard event
		
		float					flyspeed;
		float					m_fRotateSpeed;

		////////////////mouse positions
		Point					m_mousePos;

		//non-uniform view distance from model (3rd person mode). Note, to achieve
		//a nice smooth rotation, this vector should be kept uniform.
		Vector3					m_viewDist;
		float					m_cameradist;

		float					m_transitionSpeed;
		float					m_lookAtOffset;
		float					mLerpTime;

		CameraMode				m_CameraMode;				//camera mode
		CameraState				m_state;
		

		CameraInfo mCameraInfo;

		bool					mIsFlying;	//set if the viewtarget is flying...

		WMatrix3				m_offsetMatrix;
		Quaternion				m_vanityQ;
	};
}

#endif