/* ===========================================================
	Bone.h

	Copyright (c) 2011
	========================================================== 
*/

#include "Bone.h"
#include <CoreFramework/Collision/CollisionPrimitive.h>
#include <CoreFramework/Core/HString.h>
#include <CoreFramework/Core/SkelMeshInstance.h>
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Math/WMatrix.h>


namespace GODZ
{

//This flag turns on an alternative CPU skinning method which transforms each 
//skin vertex relative to a bone.
const static bool bUSE_SKIN_VERTS = 0;
//#define USE_HIERARCHY;

void GODZ::BlendVertexNew(Vertex& result, const Vertex& v, const WMatrix& mat0, 
						  const WMatrix& mat1, float fWeight) //D3DVECTOR* normal
{
	Vector3	locVec0(v.pos.x,v.pos.y,v.pos.z);
	Vector3 locVec1(v.pos.x,v.pos.y,v.pos.z);


	mat0.Mul(locVec0);
	mat1.Mul(locVec1);

	// blend vertex
	result.pos.x = (fWeight * locVec0.x) + ((1.0f-fWeight)*(locVec1.x)) ;
	result.pos.y = (fWeight * locVec0.y) + ((1.0f-fWeight)*(locVec1.y)) ;
	result.pos.z = (fWeight * locVec0.z) + ((1.0f-fWeight)*(locVec1.z)) ;
}

//////////////////////////////////////////////////////////////////////////


void GODZ::BoneMul(Bone* bone, Vector3* v, float weight, WMatrix16f& boneTM)
{
	/*
	//Transform the vertex relative to the bone
	bone->m_init.InverseTranslateVect(&v->x);
	bone->m_init.RotateVect(&v->x);

	//Transform the vertex to it's final position
	Vector3 localPos = *v;
	bone->m_final.Mul(&localPos.x);
*/

	
	Vector3 localPos = *v;
	boneTM.Mul(localPos);

	//multiply the vertex by it's weight for this bone
	v->x = localPos.x * weight;
	v->y = localPos.y * weight;
	v->z = localPos.z * weight;
	
}


//////////////////////////////////////////////////////////////////////////

Bone::Bone()

//Note: probably dont need to auto-init these
: m_initInv(1)
, m_init(1)
, m_frame(1)
, m_final(1)
{
}

Bone::Bone(const char* Name)
: parentIndex(-1)

//Note: probably dont need to auto-init these
, m_initInv(1)
, m_init(1)
, m_frame(1)
, m_final(1)
, m_sName(Name)
{
}

Bone::~Bone()
{
	m_nodes.clear();
}

void Bone::AddBone(int boneIndex)
{
	children.Push(boneIndex);
}


Bone* Bone::GetParent(SkelMeshInstance* skMesh)
{
	if (parentIndex==-1)
		return NULL;

	return skMesh->GetBone(parentIndex);
}

bool Bone::IsChild(int boneIndex)
{
	size_t s = children.GetNumItems();
	for(size_t i=0;i<s;i++)
	{
		if (children[i] == boneIndex)
		{
			return true;
		}
	}

	return 0;
}

void Bone::SetParentBone(int parentIndex)
{
	this->parentIndex=parentIndex;
}

void Bone::Serialize(GDZArchive& ar)
{
	ArchiveVersion version(1,3);
	ar << version;

	if (!ar.IsSaving() && version.m_nCurrentVersion < 2)
	{
		rstring name;
		ar << name;
		m_sName = HString(name);
	}
	else
	{
		ar << m_sName;
	}

	ar << m_init << m_frame << m_final;
	ar << parentIndex;

	if (version.m_nCurrentVersion < 3)
	{
		int unused_collisionIndex;
		ar << unused_collisionIndex;
	}
	else
	{
		//save nodes...
		size_t numNodes = m_nodes.size();
		ar << numNodes;

		for (size_t i = 0; i < numNodes; i++)
		{
			if(ar.IsSaving())
			{
				ar << m_nodes[i];
			}
			else
			{
				SkeletonNode node;
				m_nodes.push_back(node);

				//This way we only allocate primitives, etc once
				ar << m_nodes[m_nodes.size() - 1];
			}
		}
	}

	if(!ar.IsSaving())
	{
		//compute inverse of the initial matrix
		m_init.Inverse(m_initInv);
	}

	if (version.m_nCurrentVersion < 3)
	{
		WBox unused_box;
		unused_box.Serialize(ar);
	}

	children.Serialize(ar);
}


void Bone::Transform(float dt, AnimController* animControl, udword boneIndex, bool bUsingSkinMethod, SkelMeshInstance* skMesh, Skeleton* sk)
{
	animControl->Update(this, dt, boneIndex);

	WMatrix oldTM = m_final;

	if (parentIndex > -1)
	{
#if defined(USE_HIERARCHY)
		{
			Bone* parent = skMesh->GetBone(parentIndex);

			//compute matrix relative to parent 
			WMatrix16f pInvMat(1);
			parent->m_final.Inverse(pInvMat);
			WMatrix16f pRelative = m_frame * pInvMat;

			//compute final transformation matrix
			m_final = pRelative * parent->m_final;
		}
#else
		{
			m_final = m_frame;  //---use local matrix directly (non-hierachial animation solution)
		}
#endif

		//transforms vertex relative to initial bone then towards m_final
		WMatrixMul(m_initInv, m_final, m_offsetTM);
	}


	//Transform attached collision models within our local space
	if (m_nodes.size() > 0)
	{
		//compute offset matrix (delta from last frame to this one)
		WMatrix inv;
		oldTM.Inverse(inv);
		WMatrix offset = inv * m_final;

		for (size_t i = 0; i < m_nodes.size(); i++)
		{
			m_nodes[i].m_primitive->TransformBy(offset);
		}
	}

	
	if (!bUSE_SKIN_VERTS)
	{
		if (parentIndex == -1 && ( !bUsingSkinMethod))
		{
			MeshInstance::MeshMap& map = skMesh->GetMeshMap();
			MeshInstance::MeshMap::iterator iter;

			for(iter = map.begin(); iter != map.end(); iter++)
			{
				ModelContainer* mc = iter->second;
				size_t numResources = mc->size();
				for(size_t i=0;i<numResources;i++)
				{
					//Root bone - reset verts
					ModelResource* resource = mc->get(i);
					size_t numVerts=resource->GetNumVertices();
					godzassert(resource->GetVertexType() == VT_BlendVertex); //entire skelmeshinstance code relies on this type

					for(size_t i=0;i<numVerts;i++)
					{
						BlendVertex* v = (BlendVertex*)resource->GetVertex(i);

						v->pos.x=0;
						v->pos.y=0;
						v->pos.z=0;

						v->normal.x=0;
						v->normal.y=0;
						v->normal.z=0;

						v->binormal[0] = 0;
						v->binormal[1] = 0;
						v->binormal[2] = 0;

						v->tangent[0] = 0;
						v->tangent[1] = 0;
						v->tangent[2] = 0;
					}
				}
			} // loop mesh iterator
		}
	}
	
	SkeletalMesh* mesh = SafeCast<SkeletalMesh>(skMesh->GetMesh());

	
	if (!bUSE_SKIN_VERTS && !bUsingSkinMethod) 
	{
		
		//Software Skinning....
		OrigBone *pBone = mesh->GetOrigBone(boneIndex);
		size_t numWeights = pBone->GetNumBlendWeights();

		
		//Transform the normal (for each vertex-dup). Recall the position is the same for each vertex
		//dup (so we only transform that once). However, the u & v texcoords + normals might possibly
		//differ for each dup.
		Vector3* norm = skMesh->GetNormalsArray(); //reuse this array to store normals
		Vector3* binorm = skMesh->GetBinormalsArray();
		Vector3* tangent = skMesh->GetTangentsArray();

		
		for(udword i=0;i<numWeights;i++)
		{
			
			//NOTE: I found that if any STL::Vectors are accessed in this loop performance slows 
			//down dramatically. Avoid using std::vectors  in this loop.

			//TODO: Need to free m_blendVerts (well move it to skeletal mesh)
			if (pBone->m_blendVerts[i].weight < 0.000001f)
			{
				continue;
			}

			
			VertexDuplication *vd = mesh->GetVertexDuplicate(pBone->m_blendVerts[i].vertexIndex);
			WMatrix16f& boneTM = skMesh->GetBoneTM(boneIndex);

			//get this vertex world pos (from the initial pose)
			Vector3 pos(vd->pos.x,vd->pos.y,vd->pos.z);
			BoneMul(this, &pos, pBone->m_blendVerts[i].weight, boneTM);

			size_t numInds = vd->GetNumIndices();
			

			//note, we only need to transform the normals for lighting
			for(size_t j=0;j<numInds;j++)
			{
				VertexDupIndex index = vd->m_indices[j];
				Material* m = skMesh->GetMaterial(index.m_matIndex);
				ModelContainer* mc = skMesh->GetModelResources(m);

				godzassert(mc->size() == 1); //skmesh should only be 1 per mat
				ModelResource* resource = mc->get(0);

				BlendVertex *vec = (BlendVertex*)resource->GetVertex(index.m_index);	
				vec->pos.x += pos.x;
				vec->pos.y += pos.y;
				vec->pos.z += pos.z;

				//get initial normal/binorm/tangent
				norm[j] = vd->m_pNormal[j];
				binorm[j] = vd->m_pBinormal[j];
				tangent[j] = vd->m_pTangent[j];

				//rotate the normal for this vertex
				Vector3 localPos = norm[j];
				boneTM.RotateVect(localPos);

				norm[j].x = localPos.x * pBone->m_blendVerts[i].weight;
				norm[j].y = localPos.y * pBone->m_blendVerts[i].weight;
				norm[j].z = localPos.z * pBone->m_blendVerts[i].weight;


				vec->normal += norm[j];

				//rotate the binormal for this vertex
				localPos = binorm[j];
				boneTM.RotateVect(localPos);

				binorm[j].x = localPos.x * pBone->m_blendVerts[i].weight;
				binorm[j].y = localPos.y * pBone->m_blendVerts[i].weight;
				binorm[j].z = localPos.z * pBone->m_blendVerts[i].weight;

				vec->binormal += binorm[j];

				//rotate the tangent for this vertex
				localPos = tangent[j];
				boneTM.RotateVect(localPos);

				tangent[j].x = localPos.x * pBone->m_blendVerts[i].weight;
				tangent[j].y = localPos.y * pBone->m_blendVerts[i].weight;
				tangent[j].z = localPos.z * pBone->m_blendVerts[i].weight;

				vec->tangent += tangent[j];
			}
			
			
			//godzassert(resource->GetVertexType() == VT_PerPixel);
			
		} //loop
	}

	for(GenericNode<int> *pNode=children.GetFirst();pNode!=0;pNode=pNode->next)
	{
		//update child transformations
		Bone* child = sk->GetBone(pNode->data);
		child->Transform(dt, animControl, pNode->data, bUsingSkinMethod, skMesh, sk);
	}
}


void Bone::UpdateBound()
{

}


}

