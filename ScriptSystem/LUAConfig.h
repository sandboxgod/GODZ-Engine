/* ===========================================================
	Uses LUA for configs -- http://www.lua.org

	Created Jan 7, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_LUA_CONFIG_H_)
#define _LUA_CONFIG_H_

#include <CoreFramework/Core/NameTable.h>
#include "ScriptSystem.h"
#include <CoreFramework/Core/GenericObject.h>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace GODZ
{

//----------------------------------------------------------------------
//Declares functions visible to the LUA runtime
//----------------------------------------------------------------------

	//Notification a script has set a config value.
	int luaSetConfigOption(lua_State* luaVM);

	//Request from a LUA configuration script to generate a folder on the client's system
	int luaCreateFolder(lua_State* luaVM);	

	class SCRIPT_API LUAConfig : public IGenericConfig
	{
		DeclareGeneric(LUAConfig, IGenericConfig)

	public:
		bool IsCreatingTable;
		rstring tableName;

		LUAConfig();
		~LUAConfig();

		void AddSection(ConfigSection &s);
		ConfigSection* GetSection(const char* sectionName);

		void OpenLUA();

		//Parses the configuration file.
		bool ParseFile(const char *filename, bool bReadProperties=0);

		virtual void ParseSettings();

		//Writes the node to the config file
		void WriteNode(const char *section, const char *key, const char *value);

		//Retrieves the node from the file.
		const char* ReadNode(const char *section, const char *key);
		bool ReadBoolean(const char *section, const char *key);

	protected:
		lua_State *L;
		std::vector<ConfigSection> sections;
		bool m_bParsedFile;		
	};
}

#endif