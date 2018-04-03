/* ===========================================================
	Shader resource

	Shader resource objects contain the actual implementation
	of the sfx effect. All shader effects will need to be
	implemented seperately for each renderer implementation.

	Created Apr 5, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_SHADER_RESOURCE_H_)
#define _SHADER_RESOURCE_H_

#include "ResourceObject.h"
#include "ShaderTechniqueList.h"


namespace GODZ
{
	//forward declar
	class Material;
	class Mesh;
	class MeshInstance;
	struct SceneInfo;			//will get a circular dependency unless we use pointers here. because SceneInfo includes GeometryInfo which includes MaterialInfo in which includes this file.
	struct MaterialInfo;

	static const int MAX_PARAMETERS = 99;			//maximum number of parameters we expect in a shader

	//information for the shader
	struct GODZ_API ShaderInfo
	{
		MaterialInfo*	m_pMat;						//Current Material
		mutable bool	m_bUploadedParamThisFrame[MAX_PARAMETERS];	//stores status per parametr

		ShaderInfo();
	};

	class GODZ_API ShaderResource : public ResourceObject
	{
	public:
		ShaderResource(HashCodeID hash);

		virtual void CreateRenderDeviceObject();

		//returns the number of techniques this shader has available
		size_t GetNumTechniques();

		//returns the name of the technique
		const char* GetTechniqueName(size_t index);

		//returns true if this shader can perform GPU Skinning (matrix palette skinning)
		bool IsMatrixPalette() { return false; }

		//sets the active technique
		bool SetTechnique(udword index);

		//if true, this shader always uploads constants to the GPU each render
		bool IsAutoUploadConstants();

		//returns if this shader renders the mesh subset manually
		bool HasDrawRoutine();

		//informs the shader to upload their constants on the next render. not
		//needed if this shader atuomatically uploads constants
		void UploadConstants();

	protected:
		bool m_bAlwaysUploadConstants;
		bool m_bUpdateConstants;
		bool m_bUploadOnce;		//upload constants once per pass during Begin()
		bool m_bHasDrawRoutine;
		Future<ShaderTechniqueList> m_techniqueList;
	};

	//Handle to a shader program
	typedef GenericReference<ShaderResource> ShaderRef;
}

#endif 