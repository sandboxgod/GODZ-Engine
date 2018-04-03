/* ===========================================================
	DeviceCreation.h

	Created May 8,'07 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__DEVICECREATION_H__)
#define __DEVICECREATION_H__

#include "Godz.h"
#include <windows.h>


namespace GODZ
{
	class ResourceManager;
	class Renderer;

	struct GODZ_API DeviceCreation
	{
		DeviceCreation()
			: m_window(0)
			, m_deviceWindow(0)
			, m_pRenderDeviceResMngr(NULL)
			, m_renderer(NULL)
			, width(0)
			, height(0)
			, m_isEditor(false)
		{}

		HWND				m_window;
		HWND				m_deviceWindow;
		udword				width;
		udword				height;
		ResourceManager*	m_pRenderDeviceResMngr;
		Renderer*			m_renderer;
		bool				m_isEditor;
	};
}

#endif