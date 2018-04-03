
#include "LUAConfig.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <Windows.h>


using namespace GODZ;

BOOL APIENTRY DllMain( HINSTANCE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			//Instantiate the Script System	
			GenericClass* pClass = LUAConfig::GetClass();
			godzassert(pClass != NULL);

			IGenericConfig* pConfig = (IGenericConfig*)pClass->GetInstance();
			RegisterScriptSystem(pConfig);
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
