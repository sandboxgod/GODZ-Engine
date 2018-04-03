/* ===========================================================
	Godz.h defines the framework of the engine.
	Created Nov 1, 2003 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	========================================================== 
*/

#if !defined(__GODZ_H__)
#define __GODZ_H__

//#pragma once


//Trick to speed up STL
//http://cowboyprogramming.com/2007/02/22/what-is-_invalid_parameter_noinfo-and-how-do-i-get-rid-of-it/
//http://msdn.microsoft.com/en-us/library/aa985965.aspx - Checked iterators
#ifndef _DEBUG
//#define _SECURE_SCL 0 //disable STL iterator checking in release
#endif

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <vector>
#include <math.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

namespace GODZ
{
	const float kPi = 3.14159265f;
	const float kPiOver180 = kPi / 180.0f;
	#define DEG2RAD(x) (x * kPiOver180) // Converts degrees to radians

	//Engine Version
	static const int GODZ_ENGINE_VERSION = 1008;


	//Forward Declarations
	class GenericObject;
	class GenericPackage;
	class GenericClass;
	class IGenericConfig;
	class WEntity;

	static const char* NONE="None";	//Alias for 'NULL'
	static const char* NULL_STRING = "NULL";

	// Converts an expresion into a string
	#define CLASS_TO_STR(n) ""#n""

	//Removes all the elements from memory.
	template <class T> void releaseVector(std::vector<T*>& dynamic_list)
	{
		std::vector<T*>::iterator temp;
		for (temp = dynamic_list.begin(); temp != dynamic_list.end(); temp++)
		{
			if (*temp != NULL)
			{ 
				T* element = (*temp);
				//I used to have 'delete (*temp)' here but then I discovered if the
				//class made their destructor private, etc the compiler couldn't
				//catch the mistake.
				delete element; 
			}
		}

		dynamic_list.clear();
	}

	//checks the vector for a value
	template <class T> bool ContainsValue(std::vector<T>& list, T value)
	{
		for (size_t i=0; i< list.size(); i++)
		{
			if (value == list[i])
			{
				return true;
			}
		}

		return 0;
	}
};

// Includes base classes
#include "CoreFramework.h"
#include "RString.h"
#include "HString.h"
#include "GodzAssert.h"

namespace GODZ
{
	//Set to true if engine still running and all singletons are still valid....
	struct GODZ_API EngineStatus
	{
		static bool engineRunning;
	};

	//Global Config File
	struct GODZ_API GlobalConfig
	{
		static IGenericConfig* m_pConfig;			//Global Configuration object - <engine.ini>
	};

	//Container Types
	typedef std::vector<GODZ::GenericObject*> GenericIterator;
	typedef std::vector<GODZ::WEntity*> EntityIterator;

	// ===================================================================
	// Global Functions/Variables

	//Creates a folder on the client system. Returns true if successful.
	GODZ_API bool CreateFolder(const char* folderName);

	//Returns true if the vector contains the object.
	GODZ_API bool ContainsObject(GODZ::GenericObject* obj, std::vector<GenericObject*>& generics);

	// Displays a popup (alert box)
	GODZ_API void DisplayMessage(const char* title, const char* msg, ...);

	//dynamic loads an object
	GODZ_API GenericObject* DynamicLoadObject(const char* objectName, const char* packageName, EPackageFlag flag=PF_GDZ);

	//Sets the thread name
	GODZ_API void SetThreadName( unsigned long dwThreadID, char* threadName);

	// Loads an object based on the class name. Only static class instances are returned.
	// fullName - <Package.Class> such as Default.GenericObject. Optionally, just
	// the <Class> argument can be passed- which will invoke a search for ANY object
	// that has the name. The first occurence will be returned. It is recommended to
	// pass in the package name of the object if known.
	GODZ_API GODZ::GenericClass* StaticLoadObject(const HString& string);

	//Returns true if the file at this absolute path exists!
	GODZ_API bool FileExists(const char* filename);

	//Loads a GODZ package into memory. This function returns the main package that encapsulates
	//all objects 'exported' from this package.
	// [in] filename
	// [in] flag indicates the package format. Valid arguments are PF_DLL or
	// PF_GDZ.
	extern "C" GODZ_API GODZ::GenericPackage* LoadPackage(const char* filename, GODZ::EPackageFlag flag=GODZ::PF_GDZ);

	//Serializes all objects belonging to the package argument to the file destination.
	//This function expects the absolute filename. Call GetMapExt()/GetModelExt() to
	//get the absolute filename!
	extern "C" GODZ_API bool SavePackage(const char* filename, GODZ::GenericPackage* package);

	//Unloads a package from memory.
	extern "C" GODZ_API void UnLoadPackage(GODZ::GenericPackage* package);
	
	//Sends a message to the log....
	extern "C" GODZ_API void Log(const char* format, ...);

	//Sends a developer message to the log if this is DEBUG build
	GODZ_API void Warn(const char* format, ...);

	//Formats a string into a string buffer
	GODZ_API GODZ::rstring GetString(const char* format, ...);

	//Given a package name (like "rav4"), this function searches the base folder
	//for the package! True is returned if the path could be discovered.
	// [in] packageName - name of the package. Does not use a file extension!
	// [out] path - absolute to file if this operation was successful
	GODZ_API bool GetAbsolutePathToPackage(const char* packageName, rstring& path);

	// Returns the base directory.
	extern "C" GODZ_API const char* GetBaseDirectory();

	// Returns the path to the executable file
	GODZ_API const char* GetExeName();

	// Returns the full path to this godz map file
	GODZ_API rstring GetMapExt(const char* filename);

	// Returns the full path to this godz model file
	GODZ_API rstring GetModelExt(const char* filename);

	// Returns the full path to this godz texture file
	GODZ_API rstring GetTexExt(const char* filename);

	// Returns the 'package name' from an absolute path to a file!
	GODZ_API rstring GetPackageName(const char* filename, bool bIncludesFileExt = true);

	//Appends the base directory to the filename[in] and stores it within
	//the dest[out] char buffer.
	GODZ_API rstring GetRelativeLocation(const char* filename);

	// Registers the scripting system
	GODZ_API bool RegisterScriptSystem(IGenericConfig* pObject);

	//Wrapper function that should be called when copying into char string arrays
	//sizeOfDestBuffer = maximum size of the destination buffer
	GODZ_API void StringCopy(char* dest, const char* src, size_t sizeOfDestBuffer);

	enum GODZ_API FileOpenMode
	{
		FileOpenMode_Write, //w+
		FileOpenMode_ReadBinary, //rb
		FileOpenMode_WriteBinary, //wb+
	};

	GODZ_API void fileOpen(FILE** stream, const char* filename, FileOpenMode mode);
}

#endif //__GODZ_H__