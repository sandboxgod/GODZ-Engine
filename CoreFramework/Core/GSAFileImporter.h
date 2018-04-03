/* ===================================================================
	.GSA File Importer

	Imports .GSA (GODZ Scene Export) files.

	GSA files are defined in the left handed coordinate system which
	is used by direct x.

	Created May 1, 2004 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#if !defined(GSA_FILE_IMPORT_H)
#define GSA_FILE_IMPORT_H

#include "MeshImporter.h"
#include "BoneSystem.h"
#include "SkeletalMesh.h"
#include "GodzVector.h"
#include <CoreFramework/Math/EulerAngle.h>

namespace GODZ
{
	//forward declar
	class AnimationInstance;
	class Mesh;
	class GenericPackage;

	//represents an imported matrix
	struct GSAMatrix
	{
		Vector3 pos;
		Quat4f rot;
		Vector3 scale;
		EulerAngle euler; //yaw pitch roll
		void SetMatrix(WMatrix16f& m);
		operator WMatrix16f();
	};

	struct GSAVert
	{
		std::vector<int> weights;
		std::vector<int> bones;
	};

	struct BoneRef
	{
		std::vector<rstring> parents;
	};

	struct BoneWeights
	{
		std::vector<BlendWeight> m_blendweights;
	};

	struct GSAVertexDup
	{
		std::vector<VertexDupIndex> indices;
	};



	class GODZ_API GSAFileImporter : public MeshImporter
	{
	public:
		GSAFileImporter();
		~GSAFileImporter();

		//Adds animation to an existing mesh
		bool AppendGSA(const char* filename, SkeletalMesh* mesh, GodzVector<rstring>& list);

		//Imports a mesh (if it's a skeletal mesh, that will be imported automatically)
		atomic_ptr<Mesh> ImportMesh(const char* filename, GenericPackage* p, GodzVector<rstring>& list);

	protected:
		struct MaterialFace
		{
			Face face;
			TFace tface;
		};

		GodzExporterHeader header;


		BlendVertex* m_pBlendVerts;			//blend matrix info (weights/bone index per vertex)
		BoneWeights* boneWeights;
		HString mDefaultShader;
		HString mSkinShader;
		
		FILE* stream;
		rstring filename;

		void ImportBipedSampleKeys(AnimationInstance* animInstance, udword seqNum, const char* boneName, bool bOnlyImportFirstKey = 0);
		void ImportBezierKeys(AnimationInstance* animInstance, udword seqNum, const char* boneName, bool bOnlyImportFirstKey = 0);
		void ReadAnimation(SkeletalMesh* mesh, MeshInfo& meshInfo, GodzVector<rstring>& list);
		Vector3 ReadVector3f();
		Quat4f ReadQuat4f();
		void ReadMatrix(GSAMatrix& matrix);
		void ReadMatrix16f(WMatrix16f& matrix);
		void ReadMaterial(Mesh* mesh, GenericPackage* p, GodzVector<rstring>& list);
		void ReadMesh(GenericPackage* p, Mesh* mesh, MeshInfo& meshImportInfo, GodzVector<rstring>& list);
		void ReadSkin(SkeletalMesh* mesh, MeshInfo& meshImportInfo, GodzVector<rstring>& list);
		rstring ReadText();
		void ReadShapes(Bone* parent, MeshInstance* mesh, GodzVector<rstring>& list);
		void UpdateMeshInfo(const Deprecated_GodzExporterHeader& header, MeshInfo& meshInfo);

		void DebugMeshInstance(MeshInstance* mi);
		
		//converts GSAMatrix (max coord sys) to directx LH coord system
		void ConvertMaxToDX(GSAMatrix& matrix);


		enum MatSlot
		{
			ID_AM,  // ambient
			ID_DI,   // diffuse
			ID_SP,   // specular
			ID_SH,   // shininesNs
			ID_SS,   // shininess strength
			ID_SI,   // self-illumination
			ID_OP,   // opacity
			ID_FI,   // filter color
			ID_BU,   // bump 
			ID_RL,   // reflection
			ID_RR,  // refraction 
			ID_DP,  // displacement
		};

	};

}

#endif