
#include "AnimationTable.h"
#include "AnimationInstance.h"
#include <CoreFramework/Core/Mesh.h>

using namespace GODZ;
using namespace std;


size_t AnimInfo::GetSequence()
{
	if (m_nSequenceCount > 0)
	{
		int index = rand() % m_nSequenceCount;
		godzassert(index < static_cast<int>(m_nSequenceCount));
		return this->m_nSequences[index];
	}

	return 0;
}

GDZArchive& GODZ::operator<<(GDZArchive& ar, AnimInfo* pInfo) 
{
	ar << pInfo->m_kAnimType;

	ar << pInfo->m_nSequenceCount;

	if (pInfo->m_nSequenceCount && !ar.IsSaving())
	{
		pInfo->m_nSequences = new size_t[pInfo->m_nSequenceCount];
	}

	for(size_t i=0;i<pInfo->m_nSequenceCount;i++)
	{
		ar << pInfo->m_nSequences[i];
	}

	return ar;
}

///////////////////////////////////////////////////////////////////////////

void AnimationTable::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	if (ar.IsSaving())
	{
		m_pAnims.Serialize(ar);
	}
	else
	{
		size_t numNodes;
		ar << numNodes;

		for(size_t i=0;i<numNodes;i++)
		{
			AnimInfo* pInfo = new AnimInfo;
			m_pAnims.Push(pInfo);
			ar << pInfo;
		}
	}
}


void AnimationTable::BuildAnims(Mesh* pMesh)
{
	for(GenericNode<AnimInfo*> *pNode=m_pAnims.GetFirst();pNode!=0;pNode=pNode->next)
	{
		delete pNode->data;
		pNode->data=NULL;
	}

	m_pAnims.Clear();

	//build the animation table
	AnimationInstance *animInstance = pMesh->GetAnimationInstance();
	if (animInstance)
	{
		//Load Available Animation Types
		ConfigSection* section = GlobalConfig::m_pConfig->GetSection("Sequences");
		if (section != NULL)
		{
			int size=section->GetNumOfProperties();
			for(int i=0;i<size;i++)
			{
				Str_Pair p = section->GetProperty(i);
				const char* value = p.second;

				AnimInfo* animInfo = new AnimInfo();
				HString animType(value);

				size_t sets = animInstance->GetNumOfAnimSets();
				for(udword j=0;j<sets;j++)
				{
					AnimSequence *seq = animInstance->GetSequence(j);
					if (seq->GetAnimType() == animType)
					{
						animInfo->m_nSequenceCount++;
						animInfo->m_kAnimType=animType;
					}
				} //loop

				if (animInfo->m_nSequenceCount>0)
				{
					//allocate the buffer
					animInfo->m_nSequences = new size_t[animInfo->m_nSequenceCount];
					for(u16 j=0;j<animInfo->m_nSequenceCount;j++)
					{
						animInfo->m_nSequences[j]=0;
					}

					int index=0;
					for(u16 j=0;j<animInstance->GetNumOfAnimSets();j++)
					{
						AnimSequence* seq = animInstance->GetSequence(j);
						if (seq->GetAnimType() == animType)
						{
							animInfo->m_nSequences[index] = j; //store seq #
							index++;
						}
					}

					m_pAnims.Push(animInfo);
				} //if	
				else
				{
					delete animInfo;
				}
			} //loop section properties
		} //if section
	} //if animInstance

	m_pAnims.BuildArray(); //optimize array access
}


AnimationTable::~AnimationTable()
{
	for(GenericNode<AnimInfo*> *pNode=m_pAnims.GetFirst();pNode!=0;pNode=pNode->next)
	{
		delete pNode->data;
		pNode->data=NULL;
	}
}


AnimInfo* AnimationTable::GetSequences(HashCodeID type)
{
	for(GenericNode<AnimInfo*> *pNode=m_pAnims.GetFirst();pNode!=0;pNode=pNode->next)
	{
		AnimInfo* pInfo = pNode->data;
		if (pInfo->m_kAnimType == type)
		{
			return pInfo;
		}
	}

	return NULL;
}

