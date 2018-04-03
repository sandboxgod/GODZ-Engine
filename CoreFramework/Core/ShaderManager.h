/* ===========================================================
	Shader Manager

	Stores a list of shaders that can be applied to models. 

	Created oct 15, '05 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__SHADERMANAGER__)
#define __SHADERMANAGER__

#include "Godz.h"
#include "GenericSingleton.h"
#include "ShaderResource.h"
#include <hash_map>
#include <vector>

namespace GODZ
{
	//Represents a shader
	struct GODZ_API ShaderDefinition
	{
		rstring shaderName; //Name of the Shader
		rstring filename;	//filename of the shader
		ShaderRef m_pShader;
	};

	//Loads all shaders from the Shaders folder into memory
	class GODZ_API ShaderManager : public GenericSingleton<ShaderManager>
	{
	public:
		ShaderManager();

		//Adds a shader definition to the engine. Shader definitions represent actual shaders that
		//are available for use.
		void AddShaderDefinition(ShaderDefinition& def);
		ShaderDefinition& GetShaderDefinition(size_t index);
		size_t GetNumDefinitions();

		//Finds the desired shader
		ShaderResource* FindShader(HString shaderName);

	protected:
		//loads the shader into memory
		ShaderResource* LoadShader(ShaderDefinition* pShaderDef);


		typedef stdext::hash_map<HashCodeID, ShaderRef> ShaderMap;
		ShaderMap mMap; //for hash lookups
		std::vector<ShaderDefinition> mShaders; //for iteration
		GenericPackage* m_shaderPackage;
	};
}

#endif