
#ifndef _SCRIPTSYSTEM_H_
#define _SCRIPTSYSTEM_H_

#ifdef SCRIPTSYSTEM_EXPORTS
#define SCRIPT_API __declspec(dllexport)
#else
#define SCRIPT_API __declspec(dllimport)
#endif

#include <CoreFramework/Core/Godz.h>

#endif