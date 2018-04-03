/* ===========================================================
	Window Manager

	Delegates which window interface to use.

	Created Apr 3,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WINDOW_MANAGER_H_)
#define _WINDOW_MANAGER_H_

#include "IWindow.h"

namespace GODZ
{
	class GODZ_API WindowManager
	{
	public:
		//Returns a window interface
		static IWindow* GetInstance();

		//Destroys the global window interface (reference to application window)
		static void Release();

	private:
		static IWindow* m_objWindow;
	};
}

#endif