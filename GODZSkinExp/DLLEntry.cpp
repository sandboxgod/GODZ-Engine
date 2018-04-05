
#include "GODZSkinExp.h"

#pragma comment(lib, "comctl32.lib") //required so we can use Win32 controls

// ---------------------------------------------------------
// Define globals
// ---------------------------------------------------------
HINSTANCE g_hInstance;
bool g_bControlsInit=0;

//DLL Loading, etc
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
    g_hInstance = hinstDLL;                // Hang on to this DLL's instance handle.

    if (!g_bControlsInit) {
        g_bControlsInit = TRUE;
        InitCustomControls(g_hInstance);    // Initialize MAX's custom controls
        InitCommonControls();            // Initialize Win95 controls
    }
            
    return (TRUE);
}

// ---------------------------------------------------------
// 3D Studio MAX Interfaces
// ---------------------------------------------------------


// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR* LibDescription()
{
	return "http://www.planetunreal.com/godz";
}

// This function returns the number of plug-in classes this DLL
//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
    return 1;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
    switch(i) {
		case 0: 
			return GODZ::GetGODZSkinExpDesc();
        default: 
			return 0;
    }
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
   return 1;
}