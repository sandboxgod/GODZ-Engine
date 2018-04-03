/* ===========================================================
	Skeleton.h

	Copyright (c) 2011
	========================================================== 
*/

#pragma once

#include "SkeletonNode.h"
#include "Bone.h"
#include <vector>
#include <hash_map>

namespace GODZ
{
	class GODZ_API SkeletonNodeInspector
	{
	public:
		enum Action
		{
			Action_RemoveNode,
			Action_CancelSearch,
			Action_Continue
		};

		//Retun true to bail from the search....
		virtual Action inspect(SkeletonNode& node) = 0;
	};

	/*
	* Node Hierarchy
	*/
	class GODZ_API Skeleton
	{
	public:
		Skeleton();
		~Skeleton();

		Bone* GetBone(size_t index);
		Bone* GetBones() { return m_bones; }
		size_t GetNumBones() { return m_numBones; }
		void SetBones(Bone* bones, size_t num);

		void AddNode(const SkeletonNode& node);
		SkeletonNode* GetNodeByName(HString name);

		void TransformBy(const Vector3& pos);
		void TransformBy(const WMatrix3& matrix, const WMatrix& parentMatrix);
		void SetInitTransform(const Vector3& pos, const WMatrix3& matrix);

		void InspectNodes(SkeletonNodeInspector& inspector);
		void GetSkeletonNodes(std::vector<SkeletonNode*>& primitives);

		void ComputePrimitives( bool blockUntilDone = false );

	private:
		void Transform(SkeletonNode& node);

		Bone* m_bones;
		size_t m_numBones;

		//nodes that belongs to the base skeleton
		std::vector<SkeletonNode> m_nodes;

		typedef stdext::hash_map<HashCodeID, SkeletonNode* > SkeletonNodeMap;
		SkeletonNodeMap m_nameMap;
	};
}

