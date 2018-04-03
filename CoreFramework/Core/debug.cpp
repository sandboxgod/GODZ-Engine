
#include "debug.h"
#include "Godz.h"
#include <windows.h>

using namespace GODZ;

ERROR_RESULT GODZ::notifyAssertion(const tchar* condition, const tchar* description, const tchar* fileName, int lineNumber)
{
	// pass the data on to the message box
	ERROR_RESULT result = displayError(_text("Assert Failed!"),
										condition,
										description,
										fileName,
										lineNumber);

	return(result);

}

ERROR_RESULT GODZ::displayError(const tchar* errorTitle,
										  const tchar* errorText,
										  const tchar* errorDescription,
										  const tchar* fileName, 
										  int lineNumber)
{
	const	int		MODULE_NAME_SIZE = 255;
	tchar moduleName[MODULE_NAME_SIZE];

	// attempt to get the module name
    if (!GetModuleFileName(NULL, moduleName, MODULE_NAME_SIZE))
    {
		GODZ::StringCopy(moduleName, _text("<unknown application>"), MODULE_NAME_SIZE);
    }

	// if stack tracing is enabled, build a string containing the 
	// unwound stack information
#ifdef _STACKTRACE
		const	int		STACK_STRING_SIZE = 255;
		tchar stackText[STACK_STRING_SIZE];

		buildStackTrace(stackText, STACK_STRING_SIZE, 2);
#else
		tchar stackText[] = _text("<stack trace disabled>");
#endif

	// build a collosal string containing the entire asster message
	const	int		MAX_BUFFER_SIZE = 1024;
	tchar	buffer[MAX_BUFFER_SIZE];

	int Size = _snprintf_s(	buffer, 
							MAX_BUFFER_SIZE,
							MAX_BUFFER_SIZE,
							_text(	"%s\n\n"						\
									 "Program : %s\n"               \
									 "File : %s\n"                  \
									 "Line : %d\n"                  \
									 "Error: %s\n"			\
									 "Comment: %s\n"						\
									 "\nStack:\n%s\n\n"						\
									 "Abort to exit (or debug), Retry to continue,\n"\
									 "Ignore to disregard all occurances of this error\n"),
							errorTitle,
							moduleName,
							fileName,
							lineNumber,
							errorText,
							errorDescription,
							stackText
							);


	// place a copy of the message into the clipboard
	if (OpenClipboard(NULL))
	{
		uint32 bufferLength = (uint32)_tcsclen(buffer);
		HGLOBAL hMem = GlobalAlloc(GHND|GMEM_DDESHARE, bufferLength+1);

		if (hMem)
		{
			uint8* pMem = (uint8*)GlobalLock(hMem);
			memcpy(pMem, buffer, bufferLength);
			GlobalUnlock(hMem);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hMem);
		}

		CloseClipboard();
	}


	// find the top most window of the current application
	HWND hWndParent = GetActiveWindow ( ) ;
	if ( NULL != hWndParent )
	{
		hWndParent = GetLastActivePopup ( hWndParent ) ;
	}

	// put up a message box with the error
	int iRet = MessageBox ( hWndParent,
							buffer,
							_text ( "ERROR NOTIFICATION..." ),
							MB_TASKMODAL
							|MB_SETFOREGROUND
							|MB_ABORTRETRYIGNORE
							|MB_ICONERROR);

	// Figure out what to do on the return.
	if (iRet == IDRETRY)
	{
		// ignore this error and continue
		return (VR_CONTINUE);
	}
	if (iRet == IDIGNORE)
	{
		// ignore this error and continue,
		// plus never stop on this error again (handled by the caller)
		return (VR_IGNORE);
	}

	// The return has to be IDABORT, but does the user want to enter the debugger
	// or just exit the application?
	iRet = MessageBox ( hWndParent,
						"Do you wish to debug the last error?",
						_text ( "DEBUG OR EXIT?" ),
						MB_TASKMODAL
						|MB_SETFOREGROUND
						|MB_YESNO
						|MB_ICONQUESTION);

	if (iRet == IDYES)
	{
		// inform the caller to break on the current line of execution
		return (VR_BREAKPOINT);
	}

	// must be a full-on termination of the app
	ExitProcess ( (UINT)-1 ) ;
	return (VR_ABORT);
}
