
#include "OctreeSceneNode.h"
#include "WCamera.h"
#include "LevelNode.h"
#include <CoreFramework/Collision/PhysicsObject.h>

using namespace GODZ;
using namespace std;

OctreeSceneNode::OctreeSceneNode(const WBox& Box)
: iChildTrees(0)
{
	NodeFlags = SNF_Spatial; //This is a spatial sorting node
	if (Box.IsValid())
		SetBounds(Box);
}

bool OctreeSceneNode::IsEntityElgible(PhysicsObject* entity)
{
	CollisionPrimitive* model = entity->GetCollisionModel();

	//we place all entities inside the octree even if their box is invalid
	WBox EntityBound;
	model->GetBox(EntityBound);

	if (m_box.ContainsBox(EntityBound))
	{
		return true;
	}

	return false;
}

bool OctreeSceneNode::AddEntity(PhysicsObject* Entity)
{
	if (IsEntityElgible(Entity))
	{
		CollisionPrimitive* model = Entity->GetCollisionModel();

		//we place all entities inside the octree even if their box is invalid
		WBox EntityBound;
		model->GetBox(EntityBound);

		if (iChildTrees == 0 && ( m_box.GetSize() > MIN_NODE_SIZE ) //size >= MAX_ACTORS_PER_NODE &&
			)
		{
			//See if one of thez child boxes contain the box
			WBox childBox[8];
			CreateChildBounds(childBox);

			//Now query each child box to determine if one of them
			//contains the entity completely.
			int indexEntityContained = -1;
			for (int i = 0; i < 8; i++)
			{
				if (childBox[i].ContainsBox(EntityBound))
				{
					indexEntityContained=i;
					break;
				}
			}

			//If one of the children completely encapsulates the entity,
			//then create octants to encapsulate the entity.
			if (indexEntityContained > -1)
			{
				//If we can expand the octree still then do it
				if (m_bCanExpand)
				{
					SplitNode(childBox);
				}

				children[indexEntityContained]->AddEntity(Entity);
			}
			else
			{
				//store within this octant
				StoreElement(Entity);
			}

			return true;
		}
		else
		{
			StoreElement(Entity);
			return true;
		}
	}

	return false;
}

void OctreeSceneNode::CreateChildBounds(WBox childBox[8])
{
	Vector3 totalDist = m_box.GetMax() - m_box.GetMin();
	Vector3 boxSize = totalDist/2.0f;

	//Add the BOTTOM_BACK_LEFT Box
	Vector3 min = m_box.GetMin();
	Vector3 max = min + boxSize;
	childBox[0] = WBox(min, max);

	//BOTTOM_BACK_TOP
	min.y += boxSize.y;
	max.y += boxSize.y;
	childBox[1] = WBox(min, max);
		
	//TOP_BACK_RIGHT
	min.z += boxSize.z;
	max.z += boxSize.z;
	childBox[2] = WBox(min, max);

	//BOTTOM_BACK_RIGHT
	min.y -= boxSize.y;
	max.y -= boxSize.y;
	childBox[3] = WBox(min, max);

	//BOTTOM_FRONT_RIGHT
	min.x += boxSize.x;
	max.x += boxSize.x;
	childBox[4] = WBox(min, max);

	//TOP_FRONT_RIGHT
	min.y += boxSize.y;
	max.y += boxSize.y;
	childBox[5] = WBox(min, max);

	//TOP_FRONT_LEFT
	min.z -= boxSize.z;
	max.z -= boxSize.z;
	childBox[6] = WBox(min, max);

	//BOTTOM_FRONT_LEFT
	min.y -= boxSize.y;
	max.y -= boxSize.y;
	childBox[7] = WBox(min, max);
}

bool OctreeSceneNode::HasChildren()
{
	return iChildTrees > 0;
}

bool OctreeSceneNode::HasData()
{
	return m_objects.size() > 0;
}

void OctreeSceneNode::SplitNode(WBox childBox[8])
{
	//Reserve 8 slots of contigious memory slots (optimization)
	children.reserve(8);

	for (int i = 0; i < 8; i++)
	{
		OctreeSceneNode *treeNode = new OctreeSceneNode();
		godzassert(treeNode != NULL); //, "Out of Memory");
		treeNode->SetBounds(childBox[i]);
		treeNode->parent=this;
		treeNode->NodeFlags = NodeFlags;
		AddNode(treeNode); //append the child
		//Log("Child Octree Size(%f) min: %f %f %f max: %f %f %f\n", childBox[i].GetSize(), childBox[i].min.x, childBox[i].min.y, childBox[i].min.z, childBox[i].max.x, childBox[i].max.y, childBox[i].max.z);
	}

	iChildTrees=8; //this octant stores 8 child nodes
}


