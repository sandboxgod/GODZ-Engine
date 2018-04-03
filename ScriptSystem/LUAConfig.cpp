
#include <stdlib.h>
#include <stdio.h>
#include "LUAConfig.h"
#include <map>
#include <CoreFramework/Reflection/GenericClass.h>


using namespace GODZ;

ImplementGeneric(LUAConfig)

LUAConfig *g_objConfig = NULL;
//NameTable *m_pNames = 0; //string table

int GODZ::luaSetConfigOption(lua_State* L)
{	
	bool bJustCreatedTable=false;
	const char* typeName=NULL;
	if (lua_isboolean(L,-1))
	{
		int bResult = lua_toboolean(L,-1);
		if (bResult)
			typeName="true";
		else
			typeName="false";//m_pNames->AddString("false");
	}
	else
	{
		//This value indicates 'string','table', or will encapsulate a value if this
		//is a table element (property defined within a table).
		typeName=lua_tostring(L, -1); 
	}

	lua_pop(L, 1);

	//this is always the variable/table name
	const char* propertyName=lua_tostring(L, -1);
	lua_pop(L, 1);

	//the following will always be NULL if this is a TABLE or table element!
	const char* value=lua_tostring(L, -1);
	
	//map the property
	if (typeName!=NULL)
	{
		if (strcmp(typeName,"string")==0)
		{
			g_objConfig->IsCreatingTable=0;
		}

		if (strcmp(typeName,"table")==0)
		{
			g_objConfig->IsCreatingTable=1;
			g_objConfig->tableName=propertyName;
			bJustCreatedTable=true;
		}

		const char *sectionName=NULL;
		if (g_objConfig->IsCreatingTable)
		{
			sectionName=g_objConfig->tableName;
			if (!bJustCreatedTable) //don't store the table name itself as a property
				value=typeName; //the value is stored within the typename
		}
		else
			sectionName="GLOBAL";


		ConfigSection *s=NULL;
		if (g_objConfig)
			s = g_objConfig->GetSection(sectionName);

		if (s==NULL)
		{
			ConfigSection section;
			section.m_strSectionName=sectionName;
			if (value!=NULL)
			{
				//propertyName=m_pNames->AddString(propertyName);
				//value=m_pNames->AddString(value);
				section.m_mapConfig.insert(Str_Pair(propertyName, value));
			}
			g_objConfig->AddSection(section);
		}

		if (s!=NULL && value!=NULL)
		{
			//propertyName=m_pNames->AddString(propertyName);
			//value=m_pNames->AddString(value);
			s->m_mapConfig.insert(Str_Pair(propertyName, value));
		}
	}

	return 0;
}

int GODZ::luaCreateFolder(lua_State* L)
{
	const char *folderName = lua_tostring(L, -1); 
	bool bCreated = GODZ::CreateFolder(folderName);

	//debug_assert (bCreated, GetString("Could not create the folder %s", folderName));

	return 0;
}

LUAConfig::LUAConfig()
: L(NULL)
, m_bParsedFile(0)
, IsCreatingTable(0)
{
	g_objConfig=this;
	OpenLUA();
	sections.reserve(150);
}

void LUAConfig::AddSection(ConfigSection &s)
{
	sections.push_back(s);
}


ConfigSection* LUAConfig::GetSection(const char* sectionName)
{
	std::vector<ConfigSection>::iterator sectionIter;
	for (sectionIter=sections.begin();sectionIter!=sections.end();sectionIter++)
	{
		ConfigSection section = *sectionIter;
		if (strcmp(section.m_strSectionName,sectionName)==0)
			return &(*sectionIter);
	}

	return NULL; //config section not found.
}

void LUAConfig::OpenLUA()
{
	L = lua_open();
	lua_baselibopen(L);
	lua_iolibopen(L);
	lua_strlibopen(L);
	lua_mathlibopen(L);
	lua_tablibopen(L); //Table lib

	lua_register( L, "SetConfigOption", luaSetConfigOption );
	lua_register( L, "CreateFolder", luaCreateFolder );
}

LUAConfig::~LUAConfig()
{
	lua_close(L);
	//delete m_pNames;
}


bool LUAConfig::ParseFile(const char* filename, bool bReadProperties)
{
	if (bReadProperties && (lua_dofile(L, "Scripts/ReadConfig.lua") == 0)
		)
	{
		//If we are reading properties, first we must setup the ReadConfig.lua
		//script.
	}

	//run the script
	if (lua_dofile(L, filename) != 0)
	{
		return false;
	}

	m_bParsedFile=true;

	if (bReadProperties)
	{
		//okay, close the VM
		lua_close(L);
		OpenLUA();
	}

	return true;
}

void LUAConfig::ParseSettings()
{
	rstring path = GetExeName();
	path += ".lua";

	//look for the default config script
	ParseFile("Default.lua", true);

	ParseFile(path, true);

	//Read Engine/Gameplay Settings...
	ParseFile("EngineSettings.lua", true);
}

const char* LUAConfig::ReadNode(const char *section, const char *key)
{
	ConfigSection *s = GetSection(section);
	if (s==NULL)
		return NULL;

	std::map<FName,FName>::iterator m1Iter;
	for(m1Iter=s->m_mapConfig.begin();m1Iter!=s->m_mapConfig.end();m1Iter++)
	{
		const char* property=m1Iter->first;
		const char* val=m1Iter->second;
		if (strcmp(property,key)==0)
		{
			return val;
		}
	}

	return NULL;
}

bool LUAConfig::ReadBoolean(const char *section, const char *key)
{
	ConfigSection *s = GetSection(section);
	if (s==NULL)
		return 0;

	const char *text=ReadNode(section,key);
	if (text!=NULL && strcmp(text,"true")==0)
		return true;

	return false;
}


void LUAConfig::WriteNode(const char *section, const char *key, const char *value)
{
}