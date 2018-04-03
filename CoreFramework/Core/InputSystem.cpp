
#include "InputSystem.h"
#include "Windows.h"

using namespace std;

namespace GODZ
{


InputEvent::InputEvent(VirtualKeyCode key, EKeyState keyState)
{
	m_kType = IC_KeyPressed;
	m_keyState.m_state = keyState;
	m_keyState.m_key = key;
}

InputEvent::InputEvent(float delta)
{
	m_kType = IC_MouseWheel;
	m_fDelta = delta;
}


InputEvent::InputEvent(EInputCode input, VKeyState keyState, int x, int y)
: m_kType(input)
, m_keyState(keyState)
, m_X(x)
, m_Y(y)
{
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
bool InputBroadcast::IsKeyPressed() const
{
	return m_kType > IC_Unknown;
}

bool InputBroadcast::IsKeyActionActive(EKeyAction keyAction) const
{
	return m_keyActionState[keyAction] == KS_KeyDown;
}

bool InputBroadcast::IsKeyJustReleased(EKeyAction keyAction) const
{
	return m_keyActionState[keyAction] == KS_KeyJustReleased;
}

EKeyState InputBroadcast::GetKeyState(VirtualKeyCode key) const
{
	EKeyState keyState = KS_Unknown;
	std::vector<VKeyState>::const_iterator iter;
	for(iter = m_keyStates.begin(); iter != m_keyStates.end(); ++iter)
	{
		if ( (*iter).m_key == key)
		{
			keyState = (*iter).m_state;
			break;
		}
	}

	return keyState;
}


InputBroadcast::InputBroadcast(const InputEvent& event, InputSystem* input)
{
	
	m_kType = event.m_kType;

	//attach the state of all keys currnetly pressed if this a key event
	switch(event.m_kType)
	{
	default:
		break;

	case IC_MiddleMClick:
	case IC_RightMClick:
	case IC_LeftMClick:
		{
			m_keyState.m_key = event.m_keyState.m_key;
			m_keyState.m_state = event.m_keyState.m_state;

			m_X = event.m_X;
			m_Y = event.m_Y;
		}
		break;

	case IC_MouseWheel:
		{
			m_fDelta = event.m_fDelta;
		}
		break;
	case IC_KeyPressed:
		{
			m_keyState.m_key = event.m_keyState.m_key;
			m_keyState.m_state = event.m_keyState.m_state;
			//m_keyStates = event.m_keyState.m_state;

			//set the key action- inputEvents have no way to set this directly
			m_keyState.m_kAction =  input->GetKeyAction(event.m_keyState.m_key);

			//set keyActions array
 			for(unsigned int i=0; i < KA_MaxActions;i++)
			{
				EKeyAction keyAction = (EKeyAction)i;
				m_keyActionState[i] = KS_KeyUp;

				if (input->IsKeyActionActive(keyAction))
				{
					m_keyActionState[i] = KS_KeyDown;
				}
				

				else if (input->IsKeyJustReleased(keyAction))
				{
					m_keyActionState[i] = KS_KeyJustReleased;
				}
			}
		}		
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
InputSystem::InputSystem()
: GenericSingleton<InputSystem>(*this)
{
	//m_lockInputBuffer = CreateMutex (NULL, FALSE, NULL);


	LoadKeyMaps();
	LoadKeyActions();
}

InputSystem::~InputSystem()
{
	for(GenericNode<KeyMap*> *pNode=m_keymaps.GetFirst(); pNode; pNode=pNode->next)
	{
		delete pNode->data;
	}

	m_keymaps.Clear();
	listeners.clear();
}

void InputSystem::LoadKeyActions()
{
	//initialize	
	for(UINT i=0;i<KA_MaxActions;i++)
	{
		m_act[i].Clear();
	}

	/*
	for(UINT i=0; i<VC_MAX; i++)
	{
		m_kActions[i] = KA_Unknown;
		m_keyState[i] = KS_KeyUp;
	}*/

	for(GenericNode<KeyMap*> *pNode=m_keymaps.GetFirst(); pNode; pNode=pNode->next)
	{
		KeyMap* pKey = pNode->data;
		const char* keyName = GlobalConfig::m_pConfig->ReadNode("KeyActions",pKey->m_sKeyName);
		if (keyName)
		{
			EKeyAction kAct = GetKeyAction(keyName);
			SetKeyAction(pKey->m_kCode, kAct);
			//m_kActions[pKey->m_kCode]=(kAct);

			//store the list of keys that are bound to this action
			m_act[kAct].Push(pKey->m_kCode); 
		}
	}
}

void InputSystem::LoadKeyMaps()
{
	//arrow keys
	m_keymaps.Push(new KeyMap("Key_ArrowUp",VK_UP));
	m_keymaps.Push(new KeyMap("Key_ArrowDown",VK_DOWN));
	m_keymaps.Push(new KeyMap("Key_ArrowLeft",VK_LEFT));
	m_keymaps.Push(new KeyMap("Key_ArrowRight",VK_RIGHT));

	//keyboard
	m_keymaps.Push(new KeyMap("Key_Spacebar",VK_SPACE));	
	m_keymaps.Push(new KeyMap("Key_Ctrl",VK_CONTROL));

	//rest of the keyboard inputs
	m_keymaps.Push(new KeyMap("Key_A",'A'));
	m_keymaps.Push(new KeyMap("Key_B",'B'));
	m_keymaps.Push(new KeyMap("Key_C",'C'));
	m_keymaps.Push(new KeyMap("Key_D",'D'));
	m_keymaps.Push(new KeyMap("Key_E",'E'));
	m_keymaps.Push(new KeyMap("Key_F",'F'));
	m_keymaps.Push(new KeyMap("Key_G",'G'));
	m_keymaps.Push(new KeyMap("Key_H",'H'));
	m_keymaps.Push(new KeyMap("Key_I",'I'));
	m_keymaps.Push(new KeyMap("Key_J",'J'));
	m_keymaps.Push(new KeyMap("Key_K",'K'));
	m_keymaps.Push(new KeyMap("Key_L",'L'));
	m_keymaps.Push(new KeyMap("Key_M",'M'));
	m_keymaps.Push(new KeyMap("Key_N",'N'));
	m_keymaps.Push(new KeyMap("Key_O",'O'));
	m_keymaps.Push(new KeyMap("Key_P",'P'));
	m_keymaps.Push(new KeyMap("Key_Q",'Q'));
	m_keymaps.Push(new KeyMap("Key_R",'R'));
	m_keymaps.Push(new KeyMap("Key_S",'S'));
	m_keymaps.Push(new KeyMap("Key_T",'T'));
	m_keymaps.Push(new KeyMap("Key_U",'U'));
	m_keymaps.Push(new KeyMap("Key_V",'V'));
	m_keymaps.Push(new KeyMap("Key_W",'W'));
	m_keymaps.Push(new KeyMap("Key_X",'X'));
	m_keymaps.Push(new KeyMap("Key_Y",'Y'));
	m_keymaps.Push(new KeyMap("Key_Z",'Z'));

	//TODO: Add rest of keyboard inputs like F1 - F12, etc
}

EKeyAction InputSystem::GetKeyAction(const char* keyName)
{
	//yes I know string compares are slow but this only happens at engine start
	if (_stricmp(keyName,"MoveUp") == 0)
	{
		return KA_MoveUp;
	}
	else if (_stricmp(keyName,"MoveDown") == 0)
	{
		return KA_MoveDown;
	}
	else if (_stricmp(keyName,"MoveLeft") == 0)
	{
		return KA_MoveLeft;
	}
	else if (_stricmp(keyName,"MoveRight") == 0)
	{
		return KA_MoveRight;
	}
	else if (_stricmp(keyName,"Jump") == 0)
	{
		return KA_Jump;
	}
	else if (_stricmp(keyName,"Crouch") == 0)
	{
		return KA_Crouch;
	} //
	else if (_stricmp(keyName,"ViewEditor") == 0)
	{
		return KA_ActivateEd;
	} //
	else if (_stricmp(keyName,"TakeSnapshotShadowMap") == 0)
	{
		return KA_TakeSnapshotShadowMap;
	} //
	else if (_stricmp(keyName,"DebugFirstPassOnly") == 0)
	{
		return KA_DebugFirstPassOnly;
	}
	else if (_stricmp(keyName,"RotateRight") == 0)
	{
		return KA_RotateRight;
	}
	else if (_stricmp(keyName,"RotateLeft") == 0)
	{
		return KA_RotateLeft;
	}
	else if (_stricmp(keyName,"PitchUp") == 0)
	{
		return KA_PitchUp;
	}
	else if (_stricmp(keyName,"PitchDown") == 0)
	{
		return KA_PitchDown;
	}
	else if (_stricmp(keyName,"MoveCamDown") == 0)
	{
		return KA_MoveCamDown;
	}
	else if (_stricmp(keyName,"SelectNextCamera") == 0)
	{
		return KA_SelectNextCamera;
	}

	return KA_Unknown;
}

void InputSystem::AddInputListener(InputListener *listener)
{
	listeners.push_back(listener);
}

void InputSystem::PublishInputEvent(InputEvent &event)
{
	//figure out the key action (we assume the calling thread did not specify the key action).....
	InputBroadcast broadcastMsg(event, this);

	//GodzMutex mutex(m_lockInputBuffer);
	m_inputBuffer.push_back(broadcastMsg);
}

//TODO: ?? What's going on in here???? this is single threaded gameplay logic stuff
void InputSystem::CopyInputBuffer()
{
	//copy main input buffer into the copy. This way the InputSystem can proceed on after this
	//transaction is complete
	//GodzMutex mutex(m_lockInputBuffer);
	m_inputBufferCopy = m_inputBuffer;
	m_inputBuffer.clear();
}

//Called by a worker thread when they desire to receive the input events. It is assumed all of these events
//will be broadcast to "workers" in the same thread as the caller (hopefully)
void InputSystem::BroadcastEvents()
{
	CopyInputBuffer();	//update our thread safe buffer. 

	//broadcast all of our buffered events
	vector<InputBroadcast>::iterator iter;
	for(iter=m_inputBufferCopy.begin();iter!=m_inputBufferCopy.end();++iter)
	{
		vector<InputListener*>::iterator list;
		for(list=listeners.begin();list!=listeners.end();++list)
		{
			InputListener* listener = *list;
			listener->ReceiveInputEvent(*iter);
		}
	}
}

void InputSystem::ResetStates()
{
	vector<VKeyState>::iterator inputIter;
	for(inputIter=m_keyState.begin();inputIter!=m_keyState.end();++inputIter)
	{
		VKeyState &myState = (*inputIter);
		if (myState.m_state == KS_KeyJustReleased)
		{
			myState.m_state = KS_KeyUp;
		}
	}
}

void InputSystem::RemoveListener(InputListener *Listener)
{
	vector<InputListener*>::iterator inputIter;
	for(inputIter=listeners.begin();inputIter!=listeners.end();++inputIter)
	{
		InputListener* thisListener = *inputIter;
		if (thisListener==Listener)
		{
			listeners.erase(inputIter);
			return;
		}
	}
}

void InputSystem::SetKeyAction(VirtualKeyCode key, EKeyAction kAction)
{
	//m_keyState[key] = kState;
	vector<VKeyState>::iterator inputIter;
	for(inputIter=m_keyState.begin();inputIter!=m_keyState.end();++inputIter)
	{
		VKeyState& myState = (*inputIter);
		if (myState.m_key == key)
		{
			godzassert(0); //we do not support binding more than one keyaction to a key atm
		}
	}

	//did not find the key so add it. 
	VKeyState newKey;
	newKey.m_key=key;
	newKey.m_state = KS_Unknown;
	newKey.m_kAction = kAction;
	m_keyState.push_back(newKey);
}

void InputSystem::SetKeyState(VirtualKeyCode key, EKeyState kState)
{
	//m_keyState[key] = kState;
	vector<VKeyState>::iterator inputIter;
	for(inputIter=m_keyState.begin();inputIter!=m_keyState.end();++inputIter)
	{
		VKeyState &myState = (*inputIter);
		if (myState.m_key == key)
		{
			myState.m_state = kState;
			return;
		}
	}

	//did not find the key so add it
	VKeyState newKey;
	newKey.m_key=key;
	newKey.m_state = kState;
	newKey.m_kAction = KA_Unknown;
	m_keyState.push_back(newKey);
}

//i could have used the win32 GetKeyState() but wrote my own for new states like key_just_released
EKeyState InputSystem::GetVKeyState(VirtualKeyCode key)
{
	vector<VKeyState>::iterator inputIter;
	for(inputIter=m_keyState.begin();inputIter!=m_keyState.end();++inputIter)
	{
		VKeyState &myState = (*inputIter);
		if (myState.m_key == key)
		{
			return myState.m_state;
		}
	}

	return KS_Unknown;
}

EKeyAction InputSystem::GetKeyAction(VirtualKeyCode key)
{
	vector<VKeyState>::iterator inputIter;
	for(inputIter=m_keyState.begin();inputIter!=m_keyState.end();++inputIter)
	{
		VKeyState &myState = (*inputIter);
		if (myState.m_key == key)
		{
			return myState.m_kAction;
		}
	}

	return KA_Unknown;
}

bool InputSystem::IsKeyActionActive(EKeyAction kAction)
{
	for(GenericNode<VirtualKeyCode> *pNode=m_act[kAction].GetFirst();pNode;pNode=pNode->next)
	{
		if (GetVKeyState(pNode->data) == KS_KeyDown)
		{
			return true;
		}
	}

	return 0;
}


bool InputSystem::IsKeyJustReleased(EKeyAction kAction)
{
	for(GenericNode<VirtualKeyCode> *pNode=m_act[kAction].GetFirst();pNode;pNode=pNode->next)
	{
		if (GetVKeyState(pNode->data) == KS_KeyJustReleased)
		{
			return true;
		}
	}

	return 0;
}



} //namespace GODZ

