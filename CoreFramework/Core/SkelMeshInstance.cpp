
#include "SkelMeshInstance.h"
#include "SkeletalMesh.h"
#include "ModelResource.h"
#include "SceneManager.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "IAnimNotify.h"
#include "WEntity.h"
#include "Material.h"
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Reflection/GenericClass.h>


using namespace GODZ;
using namespace std;

///////////////////////////////////////////////////////////

ImplementGeneric(SkelMeshInstance)

SkelMeshInstance::SkelMeshInstance()
: m_nRootIndex(-1)
,m_pNorms(NULL)
,m_pTangents(NULL)
,m_pBinorms(NULL)
,m_pAnimInstance(NULL)
,m_boneTM(NULL)
,m_bones(NULL)
,m_numBones(0)
{
}


SkelMeshInstance::~SkelMeshInstance()
{
	if (m_bones)
	{
		delete[] m_bones;
		m_bones=NULL;
	}

	if (m_boneTM)
	{
		delete[] m_boneTM;
		m_boneTM=NULL;
	}

	if (m_pNorms)
	{
		delete[] m_pNorms;
		m_pNorms=0;
	}

	if (m_pBinorms)
	{
		delete[] m_pBinorms;
		m_pBinorms=0;
	}

	if (m_pTangents)
	{
		delete[] m_pTangents;
		m_pTangents=0;
	}
}

bool SkelMeshInstance::CreateBones(size_t numBones)
{
	if (m_bones != NULL)
		return false;

	m_numBones=numBones;
	m_bones = new Bone[numBones];
	m_boneTM = new WMatrix16f[numBones];

	return m_bones != NULL;
}

Skeleton* SkelMeshInstance::GetSkeleton()
{
	Skeleton* s = MeshInstance::GetSkeleton();
	s->SetBones(m_bones, m_numBones);
	return s;
}

WMatrix16f& SkelMeshInstance::GetBoneTM(size_t index)
{
	godzassert(index<m_numBones);
	return m_boneTM[index];
}

Bone* SkelMeshInstance::GetBone(size_t boneIndex)
{
	godzassert(boneIndex<GetNumOfBones());
	return &m_bones[boneIndex];
}

size_t SkelMeshInstance::GetNumOfBones()
{
	return m_numBones;
}

Bone* SkelMeshInstance::GetRootBone()
{
	return &m_bones[m_nRootIndex];
}

int SkelMeshInstance::GetRootBoneIndex()
{
	return m_nRootIndex;
}

Bone* SkelMeshInstance::FindBone(HashCodeID name)
{
	for (udword i=0;i<GetNumOfBones();i++)
	{
		Bone* bone = GetBone(i);
		if (bone->GetName() == name)
		{
			return bone;
		}
	}

	return NULL;
}

bool SkelMeshInstance::FindBone(HashCodeID name, UInt32& index)
{
	for (udword index=0;index<GetNumOfBones();index++)
	{
		Bone* bone = GetBone(index);
		if (bone->GetName() == name)
		{
			return true;
		}
	}

	return false;
}

void SkelMeshInstance::Initialize(Mesh* mesh)
{
	MeshInstance::Initialize(mesh);

	SkeletalMesh* owner = SafeCast<SkeletalMesh>(mesh);
	m_pAnimInstance = owner->GetAnimationInstance();
	if (m_pAnimInstance == NULL)
	{
		owner->CreateAnimationInstance();
		m_pAnimInstance = owner->GetAnimationInstance();
	}
}


void SkelMeshInstance::Transform(float deltaTime, Skeleton* skeleton, AnimController* animController)
{
	SceneManager* smgr = SceneManager::GetInstance();
	const SceneManagerConfigData& config = smgr->GetSceneManagerConfigData();

	float animdeltat = animController->IsFrozen() ? 0 : deltaTime;

	if (animController != NULL
		&& animController->GetSequence() > -1
		&& animController->IsAnimationAdvanced(animdeltat)
		)
	{
		//compute all the bone matrices (world/offset)
		float time = animController->GetTime();

		SkeletalMesh* skMesh=SafeCast<SkeletalMesh>(mesh);

		if ( !config.m_bUseSavedSkelFrames 
			|| !skMesh->HasCompleteSequence(animController)
			)
		{
			if (config.m_bUseSavedSkelFrames && !skMesh->IsSkelAnimSeqInitialized())
			{
				skMesh->CreateSkelAnimSequences(animController);
			}

			//transform the bones...
			Bone* root = skeleton->GetBone(m_nRootIndex);
			root->Transform(time, animController, m_nRootIndex, config.m_bUseHWSkinning, this, skeleton);
		}
	}
}


void SkelMeshInstance::RenderCollisionVolumes(SceneInfo& sceneInfo)
{
}


void SkelMeshInstance::SetParentForBone(udword parentIndex, udword boneIndex)
{
	godzassert(boneIndex < GetNumOfBones());
	godzassert(parentIndex < GetNumOfBones());

	m_bones[boneIndex].SetParentBone(parentIndex);
}

void SkelMeshInstance::Serialize(GDZArchive& ar)
{
	MeshInstance::Serialize(ar);

	ArchiveVersion version(1,2);
	ar << version;

	ar << m_nRootIndex;
	ar << m_numBones;

	if (!ar.IsSaving())
	{
		CreateBones(m_numBones);
	}

	for(size_t i=0;i<m_numBones;i++)
	{
		if (ar.IsSaving())
		{
			Bone& bone = m_bones[i];
			bone.Serialize(ar);
		}
		else
		{
			m_bones[i].Serialize(ar);
		}
	}

	//old stuff....
	if (version.m_nCurrentVersion < 2)
	{
		size_t unused_hitSpheres = 0;
		ar << unused_hitSpheres;

		for(UInt32 i=0;i<unused_hitSpheres;i++)
		{
			Vector3 rel;
			float radius;
			ar << rel;
			ar << radius;
		}
	}
}


void SkelMeshInstance::UpdateHierarchy()
{

	size_t size = this->GetNumOfBones();
	for (size_t i=0;i<size;i++)
	{
		Bone* b = this->GetBone(i);
		if (b->GetParent(this)==NULL)
		{
			//m_RootBone=b;
			m_nRootIndex=static_cast<udword>(i);
			Log("Root Bone is %s\n", b->GetName().c_str());
		}

		for (size_t j=0;j<size;j++)
		{
			Bone* child = this->GetBone(j);
			if (b!=child && child->GetParent(this) == b)
			{
				int index = static_cast<int>(j);
				b->AddBone(index);
				Log("Bone %s is child of %s\n", child->GetName().c_str(), b->GetName().c_str());
			}
		}
	}
}
