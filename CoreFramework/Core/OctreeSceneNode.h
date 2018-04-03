/* =======================================================================
	Spatial Sorting System

	This octree stores references to colliding entities. 

	Created Jan 3, '04 by Richard Osborne
	Copyright (c) 2010
	=======================================================================
*/

#if !defined(_OctreeSceneNode_H_)
#define _OctreeSceneNode_H_

#include "SceneNode.h"
#include "WEntity.h"

namespace GODZ
{
	class GODZ_API OctreeSceneNode : public SceneNode
	{
	public:
		OctreeSceneNode(const WBox& Box=EMPTY_BOX);

		//Returns true if this entity could be added to this
		//scene node.
		virtual bool AddEntity(PhysicsObject* Entity);

		//Returns true if this octant contains any data.
		virtual bool HasData();

		//Returns true if this tree node contains child tree nodes.
		virtual bool HasChildren();

		virtual bool IsEntityElgible(PhysicsObject* entity);

	protected:
		int iChildTrees;		//Child Octrees
		static const int MIN_NODE_SIZE=100;			//minimum volume size of an octant

		//Creates child boxes based on the bounding box of this node.
		//[in] childBox - child bounding box
		void CreateChildBounds(WBox childBox[8]);

	private:

		//Splits this octant into 8 leaves. All entities that belong to this
		//node are pushed into the leaves (if they can be moved).
		void SplitNode(WBox childBox[8]);
	};
}

#endif