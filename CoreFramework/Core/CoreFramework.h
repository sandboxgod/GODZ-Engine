

#if !defined(_CORE_FRAMEWORK_H_)
#define _CORE_FRAMEWORK_H_

#ifdef GODZEXPORTS
#define GODZ_API __declspec(dllexport)
#else
#define GODZ_API __declspec(dllimport)
#endif

#define NULL    0

#pragma warning(disable : 4244) // warning C4244: '=' : conversion from 'ATL::CSimpleStringT<BaseType>::XCHAR' to 'char', possible loss of data
#pragma warning(disable : 4251) // needs to have dll-interface to be used
#pragma warning(disable : 4275) // non dll-interface class used as base for dll-interface class     
#pragma warning(disable : 4355) // warning C4355: 'this' : used in base member initializer list

namespace GODZ
{
	typedef unsigned int HashCodeID;
	static const HashCodeID INVALID_HASH = 0;

	//Defines Package File Types
	enum EPackageFlag
	{
		PF_DLL,				//DLL Package
		PF_EXE,				//EXE
		PF_GDZ,				//GODZ File
		PF_RESOURCE,		//Resource Package
		PF_UNUSED			//used to be PF_GDZ, now it's updated to mark this field should be switched...
	};

	typedef unsigned int udword;
	typedef unsigned int UInt32;
	typedef unsigned int u32;
	typedef signed int s32;
	typedef unsigned short UInt16;			//same as WORD
	typedef unsigned short u16;			//same as WORD
	typedef int sdword;
	typedef unsigned char u8;
	typedef unsigned char UInt8;		//same as BYTE
	typedef unsigned long ulong;
	typedef __int64 Int64;
	typedef unsigned __int64 UInt64;
	typedef void* GODZHANDLE;			//used to bypass Windows.h / WinNT.h
	typedef void* GODZHWND;

	//Property types
	enum EPropertyType
	{
		PT_BOOL,		//stores a boolean primitive
		PT_FLOAT,		//stores a float
		PT_INT,			//stores an integer
		PT_VECTOR,		//world space vector
		PT_STRING,		//stores a char string
		PT_OBJECT,		//generic object
		PT_BOX,			//bounding box
		PT_MAP,			//hash_map
		PT_GUID,		//globally unique identifier (GUID)
		PT_MATRIX,		//matrix
		PT_RESOURCE,	//resource
		PT_CHARARRAY,	//reserved - obsolete?
		PT_EULER,		//Euler Angles
		PT_FLOAT4,		//float array
		PT_UINT32,		//unsigned int32
		PT_HASHCODE,	//hash code during runtime; possibly avail runtime / (always)editor
		PT_HASHSTRING,	//combines hash + string for debugging
		PT_COLOR,
		PT_QUATERNION,	//4D Vector that is composed of imaginary and real parts
		PT_ARRAY,		//Array of elements
		PT_UNKNOWN,		//Unknown property
		PT_STRUCT,		//Struct
		PT_MATRIX3,		//Rotation Matrix
	};

	//Loads all the DLLs declared in the config data
	void GODZ_API LoadPlugins();
}

#endif