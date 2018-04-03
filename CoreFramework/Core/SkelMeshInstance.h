/* ==============================================================
	SkeletalMeshInstance

	To keep things simple, the structures declared here don't
	have very many pointer memory variables. This makes it a bit
	easier to clone mesh instances.

	Created Jun 25, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#include "SkeletalMesh.h"
#include <CoreFramework/Animation/AnimationInstance.h>
#include "Timer.h"
#include <CoreFramework/Animation/Bone.h>

#if !defined(__SKELETAL_MESH_INSTANCE__H__)
#define __SKELETAL_MESH_INSTANCE__H__

namespace GODZ
{
	//foward decl
	class AnimController;
	class IAnimNotify;
	class SkelMeshInstance;

	class GODZ_API SkelMeshInstance : public MeshInstance
	{
		DeclareGeneric(SkelMeshInstance, MeshInstance)

	public:
		SkelMeshInstance();
		~SkelMeshInstance();

		//Alternate software skinning solution - transforms each 'skin' vertex. Uses duplicate vertices
		//to update the cloned verts
		void ComputeSkinVerts(WMatrix16f& worldMatrix);

		//Alternate software skinning solution - transforms each individual vertex
		void ComputeSkinVerts2(WMatrix16f& worldMatrix);

		//Creates bones that will be owned by this mesh
		bool CreateBones(size_t numBones);

		virtual Skeleton* GetSkeleton();

		//returns a ptr to the bone
		Bone* GetBone(size_t boneIndex);

		//returns the normals array
		Vector3* GetNormalsArray()
		{
			return &m_pNorms[0];
		}

		Vector3* GetTangentsArray()
		{
			return &m_pTangents[0];
		}

		Vector3* GetBinormalsArray()
		{
			return &m_pBinorms[0];
		}

		//returns the matrix that can be used to transform a vector relative to a bone.
		//The memory alignment is guaranted to be contigious for fast access to vertex transformations, etc
		WMatrix16f& GetBoneTM(size_t index);

		//returns number of bones stored within this system
		size_t GetNumOfBones();

		//returns the root of the bone structure
		Bone* GetRootBone();

		//returns the index of the root bone
		int GetRootBoneIndex();

		//finds the named bone
		Bone* FindBone(HashCodeID name);
		bool FindBone(HashCodeID name, UInt32& index);

		//initializes this instance
		virtual void Initialize(Mesh* mesh);

		//Renders collision volumes
		void RenderCollisionVolumes(SceneInfo& sceneInfo);

		//sets the parent of the bone at the boneIndex
		void SetParentForBone(udword parentIndex, udword boneIndex);

		//serializes this skeletal mesh instance
		virtual void Serialize(GDZArchive& ar);

		virtual void Transform(float deltaTime, Skeleton* skeleton, AnimController* controller);

		//Updates the hierarchy of the bones, assigning child bones to parents, etc
		void UpdateHierarchy();

	protected:
		Vector3* m_pNorms;						//non-serialized (normals array)
		Vector3* m_pTangents;					//non-serialized (tangents array)
		Vector3* m_pBinorms;					//non-serialized (binormals array)

		//transforms a vertex/position relative to this bone's initial position
		//then towards m_final
		WMatrix16f* m_boneTM;

		Bone*		m_bones;					//skeletal animation (animated bones)
		size_t		m_numBones;
		AnimationInstance* m_pAnimInstance;		//my animation instance
		int m_nRootIndex;						//root of the bone hierarchy
	};
}

#endif