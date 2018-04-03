
#include "Skeleton.h"
#include <CoreFramework/Core/RenderDeviceEvent.h>
#include <CoreFramework/Core/SceneData.h>
#include <CoreFramework/Math/WMatrixUtil.h>


namespace GODZ
{

Skeleton::Skeleton()
: m_bones(NULL)
, m_numBones(0)
{
}

Skeleton::~Skeleton()
{
	if (m_bones != NULL)
	{
		delete[] m_bones;
		m_bones = NULL;
	}

	//TODO: Send request to remove SkeletonNodes...
}

void Skeleton::AddNode(const SkeletonNode& node)
{
	m_nodes.push_back(node);
	m_nameMap[ node.m_name.getHashCode() ] = &m_nodes[ m_nodes.size() - 1];
}

Bone* Skeleton::GetBone(size_t index)
{
	godzassert(index < m_numBones);
	return &m_bones[index];
}

void Skeleton::SetBones(Bone* bones, size_t num)
{
	m_bones = new Bone[num];
	m_numBones = num;

	//make private copy of the bones array...
	for(size_t i = 0; i < num; i++)
	{
		m_bones[i] = bones[i];

		//Add the nodes to name map
		for(size_t j = 0; j < m_bones[i].m_nodes.size(); j++)
		{
			m_nameMap[ m_bones[i].m_nodes[j].m_name.getHashCode() ] = &m_bones[i].m_nodes[j];
		}
	}
}

void Skeleton::InspectNodes(SkeletonNodeInspector& inspector)
{
	//for now we only update the base skeleton nodes and transform those to
	//world space
	std::vector<SkeletonNode>::iterator iter;
	for (iter = m_nodes.begin(); iter != m_nodes.end(); iter++)
	{
		SkeletonNodeInspector::Action act = inspector.inspect(*iter);

		switch(act)
		{
		case SkeletonNodeInspector::Action_CancelSearch:
			{
				//cancel search...
				return;
			}
			break;
		case SkeletonNodeInspector::Action_RemoveNode:
			{
				SkeletonNode& n = *iter;

				//drop the node from the name map
				SkeletonNodeMap::iterator mapIter = m_nameMap.find(n.m_name);
				if (mapIter != m_nameMap.end())
				{
					m_nameMap.erase(mapIter);
				}

				m_nodes.erase(iter);
				return;
			}
			break;
		}
	}
}

SkeletonNode* Skeleton::GetNodeByName(HString name)
{
	SkeletonNodeMap::iterator iter = m_nameMap.find(name.getHashCode());
	if (iter != m_nameMap.end())
	{
		return iter->second;
	}

	return NULL;
}

//Set initial transform --- moves entire skeleton relative
void Skeleton::SetInitTransform(const Vector3& pos, const WMatrix3& matrix)
{
	//for now we only update the base skeleton nodes and transform those to
	//world space
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		WMatrix m(matrix);
		m.SetTranslation(pos);
		m_nodes[i].m_primitive->TransformBy(m);
		Transform(m_nodes[i]);
	}
}

void Skeleton::TransformBy(const Vector3& pos)
{
	//for now we only update the base skeleton nodes
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		m_nodes[i].m_primitive->TransformBy(pos);
		Transform(m_nodes[i]);
	}
}

void Skeleton::TransformBy(const WMatrix3& rot, const WMatrix& parentMatrix)
{
	//Vector3 t = parentMatrix.GetTranslation();
	//Log("main parent %f %f %f \n", t.x, t.y, t.z);

	//for now we only update the base skeleton nodes and transform those to world space
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		//if we dont have a physics proxy then we do things locally...
		Vector3 p = m_nodes[i].m_offset;
		rot.Mul(p);

		WMatrix objectTm(1);
		CopyMatrix(m_nodes[i].m_initRot, objectTm);
		objectTm.SetTranslation( p );

		WMatrix final = parentMatrix * objectTm;
		Matrix3 final3(final);

		m_nodes[i].m_primitive->SetTransform( final.GetTranslation(), final3 );


		Transform(m_nodes[i]);
	}
}

void Skeleton::Transform(SkeletonNode& node)
{
	if (node.m_renderObject.isReady())
	{
		//Get the world space matrix from the primitive and use that.
		Vector3 p;
		WMatrix3 mt;
		node.m_primitive->GetTransform(p, mt);
		//node.m_proxy.GetWorldTransform(p, mt);

		UpdateMeshTransformEvent* um = new UpdateMeshTransformEvent();
		um->m_renderDeviceObject = node.m_renderObject.getValue();
		CopyMatrix(mt, um->tm);
		um->tm.SetTranslation(p);
		SceneData::AddRenderDeviceEvent( um );

	/*
		Log("physics child %f %f %f \n", p.x, p.y, p.z);

		node.m_primitive->GetTransform(p, mt);
		Log("main child %f %f %f \n", p.x, p.y, p.z);
		*/
		
	}
}

void Skeleton::GetSkeletonNodes(std::vector<SkeletonNode*>& primitives)
{
	//add nodes from bones...
	size_t num = m_nodes.size();
	for(size_t  i = 0; i < num; i++)
	{
		primitives.push_back( &m_nodes[i] );
	}
}

void Skeleton::ComputePrimitives( bool blockUntilDone )
{
	//1st submit resource requests....
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		m_nodes[i].m_renderObject = m_nodes[i].m_primitive->GetRenderObject();
	}

	//now block until done
	if (blockUntilDone)
	{
		for (size_t i = 0; i < m_nodes.size(); i++)
		{
			while(!m_nodes[i].m_renderObject.isReady());
		}
	}
}

}

