
#include "WindowManager.h"
#include "Win32Interface.h"

using namespace GODZ;

//-------------------------------------------------------------
//global member decl
//-------------------------------------------------------------

// Reference to window
IWindow* WindowManager::m_objWindow = NULL;

IWindow* WindowManager::GetInstance()
{
	if (m_objWindow==NULL)
	{
		//TODO: Once engine ported to other systems they can instantiate their
		//own type.
		m_objWindow=new Win32Interface();
		godzassert(m_objWindow != NULL);
	}

	return m_objWindow;
}

void WindowManager::Release()
{
	if (m_objWindow != NULL)
	{
		m_objWindow->Release();
		delete m_objWindow;
		m_objWindow=NULL;
	}
}
