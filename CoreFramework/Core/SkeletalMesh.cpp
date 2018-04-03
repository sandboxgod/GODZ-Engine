
#include "SkeletalMesh.h"
#include "SkelMeshInstance.h"
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Reflection/GenericClass.h>


using namespace GODZ;
using namespace std;

GenericNode<SkelAnimFrame*>* SkelAnimSeq::GetFrame(float time)
{
	for(GenericNode<SkelAnimFrame*> *pNode=m_pFrames.GetFirst();pNode!=0;pNode=pNode->next)
	{
		if (time <= pNode->data->m_fTime)
		{
			return pNode;
		}
	}

	return 0;
}

void VertexDuplication::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	ar << pos;
	//ar << normal;
	ar << m_nNumIndices;

	if (!ar.IsSaving())
	{
		CreateIndices(m_nNumIndices);
	}

	for(udword i=0;i<m_nNumIndices;i++)
	{
		ar << m_indices[i];
		ar << m_pNormal[i];
		ar << m_pBinormal[i];
		ar << m_pTangent[i];
	}
}

//////////////////////////////////////////////////////////////////////

void OrigBone::CreateBlendVerts(size_t numBlendWeights)
{
	if (m_blendVerts != NULL)
	{
		delete[] m_blendVerts;
		m_blendVerts=0;
	}
	//
	m_numBlendWeights=numBlendWeights; 
	m_blendVerts = new BlendWeight[m_numBlendWeights]; //memory leak
}

void OrigBone::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	ar << m_numBlendWeights;

	if (!ar.IsSaving())
	{
		CreateBlendVerts(m_numBlendWeights);
	}

	for(udword i=0;i<m_numBlendWeights;i++)
	{
		m_blendVerts[i].Serialize(ar);
	}
}

////////////////////////////////////////////////////////////////////////////

BlendWeight::BlendWeight(float weight, udword blendVertexId)
{
	this->weight=weight;
	this->vertexIndex=blendVertexId;
}

void BlendWeight::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	ar << weight;
	ar << vertexIndex;
}

//////////////////////////////////////////////////////////////////////

ImplementGeneric(SkeletalMesh)

SkeletalMesh::SkeletalMesh()
: m_pBones(NULL)
, m_nMaxNumBones(0)
, duplicates(NULL)
, m_nNumDuplicates(0)
, m_nNumOrigBones(0)
, m_nMaxNumDupInd(0)
, m_pAnimSeq(NULL)
, numSequences(0)
{
}

SkeletalMesh::~SkeletalMesh()
{
	if (duplicates != NULL)
	{
		delete[] duplicates;
		duplicates = NULL;
	}

	if (m_pBones != NULL)
	{
		delete[] m_pBones;
		m_pBones = NULL;
	}

	if (m_pAnimSeq != NULL)
	{
		delete[] m_pAnimSeq;
		m_pAnimSeq = NULL;
	}
}

GenericClass* SkeletalMesh::GetMeshType()
{
	return SkelMeshInstance::GetClass();
}

void SkeletalMesh::AllocVertexDuplication(size_t size)
{
	if (duplicates != NULL)
	{
		delete[] duplicates;
		duplicates=0;
	}
	this->m_nNumDuplicates = size;
	duplicates = new VertexDuplication[size];
}

void SkeletalMesh::AttachSkin()
{
	size_t size = GetNumDuplicates();
	for(udword i=0;i<size;i++)
	{
		VertexDuplication* vd = GetVertexDuplicate(i);
		size_t numIndices = vd->GetNumIndices();
		if (numIndices>0)
		{
			if (numIndices>m_nMaxNumDupInd)
			{
				m_nMaxNumDupInd=numIndices;
			}

			const VertexDupIndex& index = vd->m_indices[0]; //just use one of the verts
			Material* m = m_pMeshInstance->GetMaterial(index.m_matIndex);
			ModelContainer* mc = m_pMeshInstance->GetModelResources(m);
			ModelResource* res = mc->get(0);
			godzassert(mc->size() == 1); //skeletal mesh expects a 1-to-1
			Vertex* bv = (Vertex*)res->GetVertex(index.m_index);
			vd->pos = *bv;

			for(size_t j=0;j<numIndices;j++)
			{
				VertexDupIndex index = vd->m_indices[j]; //just use one of the verts	
				
				Material* m = m_pMeshInstance->GetMaterial(index.m_matIndex);
				ModelContainer* mc = m_pMeshInstance->GetModelResources(m);
				godzassert(mc->size() == 1); //skeletal mesh expects a 1-to-1

				ModelResource *res = mc->get(0);
				BlendVertex* bv = (BlendVertex*)res->GetVertex(index.m_index);
				vd->m_pNormal[j] = bv->normal;
				vd->m_pTangent[j] = bv->tangent;
				vd->m_pBinormal[j] = bv->binormal;
			}
		}
	}
}

void SkeletalMesh::CreateSkelAnimSequences(AnimController* pCntrl)
{
	size_t numSets = pCntrl->GetAnimInstance()->GetNumOfAnimSets();
	numSequences = numSets;
	m_pAnimSeq = new SkelAnimSeq[ numSets ];
	for(size_t i=0;i<numSets;i++)
	{
		m_pAnimSeq[i].m_fps = pCntrl->GetFPS();
	}
}


bool SkeletalMesh::HasCompleteSequence(AnimController* pCntrl)
{
	int index = pCntrl->GetSequence();
	if (m_pAnimSeq == 0 || m_pAnimSeq[index].m_pFrames.GetNumItems() == 0)
	{
		return false;
	}

	return m_pAnimSeq[index].m_bCompleteSet;
}

void SkeletalMesh::CreateOrigBones(size_t num)
{
	if (m_pBones != NULL)
	{
		delete[] m_pBones;
		m_pBones = NULL;
	}
	m_nNumOrigBones=num;
	m_pBones = new OrigBone[num];
}

SkelAnimSeq* SkeletalMesh::GetSkelAnimSequence(size_t index)
{
	return &m_pAnimSeq[index];
}

VertexDuplication* SkeletalMesh::GetVertexDuplication()
{
	return duplicates;
}


void SkeletalMesh::Serialize(GDZArchive& ar)
{
	Mesh::Serialize(ar);

	ArchiveVersion version(1,1);
	ar << version;

	ar << m_nNumDuplicates;
	ar << m_nMaxNumDupInd;

	if (!ar.IsSaving())
	{
		this->AllocVertexDuplication(m_nNumDuplicates);
	}

	for(udword i=0;i<m_nNumDuplicates;i++)
	{
		duplicates[i].Serialize(ar);
	}

	ar << m_nMaxNumBones;
	ar << m_nNumOrigBones;

	if (m_nNumOrigBones && !ar.IsSaving())
	{
		CreateOrigBones(m_nNumOrigBones);
	}

	for(size_t i=0;i<m_nNumOrigBones;i++)
	{
		m_pBones[i].Serialize(ar);
	}
}

void SkeletalMesh::SetMaxNumBones(unsigned char nMaxNumBones)
{
	m_nMaxNumBones = nMaxNumBones;
}


