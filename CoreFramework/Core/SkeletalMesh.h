/* ==============================================================
	SkeletalMesh

	Created Mar 1, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#include "Mesh.h"
#include <CoreFramework/Animation/AnimationInstance.h>
#include "GenericNode.h"

#if !defined(__SKELETALMESH__H__)
#define __SKELETALMESH__H__

namespace GODZ
{
	//forward decl
	class SkelMeshInstance;

	//This is a snapshot of a mesh at certain time along an animation sequence
	struct GODZ_API SkelAnimFrame
	{
		float m_fTime;				//time this snapshot was taken
		BlendVertex *m_pVerts;	//state of the vertices at this frame

		SkelAnimFrame()
		{
			m_pVerts=0;
		}

		~SkelAnimFrame()
		{
			delete[] m_pVerts;
		}
	};

	//Represents all the skeletal animation snapshots for an animation sequence
	struct GODZ_API SkelAnimSeq
	{
		udword m_nSequenceIndex;			//animation sequence this snapshot was taken from
		float m_fps;						//fps
		bool m_bCompleteSet;
		NodeList<SkelAnimFrame*> m_pFrames;	//there will be a frame per second (if fps is 30, then 30 frames)

		SkelAnimSeq()
		{
			m_bCompleteSet=0;
		}

		//returns the frame closest to the time
		GenericNode<SkelAnimFrame*>* GetFrame(float time);

		~SkelAnimSeq()
		{
			for(GenericNode<SkelAnimFrame*> *pNode=m_pFrames.GetFirst();pNode!=0;pNode=pNode->next)
			{
				delete pNode->data;
				pNode->data=0;
			}
		}

	};


	//Represents a weighted vertex
	struct GODZ_API BlendWeight
	{
		BlendWeight()
			: vertexIndex(0),weight(0)
		{
		}
		BlendWeight(float weight, udword blendVertexId);

		udword vertexIndex;	//blend vertex id (actual vertex that we can pull from the vertex buffer)
		float weight;		//amount of influence exerted by the associated bone

		//serializes this object
		void Serialize(GDZArchive& ar);
	};

	struct GODZ_API VertexDupIndex
	{
		int		m_index;			//vertex index
		size_t	m_matIndex;			//material this vertex belongs to


		friend GDZArchive& operator<<(GDZArchive& ar, VertexDupIndex& index) 
		{
			ar << index.m_index << index.m_matIndex;
			return ar;
		}
	};

	//maps skin verts to their 'real' vertex buffer counterparts. Everytime a vertex
	//is specified by a face, it gets remapped during the import process (so the position will be 
	//the same but the uv/normals wont match). this structure
	//links the skin vertex to it's actual duplicates that we generated during the import 
	//process. For instance, in 3D Studio MAX the mesh might actually only contain 30 faces.
	//When this mesh gets imported- the engine creates vertex duplicates for the 
	//render API (like DirectX).
	struct GODZ_API VertexDuplication
	{
		Vector3			pos;				//original world pos (orig mesh)
		Vector3* 		m_pNormal;			//original normal for each Vertex
		Vector3* 		m_pTangent;		//original tangent for each Vertex
		Vector3* 		m_pBinormal;		//original binormal for each Vertex
		VertexDupIndex* m_indices;
		size_t			m_nNumIndices;

		
		VertexDuplication()
			: m_indices(0)
			, m_nNumIndices(0)
			, m_pNormal(0)
			, m_pTangent(0)
			, m_pBinormal(0)
		{
		}

		~VertexDuplication()
		{
			if (m_indices)
			{
				delete[] m_indices;
			}

			if (m_pNormal)
			{
				delete[] m_pNormal;
			}

			if (m_pTangent)
			{
				delete[] m_pTangent;
			}

			if (m_pBinormal)
			{
				delete[] m_pBinormal;
			}
		}

		void CreateIndices(size_t size)
		{
			m_indices = new VertexDupIndex[size];
			m_nNumIndices=size;

			m_pNormal = new Vector3[size];
			m_pTangent = new Vector3[size];
			m_pBinormal = new Vector3[size];
		}

		size_t GetNumIndices()
		{
			return m_nNumIndices;
		}

		VertexDupIndex& GetIndice(udword index)
		{
			return m_indices[index];
		}

		//serializes this object
		void Serialize(GDZArchive& ar);
	};

	//blendweight data per bone (the actual animated bone is stored in the skeletalmeshinstance)
	struct GODZ_API OrigBone
	{
		BlendWeight* m_blendVerts;
		size_t m_numBlendWeights;

		OrigBone()
		{
			m_blendVerts=0;
			m_numBlendWeights=0;
		}

		~OrigBone()
		{
			if (m_blendVerts)
			{
				delete[] m_blendVerts;
			}
		}


		void CreateBlendVerts(size_t num);

		BlendWeight* GetBlendWeight(size_t index)
		{
			return &m_blendVerts[index];
		}

		size_t GetNumBlendWeights()
		{
			return m_numBlendWeights;
		}

		void Serialize(GDZArchive& ar);
	};

	//Skeletal mesh object. The SkeletalMesh stores global data for each skeletalmeshinstance
	//such as blending information for each bone and vertex duplication (links the original skeletal
	//animation data to the 'optimized' mesh used by the engine).
	class GODZ_API SkeletalMesh : public Mesh
	{
		DeclareGeneric(SkeletalMesh, Mesh)

	public:
		SkeletalMesh();
		~SkeletalMesh();

		//Allocates the vertex duplication buffer
		void AllocVertexDuplication(size_t size);

		void AttachSkin();

		//creates the requested number of orig_bones
		void CreateOrigBones(size_t num);

		void CreateSkelAnimSequences(AnimController* pCntrl);

		bool IsSkelAnimSeqInitialized()
		{
			return m_pAnimSeq!=0;
		}

		SkelAnimSeq* GetSkelAnimSequence(size_t animSeqIndex);

		//Returns the Vertex Duplication buffer
		VertexDuplication* GetVertexDuplication();

		//returns a vertex duplication structure
		VertexDuplication* GetVertexDuplicate(UInt32 index)
		{
			return &duplicates[index];
		}

		//returns the max # of verts that a VertexDuplicate struct refers to. This is useful for
		//optimizing how big to allocate an array that refers to normal/indice 
		size_t GetMaxNumVertDupInd()
		{
			return m_nMaxNumDupInd;
		}

		size_t GetNumDuplicates()
		{
			return m_nNumDuplicates;
		}

		OrigBone* GetOrigBone(size_t index)
		{
			return &this->m_pBones[index];
		}

		bool HasCompleteSequence(AnimController* pCntrl);

		void SetMaxNumBones(unsigned char nMaxNumBones);

		//serializes this object
		void Serialize(GDZArchive& ar);

	protected:
		virtual bool ShouldAlwaysUseDefault() { return false; }
		virtual GenericClass* GetMeshType();

		VertexDuplication* duplicates;
		size_t m_nNumDuplicates;
		size_t m_nMaxNumDupInd;			//max # of indices stored in a vertex duplicate
		unsigned char m_nMaxNumBones;
		OrigBone* m_pBones;
		size_t m_nNumOrigBones;

		size_t numSequences;			//not serialized
		SkelAnimSeq* m_pAnimSeq;		//currently not serialized
	};
}

#endif