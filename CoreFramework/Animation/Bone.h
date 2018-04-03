/* ===========================================================
	Bone.h

	Copyright (c) 2011
	========================================================== 
*/

#pragma once

#include "SkeletonNode.h"
#include <CoreFramework/Core/VertexTypes.h>
#include <CoreFramework/Core/GenericNode.h>
#include <CoreFramework/Math/WMatrix.h>

namespace GODZ
{
	class AnimController;
	struct Bone;
	class SkelMeshInstance;
	class Skeleton;
	

	//Blends a vertex between two matrices...
	GODZ_API void BlendVertexNew(Vertex& result, const Vertex& pos, const WMatrix& mat0, const WMatrix& mat1, float fWeight);

	//Transforms a vertex relative to the bone/weight (influence). If a vertex is only
	//affected by one bone, the weight should be set to 1.0f (as expected).
	// [in] bone - contains the relative/world matrices affecting this vertex
	// [in] vertex - vertex that will be transformed
	// [in] weight - amount of influence
	GODZ_API void BoneMul(Bone* bone, Vector3* v, float weight, WMatrix16f& boneTM);


	struct GODZ_API Bone
	{
		Bone();
		Bone(const char* Name);
		~Bone();

		//Adds the bone as a child of this bone
		void AddBone(int boneIndex);

		void SetName(const char* str) { m_sName = str; }
		const HString& GetName() const { return m_sName; }

		//checks to see if this bone is assigned as a child of this bone
		bool IsChild(int boneIndex);

		//Returns the parent of this bone
		Bone* GetParent(SkelMeshInstance* skMesh);


		//Assigns a parent to this bone
		void SetParentBone(int parentIndex);

		//serializes this bone
		void Serialize(GDZArchive& ar);

		//update the current transform of this bone (and all children attached to this
		//bone).
		//[in] dt - delta time
		//[in] animController - updates this bone with the current animation track
		void Transform(float dt, AnimController* animController, udword boneIndex, bool bUsingSkinMethod, SkelMeshInstance* skMesh, Skeleton* s);

		//recomputes this bone's bounding box
		void UpdateBound();

		HString m_sName;					//name of this bone
		WMatrix16f m_init;					//initial pose
		WMatrix16f m_initInv;				//inverse of the initial matrix
		WMatrix16f m_frame;					//local space transform matrix for this frame
		WMatrix16f m_final;					//final matrix of this bone (includes parent). same as m_frame
		WMatrix16f m_offsetTM;					//final matrix used for vertex transformation
		int parentIndex;					//parent of this bone
		//WBox m_box;							//bounding box
		NodeList<int> children;				//children of this bone
		std::vector<SkeletonNode> m_nodes;
	};


}

