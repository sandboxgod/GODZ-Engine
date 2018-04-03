/* ===========================================================
	Axis Aligned Bounding Box Tree (AABB Tree)

	Sources Cited:

	[1] http://www.flipcode.com/dp/dpcolumn_issue05.shtml
	[2] Efficient Collision Detection of Complex Deformable 
	Models Using AABB Trees [Paper by GINO VAN DE BERGEN]

	Created Feb 7, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_AABBTree_H_)
#define _AABBTree_H_

#include "AABBCollisionPrimitive.h"
#include <CoreFramework/Core/GenericNode.h>

namespace GODZ
{
	//Forward Declarations
	class AABBNode;
	class ICollisionCallback;
	struct WPlane;
	class ModelResource;

	//Defines the behavior of the AABB tree
	struct GODZ_API AABBTreeInfo
	{
		//heuristics
		udword max_tree_depth;		//max depth the tree can reach
		udword min_vertices;		//min number of vertices each leaf can store

		//ensures that an AABB is never generated that is over min_vertices. Normally, this
		//algorithm is not required because the best axis algorithm normally produces a
		//perfectly balanced tree.
		bool m_bPrune;		

		//internal tree data
		udword right_children;		//num of right child branches
		udword left_children;		//num of left child branches
		udword curr_max_depth;		//max depth of this tree

		//Creates a tree info structure that stores heuristic data and 
		//information about the AABB tree.
		// [in] depth - max depth of the tree
		// [in] min_num_verts - minimum # of vertices stored by leaf nodes.
		// [in] bPrune - if best axis fails to produce left/right children, use a brute
		//force algorithm. This may be required for a special case when the boxes of an 
		//AABB Tree cover huge, empty spaces (leaf nodes are inaccurate).
		AABBTreeInfo(udword depth=0, udword min_num_verts=124, bool bPrune=true);

		//Returns true if this is a balanced tree
		bool IsBalanced();		

		friend GDZArchive& operator<<(GDZArchive& ar, AABBTreeInfo& s) 
		{
			ar << s.max_tree_depth << s.min_vertices << s.m_bPrune << s.right_children << s.left_children << s.curr_max_depth;
			return ar;
		}
	};

	//Child component of an AABB Tree
	class GODZ_API AABBNode : public AABBCollisionPrimitive
	{
	public:
		//for serialization
		AABBNode();

		//clones the source AABB Node
		AABBNode(const AABBNode* node);

		//note, destructor is not called on this class
		~AABBNode();

		//Returns a Copy of this instance
		virtual GenericReference<CollisionPrimitive> GetClone();

		virtual CollisionType GetCollisionType() { return CollisionType_AABBTree; }

		//constructor to build the AABB Tree
		AABBNode(std::vector<Vector3> triangles, AABBTreeInfo& info, udword depth);

		//collision test
		bool SweepHit(WMatrix16f& invMat, CollisionPrimitive* other, const Vector3& vel, const Vector3& otherVel, CollisionResult& result, ICollisionCallback* call);
		virtual bool SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result);

		//builds the AABB Tree using best axis and brute force methods to meet the required
		//heuristic
		void BuildTree(std::vector<Vector3> vertexList, AABBTreeInfo& treeInfo, udword depth);

		//Returns true if this is a leaf node (stores vertices)
		bool IsLeaf();

		//Returns left child branch
		AABBNode* GetLeftChild();

		//Returns right child branch
		AABBNode* GetRightChild();

		//returns the best axis to divide up the vertices
		int FindBestAxis(std::vector<Vector3>& vertexList);

		size_t GetNumVerts() { return m_nVerts; }

		//Returns true if this node is colliding with the collision model.
		bool Intersects(CollisionPrimitive* other);

		//Returns true if this node or a child tree node was hit.
		bool IsHitBy(const WRay& ray, TraceResult& rayTrace);
		
		//Serialization interface
		virtual void Serialize(GDZArchive& ar);

		//Returns the surface (stores it into TraceResult) that was stabbed
		virtual bool Trace(const WRay& ray, TraceResult &result);

	private:
		GenericReference<AABBNode> children[2];		//left child = 0, right child = 1
		Vector3* m_VB;				//vertex buffer
		WPlane* m_plane;			//planes that composes this node (corresponds to each triangle)
		size_t m_nVerts;			//VB size 
	};

	/*
	* Root of the AABBTree. The AABBRoot is maintained in object space.
	* So instead of transforming every triangle in our
	* soup everytime we move/rotate, instead we inverse-transform collision queries into
	* our local space and perform the collision tests there
	*/
	class GODZ_API AABBRoot : public AABBNode
	{
	public:
		AABBRoot();
		AABBRoot(const AABBRoot*);
		AABBRoot(std::vector<Vector3> triangles, AABBTreeInfo& info,  udword depth=0);

		virtual void GetBox(WBox& box) const;
		virtual void GetSphere(WSphere& sphere) const;

		virtual bool SweepHit(CollisionPrimitive* other, const Vector3& vel, const Vector3& otherVel, CollisionResult& result, ICollisionCallback* call);
		virtual bool Trace(const WRay& ray, TraceResult &result);
		virtual void TransformBy(const WMatrix& matrix);
		virtual void SetTransform(const Vector3& pos, const WMatrix3& matrix);
		virtual void SetTransform(const WMatrix& matrix);

		virtual void GetTransform(Vector3& pos, WMatrix3& matrix) const;
		virtual void GetTransform(WMatrix& matrix) const;

		virtual GenericReference<CollisionPrimitive> GetClone();

		virtual void Serialize(GDZArchive& ar);

		WMatrix16f m_tm;			//Current Scale / Transformation matrix
		WMatrix16f mInverse;
	};

	//AABB Tree Builder - builds an AABBTree and returns the primitive
	class GODZ_API AABBTreeBuilder
	{
	public:
		AABBTreeBuilder(WEntity* pent, AABBTreeInfo& treeInfo);

		//Adds a model resource to the tree
		AABBTreeBuilder(ModelResource* res, AABBTreeInfo& heuristic);
		AABBTreeBuilder(std::vector<Vertex*>& vb, AABBTreeInfo& heuristic);

		//Returns the root node of the AABB Tree
		AABBNode* GetRootNode();

	private:
		GenericReference<AABBRoot> root;				//Root of the AABB Tree
		AABBTreeInfo treeInfo;		//Contains information/heuristics defined for this tree
	};
}

#endif