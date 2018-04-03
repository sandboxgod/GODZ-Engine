/* ===========================================================
	AnimationTable

	The engine allows characters to associate animation 
	sequences with animation tables. This way we can associate
	more than one animation sequence with a specific animation
	type.

	Created Sept 4, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__ANIMATION_TABLE_H__)
#define __ANIMATION_TABLE_H__

#include <CoreFramework/Core/GenericNode.h>

namespace GODZ
{
	class AnimSequence;
	class Mesh;

	//This struct stores info about a group of animation sequences
	struct GODZ_API AnimInfo
	{
		AnimInfo()
			: m_kAnimType(0)
			, m_nSequenceCount(0)
			, m_nSequences(0)			
		{
		}

		AnimInfo(const AnimInfo& other)
			: m_kAnimType(0)
			, m_nSequenceCount(0)
			, m_nSequences(0)			
		{
			m_kAnimType=other.m_kAnimType;
			m_nSequenceCount=other.m_nSequenceCount;

			if (m_nSequenceCount)
			{
				this->m_nSequences = new size_t[m_nSequenceCount];
				for(size_t i=0;i<m_nSequenceCount;i++)
				{
					m_nSequences[i] = other.m_nSequences[i];
				}
			}
		}

		//randomly selects a sequence
		size_t GetSequence();

		
		AnimInfo& operator=(const AnimInfo& inf);
		~AnimInfo()
		{
			if (m_nSequences)
			{
				delete[] m_nSequences;
			}
		}

		friend GDZArchive& operator<<(GDZArchive& ar, AnimInfo* pInfo);

		HashCodeID m_kAnimType;		//animation group. See EAnimType
		size_t m_nSequenceCount;	//number of sequences
		size_t* m_nSequences;		//animation sequence id for this specific type		
	};

	//stores information about all the animation sequences stored in a mesh
	struct GODZ_API AnimationTable
	{
		~AnimationTable();

		//generates animation sequence information
		void BuildAnims(Mesh* pMesh);

		//serializes this struct
		void Serialize(GDZArchive& ar);

		//returns all the animation sequences for the desired type
		//[in] type - animation type
		AnimInfo* GetSequences(HashCodeID type);

		NodeList<AnimInfo*> m_pAnims;
	};
}

#endif //__ANIMATION_TABLE_H__
