
#include "VertexTypes.h"
#include <math.h>
#include "godzassert.h"
#include <windows.h>
#include <CoreFramework/Math/GodzMath.h>


using namespace GODZ;

VertexArray::VertexArray()
: m_vertexList(NULL)
, m_numVerts(0)
, m_kVertexType(VT_Unknown)
{
}

VertexArray::VertexArray(const VertexArray& v)
: m_vertexList(NULL)
, m_numVerts(0)
, m_kVertexType(VT_Unknown)
{
	if (v.m_vertexList != NULL)
	{
		CreateVertexBuffer(v.m_numVerts, v.m_kVertexType);
		memcpy(m_vertexList, v.m_vertexList, v.GetVertexStructSize() * m_numVerts);
	}
}

VertexArray::~VertexArray()
{
	if (m_vertexList != NULL)
	{
		Vertex* verts = GetVertexBuffer();
		delete[] verts;
		m_vertexList = NULL;
	}
}

void VertexArray::CreateVertexBuffer(size_t vertex_count, EVertexType kVertexType)
{
	if (m_vertexList != NULL)
	{
		Vertex* verts = GetVertexBuffer();
		delete[] verts;
		m_vertexList = NULL;
	}

	m_numVerts = vertex_count;
	m_kVertexType = kVertexType;

	switch(this->m_kVertexType)
	{
	case VT_BlendVertex:
		{
			m_vertexList = (u8*)new BlendVertex[m_numVerts];
		}
		break;
	default:
		{
			m_vertexList = (u8*)new PerPixelVertex[m_numVerts];
		}
		break;
	}
}

Vertex* VertexArray::GetVertexBuffer()
{
	switch(m_kVertexType)
	{
	case VT_BlendVertex:
		return (BlendVertex*)m_vertexList;
		break;
	default:
		return (PerPixelVertex*)m_vertexList;
		break;
	}
	return NULL;
}

VertexArray& VertexArray::operator=(const VertexArray &v)
{
	CreateVertexBuffer(v.m_numVerts, v.m_kVertexType);
	memcpy(&m_vertexList[0], &v.m_vertexList[0], v.GetVertexStructSize() * m_numVerts);
	return *this;
}

udword VertexArray::GetVertexStructSize() const
{
	switch(m_kVertexType)
	{
	case VT_BlendVertex:
		{
			return sizeof(BlendVertex);
		}
		break;
	case VT_PerPixel:
		{
			return sizeof(PerPixelVertex);
		}
		break;

	default:
		{
			godzassert(0); //need to implement for this type
		}
		break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////


GODZ_API BaseVertex* GODZ::GetVertex(size_t index, Vertex* vertexList, EVertexType vertexType)
{
	switch(vertexType)
	{
	case VT_BlendVertex:
		{
			BlendVertex* bv = (BlendVertex*)vertexList;
			return &bv[index];
		}
		break;
	default:
		{
			PerPixelVertex* v = (PerPixelVertex*)vertexList;
			return &v[index];
		}
		break;
	}

	return NULL;
}

float Vertex::Get(int index)
{
	return pos[index];
}

BaseVertex::operator const Vector3() const
{
	return pos;
}

bool Vertex::operator==(const Vertex& Other) const
{
	for(int i=0; i<8;i++)
	{
		float f = fabsf(pos[i] - Other.pos[i]);
		if (f > EPSILON)
			return 0;
	}
	return true;
}

bool BlendVertex::operator==(const BlendVertex& Other) const
{
	for(int i=0; i<6;i++)
	{
		float f = fabsf(pos[i] - Other.pos[i]);
		if (f>EPSILON)
			return 0;
	}

	for(int i=0; i<4;i++)
	{
		float f = fabsf(Other.matrixIndicies[i] - matrixIndicies[i]);
		if (f>EPSILON)
			return 0;

		f = fabsf(Other.weights[i] - weights[i]);
		if (f>EPSILON)
			return 0;
	}

	return true;
}

BlendVertex& BlendVertex::operator=(const PerPixelVertex& v)
{
	pos = v.pos;
	normal = v.normal;
	u = v.u;
	this->v = v.v;

	tangent = v.tangent;
	binormal = v.binormal;

	return *this;
}

GODZ_API float GODZ::ComputeFaceCenter( Vertex vertices[3], int iAxis )
{
	return ( vertices[ 0 ].Get(iAxis) +
			 vertices[ 1 ].Get(iAxis) +
			 vertices[ 2 ].Get(iAxis) ) / 3.f;
}

bool PerPixelVertex::operator==(const PerPixelVertex& Other) const
{
	//figure out how many floats are in this struct...
	size_t bytes = sizeof(PerPixelVertex);
	size_t numFloats = bytes / sizeof(numFloats);

	for(size_t i=0; i<numFloats;i++)
	{
		float f = fabsf(pos[i] - Other.pos[i]);
		if (f>EPSILON)
			return 0;
	}

	return true;
}
