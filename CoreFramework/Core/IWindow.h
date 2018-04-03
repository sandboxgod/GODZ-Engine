/* ===========================================================
	IWindow

	Window interface

	Created Apr 3,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_IWINDOW_H_)
#define _IWINDOW_H_

#include "Godz.h"
#include "Windows.h"

namespace GODZ
{
	//forward decl
	class GenericObject;

	//Interface to application window
	class GODZ_API IWindow
	{
	public:
		virtual ~IWindow() {}

		virtual void CenterCursor()=0;

		virtual void CenterCursor(HWND hwnd, int width, int height)=0;

		//Returns width of this application
		virtual int GetWidth()=0;

		//Returns height of this application
		virtual int GetHeight()=0;

		//returns mouse location
		virtual void GetMousePos(int &x, int &y)=0;

		//Returns true if the mouse is hidden
		virtual bool IsMouseHidden()=0;
		virtual bool IsRightMouseButtonDown() = 0;

		//Displays a folder selection control
		virtual rstring FolderSelect(bool bDisplayFiles=false)=0;
	
		//Displays a File Save Control
		virtual rstring FileSaveAs(HWND hWnd=0, char *fileExt="gmz", char *filter="GODZ Model (*.gmz)\0*.gmz")=0;

		//Displays the Window
		virtual void Present()=0;
		virtual void Release()=0;

		//Registers the UI Window (window is created but not displayed)
		virtual void RegisterWindow(const char* name, int width, int height)=0;
		virtual void RegisterWindow(const char* name)=0;

		//Window waits for UI messages
		virtual bool Run()=0;

		//Call this function to freeze drawing ops
		virtual void SetActive(bool bActive)=0;

		//Displays the mouse cursor
		virtual void ShowMouseCursor(bool bShowCursor)=0;

		virtual void SetMousePos(int x, int y) = 0;
	};
}

#endif