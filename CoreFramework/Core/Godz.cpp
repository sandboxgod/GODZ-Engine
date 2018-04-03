// CoreFramework.cpp : Defines the entry point for the DLL application.
//
#include <stdlib.h>
#include "GenericObject.h"
#include "GenericPackage.h"
#include "Logger.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "PackageWriter.h"
#include "PackageReader.h"
#include <time.h>
#include "HeapUnit.h"
#include "MemoryMngr.h"
#include <crtdbg.h>
#include <windows.h>
#include "CoreClasses.h"
#include "GenericObjData.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "StringTokenizer.h"
#include "HStringTable.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Reflection/StructProperty.h>
#include <CoreFramework/Gameplay/GameplayClasses.h>

using namespace std;


namespace GODZ
{
//DLL Module
HINSTANCE g_EngineHandle = 0;

//Starts the Logger
xLog GLogger;

//HANDLE g_Instance; //This DLL Package
char base_dir[_MAX_PATH];

HeapUnit* g_pLevelHeap=0;

bool EngineStatus::engineRunning = true;

char g_strPath[_MAX_PATH];




xLog::xLog()
: stream(NULL)
{
	char modname[_MAX_PATH];

	//Get the exe's name
	GetModuleFileName(NULL, modname, sizeof(modname));
	GetCurrentDirectory(_MAX_PATH, base_dir);

	char cname[_MAX_PATH];
	int i, lastIndex=0, len=(int)strlen(modname);
	for (i = 0; i < len; i++)
	{
		cname[i] = modname[i];
		if (base_dir[i] == '\\')
		{
			lastIndex = i;
		}

		if (cname[i] == '.')
			break;
	}

	cname[i] = '\0';

	//build path to config file
	StringCopy(g_strPath, cname, _MAX_PATH);
	rstring filename = g_strPath;
	filename += ".log";

	fileOpen(&stream, filename, FileOpenMode_Write);

#ifdef _DEBUG
	//dump memory leaks in debug mode
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
#endif

}

GODZ_API bool GODZ::CreateFolder(const char* folderName)
{
	int b = CreateDirectory(folderName, NULL);
	return b ? b == 1: 0;
}


GODZ_API void GODZ::DisplayMessage(const char* title, const char* format, ...)
{	
	va_list	ArgList;
	const int bufferSize = 1024;
	char	buf[bufferSize];

	va_start(ArgList,format);
	vsprintf_s(buf,bufferSize,format,ArgList);
	va_end(ArgList);


	Log("Error: title:%s message:%s\n", title, format);
	MessageBox(0, buf, title, 0);
}

// Returns the 'root' directory
GODZ_API const char* GODZ::GetBaseDirectory()
{
	return base_dir;
}

GODZ_API const char* GODZ::GetExeName()
{
	return g_strPath;
}

GODZ_API rstring GODZ::GetMapExt(const char* filename)
{
	rstring dir = base_dir;
	dir += "\\Data\\";
	dir += filename;
	dir += ".gmz"; //scene file (map)

	return dir;
}

GODZ_API rstring GODZ::GetModelExt(const char* filename)
{
	rstring dir = base_dir;
	dir += "\\Data\\";
	dir += filename;
	dir += ".gmz"; //model file (mesh content)

	return dir;
}

GODZ_API rstring GODZ::GetTexExt(const char* filename)
{
	rstring dir = base_dir;
	dir += "\\Data\\";
	dir += filename;
	dir += ".gmz"; //scene file (map)

	return dir;
}


GODZ_API rstring GODZ::GetPackageName(const char* filename, bool bIncludesFileExt)
{
	rstring buf = filename;
	char seps[]   = "\\./";

	vector<rstring> list;

	StringTokenizer tk(buf, seps);
	const char* token = tk.next();
	int c=0;
	while( token != NULL )
	{				
		list.push_back(token);
		c++;
		token = tk.next();
	}

	if (list.size() > 1)
	{
		if (bIncludesFileExt)
		{
			return list[c-2];
		}
		else
			return list[c-1];
	}

	return buf;
}

GODZ_API rstring GODZ::GetRelativeLocation(const char* filename)
{
	rstring dest = base_dir;
	dest += "\\";
	dest += filename;
	return dest;
}

GODZ_API rstring GODZ::GetString(const char* format, ...)
{
	va_list	ArgList;
	const int bufferSize = 1024;
	char	buf[bufferSize];

	va_start(ArgList,format);
	vsprintf_s(buf,bufferSize,format,ArgList);
	va_end(ArgList);

	return buf;
}

GODZ_API bool GODZ::FileExists(const char* filename)
{
	FILE* stream=NULL;

	fileOpen(&stream, filename, FileOpenMode_ReadBinary);
	if( stream == NULL )
	{
		return false;
	}

	fclose(stream);
	return true;
}



GODZ_API void GODZ::Log(const char* format, ...)
{
	va_list	ArgList;
	const int bufferSize = 1024;
	char	buf[bufferSize];

	va_start(ArgList,format);
	vsprintf_s(buf,bufferSize,format,ArgList);
	va_end(ArgList);

	GLogger.write(buf);

#if !defined(_FINAL)
	OutputDebugString(buf);
#endif
}

GODZ_API void GODZ::Warn(const char* format, ...)
{
#if defined(_DEBUG)
	va_list	ArgList;
	const int bufferSize = 1024;
	char	buf[bufferSize];

	va_start(ArgList,format);
	vsprintf_s(buf,bufferSize,format,ArgList);
	va_end(ArgList);

	GLogger.write(buf);
#endif
}

GODZ_API bool GODZ::GetAbsolutePathToPackage(const char* packageName, rstring& path)
{
	//Set the absolute path to this file.
	rstring modelFile = GetModelExt(packageName);
	if (FileExists(modelFile))
	{
		path = modelFile;
		return true;
	}

	rstring mapFile = GetMapExt(packageName);
	if (FileExists(mapFile))
	{
		path = mapFile;
		return true;
	}

	rstring texFile = GetTexExt(packageName);
	if (FileExists(texFile))
	{
		path = texFile;
		return true;
	}

	return false;
}

GODZ_API bool GODZ::RegisterScriptSystem(IGenericConfig* pObject)
{
	if (GlobalConfig::m_pConfig) //can only register one script system
		return false;

	GlobalConfig::m_pConfig = pObject;
	pObject->ParseSettings();
	return true;
}

GODZ_API GenericPackage* GODZ::LoadPackage(const char* filename, EPackageFlag flag)
{
	
	godzassert(flag!=PF_EXE); //"Cannot bind an executable to another executable package!");

	GenericPackage* gp = GenericObjData::m_packageList.FindPackageByName(filename);
	if (gp!=NULL) //package is already within memory
		return gp;
	

	switch(flag)
	{
	case PF_DLL:
		{
			gp = new GenericPackage(filename, flag);
			GenericObjData::m_packageList.AddPackage(gp);
			GenericObjData::m_pCurrentPackage=gp;
			Log("Loading Library %s\n", filename);
			HMODULE hOk = LoadLibrary(filename);

			if (hOk==NULL)
			{
				//TODO: update debug_assert(..) to accept #n of arguments like Log(...)
				const int bufferSize = 1024;
				char buf[bufferSize];
				sprintf_s(buf,bufferSize, "Cannot find the library %s", filename);
				godzassert(gp!=NULL);
			}

			GenericObjData::m_pCurrentPackage=gp;
			return gp;
		}
		break;
	case PF_RESOURCE:
	case PF_GDZ:
		{
			Future<GenericPackage*> p;
			if (SceneManager::GetInstance()->StreamPackage(filename, p))
			{
				//wait until it's ready!
				while(!p.isReady());
				return p.getValue();
			}
		}
		break;
	}

	return NULL;
}

GODZ_API bool GODZ::SavePackage(const char* filename, GenericPackage* package)
{
	PackageWriter writer;
	return writer.SavePackage(package, filename);
}

GODZ_API void GODZ::UnLoadPackage(GenericPackage* package)
{
	switch(package->GetPackageFlag())
	{
	default:
		{
			godzassert(0); //unknown package type?
		}
		break;
	case PF_DLL:
		godzassert(0); //feature not supported yet!
		break;
	case PF_RESOURCE:
	case PF_GDZ:
		{
			GenericObjData::m_packageList.Unload(package);
		}
		break;
	}
}

GODZ_API void GODZ::StringCopy(char* dest, const char* src, size_t numofelements)
{
	//visual studio 2008 feature-->
	strncpy_s(dest, numofelements, src, _TRUNCATE);
}

GODZ_API void GODZ::fileOpen(FILE** stream, const char* filename, FileOpenMode mode)
{
	const char* modetext = NULL;

	switch(mode)
	{
	default:
		break;
	case FileOpenMode_Write:
		{
			modetext = "w";
		}
		break;
	case FileOpenMode_ReadBinary:
		{
			modetext = "rb";
		}
		break;
	case FileOpenMode_WriteBinary:
		{
			modetext = "wb+";
		}
		break;
	}

	fopen_s(stream, filename, modetext);
}

// Loads plugins (DLLs)
void GODZ::LoadPlugins()
{
	
#if !defined(_DEBUG)
	Log("GODZ Release Build %d\n", GODZ_ENGINE_VERSION);
#else
	Log("GODZ Debug Build %d\n", GODZ_ENGINE_VERSION);
#endif

	Log("=================================================\n");

   // Seed the random-number generator with the current time so that
   // the numbers will be different every time we run.
   //srand( (unsigned)time( NULL ) );

	//setup random number generator
	time_t curTime = time( &curTime );
	srand( curTime );

	//create application heaps
	//CreateHeaps();

	//define default packages
	GenericPackage* godz = GenericObjData::m_packageList.CreateOrFindPackage("GODZ", PF_DLL);

	GenericObjData::m_pCurrentPackage = godz;

	//Load GODZ Class Definitions
	LoadCoreClasses();
	LoadGameplayClasses();

	rstring debugExtension = "_Release";

#ifdef _DEBUG
	debugExtension = "_Debug";
#endif

#ifdef _FINAL
	debugExtension = "";
#endif

	rstring scriptSystemText = "ScriptSystem";
	scriptSystemText += debugExtension;

	Log("Loading Library %s\n", scriptSystemText.c_str());

	//Load the script library
	GenericPackage* script = new GenericPackage(scriptSystemText, PF_DLL);
	GenericObjData::m_packageList.AddPackage(script);

	GenericObjData::m_pCurrentPackage = script;
	HMODULE hOk2 = LoadLibrary(scriptSystemText);
	

	//check to make sure the scripting system was set
	godzassert(hOk2 != NULL);

	//Set base folder to find game assets
	if (GlobalConfig::m_pConfig != NULL)
	{
		const char* newBaseDir = GlobalConfig::m_pConfig->ReadNode("folders","BaseDirectory");
		if (newBaseDir!=NULL)
		{
			rstring dir = base_dir;
			dir += "\\";
			dir += newBaseDir;
			StringCopy(base_dir,dir,_MAX_PATH); //set new base directory!
		}

		//load external libraries
		ConfigSection* s = GlobalConfig::m_pConfig->GetSection("Imports");
		if (s!=NULL)
		{
			int size=s->GetNumOfProperties();
			for(int i=0;i<size;i++)
			{
				Str_Pair p = s->GetProperty(i);
				const char* value = p.second;
				rstring importPackage = value;
				importPackage += debugExtension;

				//Declare a class package
				GenericPackage* dllPackage = new GenericPackage(value, PF_DLL);
				GenericObjData::m_packageList.AddPackage(dllPackage);

				//Set so all classes that load know about their package
				GenericObjData::m_pCurrentPackage = dllPackage;

				LoadPackage(importPackage, PF_DLL);
			}
		}
	}

	//All other classes that are loading into memory are coming from the .exe which
	//happens AFTER this function exists. So we simply tell the static GenericObject
	//that all incoming packages belong to the executable's package.

	//Need to strip off '.exe' ext from package name
	rstring exePackageName = GetPackageName(g_strPath, 0);
	GenericPackage* exe = new GenericPackage(exePackageName, PF_EXE);
	GenericObjData::m_packageList.AddPackage(exe);
	GenericObjData::m_pCurrentPackage=exe;

	//Mark that we are done loading Classes... This will let the engine know it can
	//no longer allow other objects to be registered....
	GenericObjData::mIsClassListBuilt = true;

	//Register class properties; attach them to their Classes...
	ClassProperty::RegistryProperties();
	StructProperty::RegisterProperties();

	//Load string table
	HStringTable::GetInstance()->Load();
}


const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, char* threadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}

} // namespace


BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	//be careful in here (keep thread safe)
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			GODZ::g_EngineHandle = hModule;

			//This case occurs after the GodzEngine.DLL has been fully loaded (which occurs
			//after all classes, etc have been instantiated).
			GODZ::LoadPlugins();
		}
		break;
	case DLL_THREAD_ATTACH:
		{
		}
		break;
	case DLL_THREAD_DETACH:
		{
		}
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

