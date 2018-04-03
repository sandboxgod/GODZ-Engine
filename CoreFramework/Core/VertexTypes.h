/* ===========================================================
	Vertex Types

	Defines the vertex formats 

	Created Sept 20, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__VERTEX_TYPES__)
#define __VERTEX_TYPES__

#include <CoreFramework/Math/Vector3.h>

namespace GODZ
{
	struct PerPixelVertex;
	
	//Vertex Types (Note: new types should be added towards the end so that old serialized files dont get fubared)
	enum EVertexType
	{
		VT_BaseVertex=0,
		VT_PerPixel,			//used by vertices that might be used for bump mapping
		VT_BlendVertex,			//currently unused
		VT_LightmapVertex,		//used by vertices that contain lightmap uv data
		VT_Unknown,
		VT_MAX			//Max # of vertex types
	};

	struct GODZ_API BaseVertex
	{
		Vector3 pos;

		BaseVertex(float x=0, float y=0, float z=0)
		{
			pos.x = x;  pos.y  = y;	pos.z  = z;
		}

		// Returns the position of this vertex.
		operator const Vector3() const;

		friend GDZArchive& operator<<(GDZArchive& ar, BaseVertex& v) 
		{
			ar << v.pos;
			return ar;
		}
	};

	// standard vertex type
	struct GODZ_API Vertex : public BaseVertex
	{		
		Vector3 normal;		//normal
		float u,v;			//texture u,v

		Vertex(): u(0), v(0)
		{}

		Vertex(float x, float y, float z)
		{
			pos.x  = x;  pos.y  = y;	pos.z  = z;
		}		

		//Returns the desired float member value (x,y,z,nx,ny,nz,u,v). For instance, to
		//get the value of x, index==0. To get the value of z, index==2.
		float Get(int index);

		void SetNormal(float nx, float ny, float nz)
		{
			normal.x = nx;
			normal.y = ny;
			normal.z = nz;
		}

		bool operator==(const Vertex& Other) const;

		friend GDZArchive& operator<<(GDZArchive& ar, Vertex& v) 
		{
			ar << v.normal << v.u << v.v << (BaseVertex)v;
			return ar;
		}
	};


	static const  int MAX_MATRIX_INDEX=4;


	struct GODZ_API PerPixelVertex : public Vertex
	{
		Vector3 tangent;	//for bump mapping / per pixel lighting
		Vector3 binormal;

		PerPixelVertex()
		{
			for(int i=0;i<3;i++)
			{
				tangent[i] = 0;
				binormal[i] = 0;
			}
		}

		//comparison to another vert
		bool operator==(const PerPixelVertex& Other) const;

		//copies a vertex struct
		PerPixelVertex& operator=(const Vertex& v)
		{
			pos = v.pos;
			normal = v.normal;
			u = v.u;
			this->v = v.v;

			return *this;
		}

		friend GDZArchive& operator<<(GDZArchive& ar, PerPixelVertex& v) 
		{
			ar << v.tangent << v.binormal << (Vertex)v;
			return ar;
		}
	};

	//skinned vertex
	struct GODZ_API BlendVertex : public Vertex
	{
		float weights[MAX_MATRIX_INDEX];			//skinned vertex weights
		float matrixIndicies[MAX_MATRIX_INDEX];		//matrix indicies
		Vector3 tangent;							//for bump mapping / per pixel lighting
		Vector3 binormal;

		BlendVertex()
		{
			for(int i=0;i<MAX_MATRIX_INDEX;i++)
			{
				weights[i] = 0;
				matrixIndicies[i] = 0;
			}
		}

		//copies a vertex struct
		BlendVertex& operator=(const Vertex& v)
		{
			pos = v.pos;
			normal = v.normal;
			u = v.u;
			this->v = v.v;

			return *this;
		}

		BlendVertex& operator=(const PerPixelVertex& v);

		//comparison to another Blended Vert
		bool operator==(const BlendVertex& Other) const;

		friend GDZArchive& operator<<(GDZArchive& ar, BlendVertex& v) 
		{
			for(int i=0;i<MAX_MATRIX_INDEX;i++)
			{
				ar << v.weights[i] << v.matrixIndicies[i];
			}

			ar << v.tangent << v.binormal << (Vertex)v;
			return ar;
		}
	};

	//Used by vertices that contain lightmap uv data
	struct GODZ_API LightmapVertex : public Vertex
	{
		float lu,lv;		//lightmap uv data

		LightmapVertex(): lu(0), lv(0)
		{
		}

		friend GDZArchive& operator<<(GDZArchive& ar, LightmapVertex& v) 
		{
			//todo: verify the BaseVertex data has been saved
			ar << v.lu << v.lv << (Vertex)v;
			return ar;
		}
	};

	/*
	* Vertex container
	*/
	struct GODZ_API VertexArray
	{
		VertexArray();
		VertexArray(const VertexArray& other);
		~VertexArray();

		void CreateVertexBuffer(size_t vertex_count, EVertexType kVertexType);
		udword GetVertexStructSize() const;
		Vertex* GetVertexBuffer();

		VertexArray& operator=(const VertexArray &v);	

		u8* m_vertexList;
		EVertexType		m_kVertexType;
		size_t			m_numVerts;			//# of vertices		
	};

	//param	vertices	Array of vertices referenced by given face.
	//param	iAxis		Index of the coordinate component to consider to compute center.
	//return			The coordinate of the triangle center on the requested axis.
	GODZ_API float ComputeFaceCenter( Vertex vertices[3], int iAxis );


	//Returns a vertex from a pool of vertices
	//vertexList - this is the entire vertex list.
	GODZ_API BaseVertex* GetVertex(size_t index, Vertex* vertexList, EVertexType vertexType);
}

#endif //__VERTEX_TYPES__