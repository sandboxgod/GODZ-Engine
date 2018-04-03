/* =======================================================
    Real Time 3D Terrain Engines Using C++ and DirectX

    by Greg Snook
    greg@mightystudios.com
===========================================================*/
#if !defined(_DEBUG_H_)
#define _DEBUG_H_

#include "CoreFramework.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

namespace GODZ
{

typedef signed long     int32;
typedef signed short    int16;
typedef signed char     int8;
typedef unsigned long   uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

// unicode vs. mbcs character types
#ifdef _UNICODE
	// define tchar as a wide-character
	typedef wchar_t tchar;
	// create a macro to build string literals
	#define _text(a) L ## a
#else
	// define tchar as a plain char
	typedef char tchar;
	// stub the macro to build string literals
	#define _text
#endif

// Assert function return values
enum ERROR_RESULT
{
	VR_IGNORE = 0,
	VR_CONTINUE,
	VR_BREAKPOINT,
	VR_ABORT
};

ERROR_RESULT GODZ_API notifyAssertion(const tchar* condition, const tchar* description, const tchar* fileName, int lineNumber);

// Private Functions...
ERROR_RESULT GODZ_API displayError(	const tchar* errorTitle,
							const tchar* errorText,
							const tchar* errorDescription,
							const tchar* fileName, 
							int lineNumber);

#define debug_assert(x, comment) {\
		static bool _ignoreAssert = false;\
		if (!_ignoreAssert && !(x)) \
		{\
			ERROR_RESULT _err_result = notifyAssertion(_text(#x), _text(comment), __FILE__, __LINE__);\
			if (_err_result == VR_IGNORE)\
			{\
				_ignoreAssert = true;\
			}\
			else if (_err_result == VR_BREAKPOINT)\
			{\
				_asm{int 3};\
			}\
		}}

}
#endif
