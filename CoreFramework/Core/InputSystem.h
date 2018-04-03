/* ===========================================================
	Input System - routes input events

	Created Feb 1,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_INPUT_SYSTEM_H_)
#define _INPUT_SYSTEM_H_

#include "GenericObject.h"
#include "GenericSingleton.h"
#include "GenericNode.h"
#include "Godz.h"

namespace GODZ
{
	class InputSystem;

	//http://msdn.microsoft.com/en-us/library/ms927178.aspx - virtual key codes
	typedef int VirtualKeyCode;

	//KeyActions - Defines a set of generic actions key presses can be translated to
	//NOTE: KeyActions should really just be hash codes that we can easily reference from XML
	enum EKeyAction
	{
		KA_Unknown		= 0,
		KA_MoveUp,
		KA_MoveDown,
		KA_MoveLeft,
		KA_MoveRight,
		KA_Jump,
		KA_Crouch,
		KA_ActivateEd,				//view/hide editor
		KA_RotateLeft,				//camera rotate left
		KA_RotateRight,
		KA_PitchUp,					//camera pitch up
		KA_PitchDown,
		KA_MoveCamDown,				//debug camera - movement
		KA_TakeSnapshotShadowMap,
		KA_DebugFirstPassOnly,
		KA_SelectNextCamera,



		KA_MaxActions				//max # of key actions
		
	};

	enum EventTypes
	{
		ET_KEY_INPUT = 0x00001,
		ET_MOUSE_INPUT = 0x00002
	};

	struct KeyMap
	{
		VirtualKeyCode	m_kCode;
		rstring			m_sKeyName;

		KeyMap(const char* name, VirtualKeyCode kCode)
			: m_kCode(kCode)
			, m_sKeyName(name)
		{
		}
	};

	enum EInputCode
	{
		IC_Unknown,
		IC_KeyPressed,
		IC_LeftMClick,		//left mouse click
		IC_RightMClick,
		IC_MiddleMClick,
		IC_MouseWheel
	};

	enum EKeyState 
	{
		KS_Unknown,
		KS_KeyUp,				//default state
		KS_KeyDown,
		KS_KeyJustReleased		//user just let go of this key
	};

	struct VKeyState
	{
		VirtualKeyCode		m_key;
		EKeyState			m_state;
		EKeyAction			m_kAction;

		VKeyState()
			: m_key(-1)
			, m_state(KS_Unknown)
			, m_kAction(KA_Unknown)
		{
		}

		VKeyState(VirtualKeyCode key, EKeyState state)
			: m_key(key)
			, m_state(state)
			, m_kAction(KA_Unknown)
		{
		}
	};

	//this is an input action we "received" from the hardware
	struct GODZ_API InputEvent
	{
		VKeyState				m_keyState;
		float					m_fDelta;
		EInputCode				m_kType;
		int						m_X;
		int						m_Y;

		InputEvent(VirtualKeyCode key, EKeyState keyState);
		InputEvent(EInputCode input, VKeyState keyState, int x, int y);
		InputEvent(float delta);
		InputEvent(EInputCode kType);
	};

	//this is an input action we are "broadcasting" to listeners
	struct GODZ_API InputBroadcast
	{
		VKeyState				m_keyState;
		float					m_fDelta;
		EInputCode				m_kType;
		std::vector<VKeyState>	m_keyStates;						//state for all keys that are currently held down
		EKeyState				m_keyActionState[KA_MaxActions];
		int						m_X;
		int						m_Y;

		InputBroadcast()
			: m_kType(IC_Unknown) 
		{}

		InputBroadcast(const InputEvent& event, InputSystem *input);
		bool IsKeyPressed() const;
		bool					IsKeyJustReleased(EKeyAction kAction) const;
		bool					IsKeyActionActive(EKeyAction keyAction) const;
		EKeyState				GetKeyState(VirtualKeyCode key) const;

		//this can be called after the receiver has processed the event
		void					Reset() { m_kType = IC_Unknown; }
	};

	//Input Listener Interface
	class GODZ_API InputListener
	{
	public:
		virtual void ReceiveInputEvent(const InputBroadcast &inputEvent)=0;
	};

	//Input System
	class GODZ_API InputSystem : public GenericSingleton<InputSystem>
	{
		friend struct InputBroadcast;

	public:
		InputSystem();
		virtual ~InputSystem();

		void AddInputListener(InputListener *Listener);	
		void RemoveListener(InputListener *Listener);

		void PublishInputEvent(InputEvent &inputEvent);

		//changes all key_released states to keyup
		void ResetStates();
		void SetKeyState(VirtualKeyCode key, EKeyState kState);

		//notification to the input system to broadcast all the buffered events
		void BroadcastEvents();


	protected:
		//called on startup - loads key maps
		void LoadKeyMaps();

		//should be called anytime a new keybind is made-  binds actions to keys
		void LoadKeyActions();

	private:
		void					CopyInputBuffer();
		EKeyState				GetVKeyState(VirtualKeyCode key);
		bool					IsKeyActionActive(EKeyAction kAction);
		bool					IsKeyJustReleased(EKeyAction kAction);

		//get EKeyAction associated with the string
		EKeyAction				GetKeyAction(const char* keyName);
		EKeyAction				GetKeyAction(VirtualKeyCode key);
		void					SetKeyAction(VirtualKeyCode key, EKeyAction kAction);



		std::vector<InputListener*>		listeners;
		NodeList<VirtualKeyCode>		m_act[KA_MaxActions];			//stored at each key action - a list of keys that are bound to this action
		NodeList<KeyMap*>				m_keymaps;						//predefined list that maps a virtual key code to a string (name of key) 
		std::vector<VKeyState>			m_keyState;						//indicates if key is down
		std::vector<InputBroadcast>		m_inputBuffer;					//stores all the messages in a queue

		//HANDLE							m_lockInputBuffer;
		std::vector<InputBroadcast>		m_inputBufferCopy;				//thread safe
	};
}

#endif //_INPUT_SYSTEM_H_