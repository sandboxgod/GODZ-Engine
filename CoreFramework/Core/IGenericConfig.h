/* ===========================================================
	INI Configuration MetaClass

	Created Jan 4, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_GENERIC_CONFIG_H_)
#define _GENERIC_CONFIG_H_

#include "GenericObject.h"
#include <map>
#include "Name.h"
namespace GODZ
{
	typedef std::pair <FName, FName> Str_Pair;
	class GODZ_API ConfigSection
	{
	public:
		int GetNumOfProperties();
		Str_Pair GetProperty(int index);

		FName m_strSectionName;
		std::map<FName,FName> m_mapConfig;
	};

	//Script VM Interface
	class GODZ_API IGenericConfig : public GenericObject
	{
		DeclareGenericBase(IGenericConfig, GenericObject)

	public:
		//Parses the configuration file.
		virtual bool ParseFile(const char *filename, bool bReadProperties=true)=0;

		//Writes the node to the config file
		virtual void WriteNode(const char *section, const char *key, const char *value)=0;

		//Retrieves the node from the file.
		virtual const char* ReadNode(const char *section, const char *key)=0;
		virtual bool ReadBoolean(const char *section, const char *key)=0;

		//Returns the desired config section.
		virtual ConfigSection* GetSection(const char* sectionName)=0;

		//parse settings files....
		virtual void ParseSettings()=0;

	};
}

#endif