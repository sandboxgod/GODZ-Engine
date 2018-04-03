/* ===================================================================
	Win32 Display

	Copyright (c) 2010
	==================================================================
*/

#include <shlobj.h>			//shell browser
#include <windows.h>
#include <mmsystem.h>

#if !defined(_WINDOW_INTERFACE_H)
#define _WINDOW_INTERFACE_H

#include "Godz.h"
#include "GenericSingleton.h"
#include "WindowManager.h"
#include "RString.h"

namespace GODZ
{
	//foward declarations
	class SceneManager;

	class GODZ_API Win32Interface : public IWindow
	{
		friend class WindowManager;

	public:

		~Win32Interface();

		virtual void Release();

		void CenterCursor();

		//Centers the mouse cursor within the window.
		void CenterCursor(HWND hwnd, int width, int height);

		//Returns the Scene Manager
		SceneManager* GetSceneManager();

		//Returns width of this application
		int GetWidth();

		//Returns height of this application
		int GetHeight();

		HWND GetHWND() { return mHWnd; }

		virtual void GetMousePos(int &x, int &y);

		bool IsMouseHidden();

		//Displays a folder selection control
		rstring FolderSelect(bool bDisplayFiles=false);
	
		//Displays a File Save Control
		rstring FileSaveAs(HWND hWnd=0, char *fileExt="gmz", char *filter="GODZ Model (*.gmz)\0*.gmz");

		//Displays the Window
		virtual void Present();

		//Registers the UI Window (window is created but not displayed)
		virtual void RegisterWindow(const char* name, int width, int height);
		virtual void RegisterWindow(const char* name);

		//Window waits for UI messages
		bool Run();

		//Call this function to freeze drawing ops
		void SetActive(bool bActive);

		//Call this function to set the window handle if the application window
		//was not created via RegisterWindow().
		void SetWindowClass(HWND hwnd);
		void ShowMouseCursor(bool bShowCursor);	

		void SetMousePos(int x, int y);

		virtual bool IsRightMouseButtonDown() { return mIsRightMouseButtonDown; }
		
	private:
		rstring				WindowTitle;	//window title
		HWND				mHWnd;				//window handle
		int					m_width;
		int					m_height;		//width, height - window dimensions

		int					m_mouseX;		//current mouse position (thread safe)
		int					m_mouseY;		//current mouse position (thread safe)
		HANDLE				m_mutex;

		bool					m_bHideMouse;
		bool					m_bMinimized;
		bool					m_bActive;
		bool					bWindowVisible;
		bool					m_bHasMouseWheel;
		bool					mIsWindowed;

		//TODO: Move out to input system, etc
		bool					mIsRightMouseButtonDown;
		
		Win32Interface();

		//Adds an item to the popup menu
		void AddPopupMenuItem(HMENU menu, const char *text, bool bEnabled=true);

		void Pause( bool bPause );

		//Adds a camera to the scene.
		void SetupScene();

		void UpdateWindowStatus(int status);	

		//Window event handler
		static LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

		void InitSettings();
	};	

	//Centers the mouse cursor within the window.
	//void CenterCursor(HWND hwnd, int width, int height);

	//Displays a Load File Control
	rstring FileLoad(HWND hWnd=0, const char* fileExt="GODZ File (*.gmz)", const char *defaultExt="gmz", const char* title = "Open");
}

#endif
