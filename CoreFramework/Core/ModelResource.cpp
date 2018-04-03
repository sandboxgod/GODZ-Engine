
#include "ModelResource.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "IReader.h"
#include "SerializedContentHandler.h"
#include "Material.h"
#include "ShaderResource.h"
#include "SceneManager.h"
#include "crc32.h"
#include "SceneData.h"
#include "debug.h"
#include <CoreFramework/Collision/AABBTree.h>

using namespace GODZ;
using namespace std;


ModelResource::ModelResource(HashCodeID hash, EVertexType vertexType)
: vertex_count(0)
, index_count(0)
, m_pPolyList(NULL)
, m_pMaterialList(NULL)
, m_visible(true)
, ResourceObject(hash)
, m_kVertexType(vertexType)
, m_pCollTree(NULL)
, mIsRecomputingNormals(true)
{
	m_Flag = RF_Model;
}

ModelResource::~ModelResource()
{
	if (m_pPolyList!=NULL)
	{
		delete[] m_pPolyList;
		m_pPolyList=NULL;
	}
	if (m_pMaterialList!=NULL)
	{
		delete[] m_pMaterialList;
		m_pMaterialList=NULL;
	}

	
	m_pCollTree = NULL;
}


void ModelResource::BatchShaders()
{
}

void ModelResource::CreateMeshChunks(BlendVertex* pVertexBuffer)
{
}

void ModelResource::CreateMesh()
{
	CreateVertices(vertex_count);
	CreateIndexBuffer(index_count);
	CreateAttributeBuffer(index_count/3);
}

ModelResource* ModelResource::CreateModelResource(std::vector<Vertex>& vertexBuffer, std::vector<u16>& indexBuffer, std::vector<ulong>& attributeBuffer) 
{
	return CreateModelResourceInMemory(vertexBuffer, indexBuffer, attributeBuffer);
}


ModelResource* ModelResource::CreateModelResourceInMemory(std::vector<Vertex>& vertexBuffer, std::vector<u16>& indexBuffer, std::vector<ulong>& attributeBuffer) 
{
	size_t size = vertexBuffer.size();
	vertex_count = size;
	CreateVertices(size);
	CreateIndexBuffer(indexBuffer.size());

	for(size_t i=0;i<size;i++)
	{
		SetVertex(i, vertexBuffer[i]);
	}

	size_t count = 0;
	vector<WORD>::iterator iIter;
	for(iIter=indexBuffer.begin();iIter!=indexBuffer.end();iIter++)
	{
		SetIndex(count, (*iIter));
		count++;
	}

	ComputeBounds();

	return this;
}

ModelResource* ModelResource::CreateModelResourceInMemory(std::vector<PerPixelVertex>& vertexBuffer, std::vector<u16>& indexBuffer) 
{
	size_t size = vertexBuffer.size();
	vertex_count = size;
	CreateVertices(size);
	CreateIndexBuffer(indexBuffer.size());

	for(size_t i=0;i<size;i++)
	{
		SetVertex(i, vertexBuffer[i]);
	}

	size_t count = 0;
	vector<WORD>::iterator iIter;
	for(iIter=indexBuffer.begin();iIter!=indexBuffer.end();iIter++)
	{
		SetIndex(count, (*iIter));
		count++;
	}

	ComputeBounds();

	return this;
}

void ModelResource::CreateAttributeBuffer(size_t size_of_buffer)
{
	m_pMaterialList = new DWORD[size_of_buffer];
}

void ModelResource::CreateVertices(size_t number_of_verts)
{
	m_pVertexList.CreateVertexBuffer(number_of_verts, m_kVertexType);
}

void ModelResource::CreateIndexBuffer(size_t number_of_polys)
{
	m_pPolyList = new WORD[number_of_polys];
	godzassert(m_pPolyList != NULL);
	index_count=number_of_polys;
}

//callers need to also update the mesh bound....
void ModelResource::ComputeBounds()
{
	switch(m_kVertexType)
	{
	case VT_BlendVertex:
		{
			BlendVertex* bv = (BlendVertex*)m_pVertexList.m_vertexList;
			Vector3 tmin, tmax;
			SetMinMaxBoundVerts<BlendVertex>(bv, vertex_count, tmin, tmax);
			m_box.SetMinMax(tmin, tmax);
		}
		break;
	default:
		PerPixelVertex* vx = (PerPixelVertex*)m_pVertexList.m_vertexList;

		Vector3 tmin, tmax;
		SetMinMaxBoundVerts<PerPixelVertex>(vx, vertex_count, tmin, tmax);
		m_box.SetMinMax(tmin, tmax);

		break;
	}
}

void ModelResource::ComputeCollisionTree(int max_tree_depth)
{
	if (m_pCollTree != NULL)
		return; //tree already generated.

	//Generate an AABB Tree for the model.
	AABBTreeInfo info;
	info.max_tree_depth=max_tree_depth;
	info.min_vertices=9;
	AABBTreeBuilder builder(this,info);
	m_pCollTree = builder.GetRootNode(); //assign to smart pointer
}

//destroys the collision tree
void ModelResource::DestroyCollisionTree()
{
	m_pCollTree = NULL;
}

DWORD* ModelResource::GetAttributeBuffer()
{
	return m_pMaterialList;
}

AABBNode* ModelResource::GetCollisionTree() const
{
	return m_pCollTree;
}

BaseVertex* ModelResource::GetVertex(size_t index) const
{
	switch(m_kVertexType)
	{
	case VT_BlendVertex:
		{
			BlendVertex* bv = (BlendVertex*)m_pVertexList.m_vertexList;
			return &bv[index];
		}
		break;
	default:
		{
			PerPixelVertex* v = (PerPixelVertex*)m_pVertexList.m_vertexList;
			return &v[index];
		}
		break;
	}

	return NULL;
}

// Returns vertex index for this face
WORD& ModelResource::GetIndex(size_t index) const
{
	return m_pPolyList[index];
}


size_t ModelResource::GetNumVertices() 
{ 
	return vertex_count; 
}

		
size_t ModelResource::GetNumIndicies() 
{ 
	return index_count; 
}

Vertex* ModelResource::GetVertexBuffer()
{
	switch(m_Flag)
	{
	case ResourceObject::RF_Skin:
		return (BlendVertex*)m_pVertexList.m_vertexList;
		break;
	default:
		return (PerPixelVertex*)m_pVertexList.m_vertexList;
		break;
	}
	return 0;
}

WORD* ModelResource::GetIndexBuffer()
{
	return m_pPolyList;
}

bool ModelResource::HasLeafNodes()
{
	return m_pCollTree != NULL;
}

udword ModelResource::GetVertexStructSize() const
{
	return m_pVertexList.GetVertexStructSize();
}


//Submits a request to render thread for a render device (renderable object)
void ModelResource::CreateRenderDeviceObject()
{
	RenderDeviceEvent* event = NULL;

	VertexParams vp;

	//We send the render thread a 'copy' of our personal memory. This way we
	//can guarantee thread-safety for our data
	vp.m_vb = m_pVertexList;

	vp.m_ib.reserve(index_count);
	for(size_t i=0;i<index_count;i++)
	{
		vp.m_ib.push_back( m_pPolyList[i] );
	}

	vp.m_ResourceType = m_Flag;
	vp.m_numVerts = static_cast<ulong>(vertex_count);
	vp.m_numInd = static_cast<ulong>(index_count);
	vp.m_vertexStructSize = GetVertexStructSize();
	vp.m_kVertexType = this->m_kVertexType;

	event = CreateModelEvent(m_renderDeviceObject, vp, m_sFilename, mIsRecomputingNormals);
	SceneData::AddRenderDeviceEvent(event);
}

void ModelResource::Serialize(GDZArchive& ar)
{
	ResourceObject::Serialize(ar);

	ArchiveVersion version(1,4);
	ar << version;

	if (version.m_nCurrentVersion == 2)
	{
		//not sure why I was serializing this but it can be removed....
		ar << m_visible;
	}

	ar << vertex_count << index_count;

	int nVertexType = static_cast<int>(m_kVertexType);
	ar << nVertexType;

	m_kVertexType = static_cast<EVertexType>(nVertexType);

	if (ar.IsSaving())
	{
		udword vertexStructSize=GetVertexStructSize();
		IWriter* writer = ar.GetWriter();
		size_t items=writer->WriteData(m_pVertexList.m_vertexList, vertexStructSize, vertex_count);
		godzassert(items==vertex_count);
		items=writer->WriteData(m_pPolyList, sizeof(WORD), index_count);
		godzassert(items==index_count);

		bool hasTree = (m_pCollTree != NULL);
		ar << hasTree;

		if (m_pCollTree != NULL)
		{
			m_pCollTree->Serialize(ar);
		}
	}
	else
	{
		IReader* reader = ar.GetReader();

		m_pVertexList.CreateVertexBuffer(vertex_count, m_kVertexType);
		udword vertexStructSize=GetVertexStructSize();

		m_pPolyList = new WORD[index_count];

		size_t items = reader->ReadData(m_pVertexList.m_vertexList, vertexStructSize, vertex_count);
		godzassert(items==vertex_count);
		reader->ReadData(m_pPolyList, sizeof(WORD), index_count);

		//read in attribute buffer if it's an old file
		if (version.m_nCurrentVersion < 2)
		{
			m_pMaterialList = new DWORD[index_count/3];
			reader->ReadData(m_pMaterialList, sizeof(DWORD), index_count/3);
		}

		//TODO: We should be saving our bounds
		ComputeBounds();

		//Allocate the video card resources
		CreateRenderDeviceObject();

		bool hasTree;
		ar << hasTree;

		if (hasTree)
		{
			m_pCollTree = new AABBRoot();
			m_pCollTree->Serialize(ar);
		}

		/*
		//output vertices --- debug
		std::vector<Vector3> list;
		for(UInt32 i = 0; i < index_count; i+= 3)
		{
			int idx0 = GetIndex(i);
			int idx1 = GetIndex(i+1);
			int idx2 = GetIndex(i+2);


			BaseVertex* v0 = GetVertex(idx0);
			BaseVertex* v1 = GetVertex(idx1);
			BaseVertex* v2 = GetVertex(idx2);

			Log("%f %f %f ", v0->pos.x, v0->pos.y, v0->pos.z);
			Log("%f %f %f ", v1->pos.x, v1->pos.y, v1->pos.z);
			Log("%f %f %f ", v2->pos.x, v2->pos.y, v2->pos.z);
			Log("\n");

			list.push_back(Vector3(v0->pos));
			list.push_back(Vector3(v1->pos));
			list.push_back(Vector3(v2->pos));
		}

		Vector3 min, max;
		SetMinMaxBound<Vector3>(&list[0], list.size(), min, max);
		Log("min %f %f %f max %f %f %f\n", min.x, min.y, min.z, max.x, max.y, max.z);
		*/
	}
}

void ModelResource::SetAttribute(size_t index, unsigned long value)
{
	debug_assert(index < (index_count/3), "Attribute Index is out of range.");
	m_pMaterialList[index] = value;
}

void ModelResource::SetVertex(size_t index, Vertex& v)
{
	debug_assert(index < vertex_count, "Vertex Index is greater than the vertex buffer size.");
	switch(m_kVertexType)
	{
	case VT_BlendVertex:
		{
			BlendVertex* bv = (BlendVertex*)m_pVertexList.m_vertexList;
			bv[index] = v;
		}
		break;
	default:
		{
			PerPixelVertex* vt = (PerPixelVertex*)m_pVertexList.m_vertexList;
			vt[index] = v;
		}
		break;
	}
}

void ModelResource::SetIndex(size_t index, WORD word)
{
	debug_assert(index < index_count, "Index is greater than the index buffer size.");
	m_pPolyList[index] = word;
}


//(Intended for Editor) All intersecting triangles are copied into new resource models. The ModelContainer
//will only be populated if there were vertices on both sides...
void ModelResource::Slice(const WPlane& plane, ModelContainer* container, size_t minNumberTriangles)
{
	if (index_count/3 < minNumberTriangles)
	{
		return;
	}

	//subdivision is intended only for PerPixelVertices
	godzassert(m_kVertexType == VT_PerPixel);

	//buffers for all the triangles that were contained (aka 'main' lists)
	vector<Vertex> vertexBuffer; //vertex list
	vector<ulong> attributeBuffer;
	vector<u16> indexBuffer;

	//buffer for all the triangles that did not fit (aka 'outside' lists)
	vector<Vertex> outsideVertexBuffer;
	vector<ulong> outsideAttributeBuffer;
	vector<u16> outsideIndexBuffer;

	int newVertexIndex=0;
	int newOutsideVertexIndex=0;

	int numVertsFit = 0;
	int numOutsideVerts = 0;
	int count = 0;
	size_t containedVerts[3];
	size_t outsideVerts[3];

	Vertex tri[3]; //triangle
	size_t triangleIndicies[3];
	Halfspace halfspace[3]; //debugging

	for (size_t i=0;i<index_count;i++)
	{
		//builds a triangle
		unsigned short vertexIndex=m_pPolyList[i]; //vertex_index
		godzassert(vertexIndex<=vertex_count);

		Vertex* vt = static_cast<Vertex*>(GetVertex(vertexIndex));
		tri[count] = *vt;
		triangleIndicies[count] = vertexIndex;
		halfspace[count] = plane.ClassifyPoint(vt->pos);
		
		//If the vertex is on-the-plane or BEHIND, then we consider it as contained...
		if (halfspace[count] < Halfspace_FRONT)
		{
			containedVerts[numVertsFit] = vertexIndex;
			numVertsFit++;
		}
		else
		{
			//vertices detected on outside of the plane (FRONT) will be treated as 'outsider'
			outsideVerts[numOutsideVerts] = vertexIndex;
			numOutsideVerts++;
		}

		count++;
		if (count == 3)
		{
			//Log("Halfspaces %d %d %d\n", halfspace[0], halfspace[1], halfspace[2]);

			//There is one material index (attribute) per face
			//size_t index = i/3;

			//create the triangle for inside of the plane
			bool isContained = ComputeTriangles(plane, numVertsFit, vertexBuffer, indexBuffer, containedVerts, triangleIndicies, tri);

			//create the triangle for outside of the plane
			if (!isContained)
			{
				ComputeTriangles(plane, numOutsideVerts, outsideVertexBuffer, outsideIndexBuffer, outsideVerts, triangleIndicies, tri);
			}

			//reset
			numOutsideVerts = 0;
			numVertsFit = 0;
			count = 0;
		}
	}

	//We are assuming all the vertices were BEHIND or IN_FRONT the plane. No point in wasting CPU tossing
	//away this model then....
	if (indexBuffer.size()/3 < minNumberTriangles || outsideIndexBuffer.size()/3 < minNumberTriangles)
	{
		return;
	}

	//now create the two brand new modelresources and push them into the container...
	std::vector<ulong> fakeAttrbVuffer;
	ResourceManager* RM = ResourceManager::GetInstance();

	if (vertexBuffer.size() > 0)
	{
		MeshResource resource1 = RM->CreateModelResource(vertexBuffer, indexBuffer, fakeAttrbVuffer, m_hashCode);
		resource1->SetPackage(m_package);
		container->add(resource1);
	}

	if (outsideVertexBuffer.size() > 0)
	{
		MeshResource resource2 = RM->CreateModelResource(outsideVertexBuffer, outsideIndexBuffer, fakeAttrbVuffer, m_hashCode);	
		resource2->SetPackage(m_package);
		container->add(resource2);
	}
}

//returns true if the entire triangle is stored....
bool ModelResource::ComputeTriangles(const WPlane& plane, int numVertsFit, std::vector<Vertex>& vertexBuffer, std::vector<u16>& indexBuffer, size_t containedVerts[3], size_t triangleIndicies[3], Vertex tri[3])
{
	if (numVertsFit == 3)
	{
		//box contains all of this triangle so move this triangle into the new buffer.

		//for the indexbuffer, we need to search through all the verts and check for matches & reuse
		//only if we cant find a preexisting one should we make a new index for it...
		AddTriangle(tri[0], tri[1], tri[2], vertexBuffer, indexBuffer);

		return true;
	}
	else if (numVertsFit > 0)
	{
		//godzassert(numVertsFit > 0 && numVertsFit < 3);

		//box contains some of this triangle, we will need to split this polygon...

		//1. Create the triangle(s) that belong to the main list....
		{
			//iterate through containedVerts, make a triangle for each one....
			//we need to cut the line segment along the plane...
			Vertex* vt = static_cast<Vertex*>(GetVertex(containedVerts[0]));
			Vector3 start =  vt->pos;

			//if greater than 1, we just need to find the other spare vert
			if (numVertsFit == 2)
			{
				size_t otherIndex = containedVerts[1];

				size_t endSegmentIndex = 0;
				for (int k = 0; k < 3; k++)
				{
					if ( (triangleIndicies[k] != containedVerts[0]) && (triangleIndicies[k] != otherIndex))
					{
						endSegmentIndex = triangleIndicies[k];
						break;
					}
				}

				Vertex* endVt = static_cast<Vertex*>(GetVertex(endSegmentIndex));
				Vector3 end = endVt->pos;

				//q will be our new vertex
				float t;
				Vector3 q;

				TestPlane(plane, start, end, t, q);

				Vertex newVertexA;
				newVertexA.pos = q;

				//note: new vertex should be within Vertex A & B
				ComputeVertex(*vt, *endVt, t, newVertexA);

				//now compute the next new vertex
				Vertex* otherVt = static_cast<Vertex*>(GetVertex(otherIndex));

				TestPlane(plane, otherVt->pos, end, t, q);

				Vertex newVertexB;
				newVertexB.pos = q;

				ComputeVertex(*otherVt, *endVt, t, newVertexB);

				//now that we have our point we can form a Triangle....
				Vertex* newtri[3];

				int assigned = 0; //debug var

				//now build the triangle using the exact same winding order....
				for(int k=0;k<3;k++)
				{
					if (endSegmentIndex == triangleIndicies[k])
					{
						newtri[k] = &newVertexA;
						assigned++;
					}
					else if (otherIndex == triangleIndicies[k])
					{
						//not sure if winding order right on this....
						newtri[k] = &newVertexB;
						assigned++;
					}
					else
					{
						assigned++;
						newtri[k] = static_cast<Vertex*>(GetVertex(triangleIndicies[k]));
					}

					//Log("vertex fit A: 2 %f %f %f uv: %f %f nxyz %f %f %f\n", newtri[k]->pos.x, newtri[k]->pos.y, newtri[k]->pos.z, newtri[k]->u, newtri[k]->v, newtri[k]->normal.x, newtri[k]->normal.y, newtri[k]->normal.z);
				}

				AddTriangle(*newtri[0], *newtri[1], *newtri[2], vertexBuffer, indexBuffer);

				godzassert(assigned == 3);


				//build another triangle
				assigned = 0; //reset debug var

				//now build the other triangle
				for(int k=0;k<3;k++)
				{
					if (endSegmentIndex == triangleIndicies[k])
					{
						newtri[k] = &newVertexB;
						assigned++;
					}
					else
					{
						assigned++;
						newtri[k] = static_cast<Vertex*>(GetVertex(triangleIndicies[k]));
					}

					//Log("vertex fit B: 2 %f %f %f uv: %f %f nxyz %f %f %f\n", newtri[k]->pos.x, newtri[k]->pos.y, newtri[k]->pos.z, newtri[k]->u, newtri[k]->v, newtri[k]->normal.x, newtri[k]->normal.y, newtri[k]->normal.z);
				}

				AddTriangle(*newtri[0], *newtri[1], *newtri[2], vertexBuffer, indexBuffer);

				return false;
			}//if numFoundVerts=2
			else if (numVertsFit == 1)
			{
				//godzassert(j == 0); //we only have 1 vert so should only loop once....
				int new_vert_count = 0;
				size_t newIndicies[2];

				//gotta make two new line segments to form a triangle
				for (int k = 0; k < 3; k++)
				{
					//containedVerts[0] is the only index in this case.
					if ( (triangleIndicies[k] != containedVerts[0]))
					{
						newIndicies[new_vert_count] = triangleIndicies[k];
						new_vert_count++;

						if (new_vert_count == 2)
							break;
					}
				} //loop k

				godzassert(new_vert_count == 2);

				//make line segment 1
				Vertex* endVt = static_cast<Vertex*>(GetVertex(newIndicies[0]));
				Vector3 end = endVt->pos;

				//q will be our new vertex
				float t;
				Vector3 q;

				TestPlane(plane, start, end, t, q);

				Vertex newVertex1;
				newVertex1.pos = q;

				//note: new vertex should be within Vertex A & B
				ComputeVertex(*vt, *endVt, t, newVertex1);


				//make line segment 2
				endVt = static_cast<Vertex*>(GetVertex(newIndicies[1]));
				end = endVt->pos;

				TestPlane(plane, start, end, t, q);

				Vertex newVertex2;
				newVertex2.pos = q;

				//note: new vertex should be within Vertex A & B
				ComputeVertex(*vt, *endVt, t, newVertex2);

				//now we can make our triangle
				Vertex* newtri[3];

				//now build the triangle using the exact same winding order....
				for(int k=0;k<3;k++)
				{
					if (newIndicies[0] == triangleIndicies[k])
					{
						newtri[k] = &newVertex1;
					}
					else if (newIndicies[1] == triangleIndicies[k])
					{
						newtri[k] = &newVertex2;
					}
					else
					{
						newtri[k] = static_cast<Vertex*>(GetVertex(triangleIndicies[k]));
					}

					//Log("vertex fit: 1 %f %f %f uv: %f %f nxyz %f %f %f\n", newtri[k]->pos.x, newtri[k]->pos.y, newtri[k]->pos.z, newtri[k]->u, newtri[k]->v, newtri[k]->normal.x, newtri[k]->normal.y, newtri[k]->normal.z);					
				}
				AddTriangle(*newtri[0], *newtri[1], *newtri[2], vertexBuffer, indexBuffer);

				return false;
			} //if numvertsfit == 1
		}
	}

	return false;
}

void ModelResource::TestPlane(const WPlane& plane, const Vector3& start, const Vector3& end, float& t, Vector3& q)
{
	if (!plane.IntersectsLineSegment(start, end, t, q))
	{
		//if we missed, try flipping the ray
		if (plane.IntersectsLineSegment(end, start, t, q))
		{
			//Need to reverse t when we flip the ray in this manner
			t = 1 - t;
		}
		else
		{
			godzassert(plane.IntersectsLineSegment(start, end, t, q));
		}
	}
}

void ModelResource::AddTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, std::vector<Vertex>& vertexBuffer, std::vector<u16>& indexBuffer)
{
	WPlane p(v0, v1, v2);
	if (p.N.GetLength() < EPSILON)
	{
		//invalid plane....
		return;
	}

	const Vertex* v[3];
	v[0] = &v0;
	v[1] = &v1;
	v[2] = &v2;

	for(int k=0;k<3; k++)
	{
		size_t foundVertIndex = 0;
		if (GetVertexIndex(vertexBuffer, *v[k], foundVertIndex))
		{
			//vertex buffer already had a matching vertex!
			indexBuffer.push_back(foundVertIndex);
		}
		else //add the new vertex
		{
			vertexBuffer.push_back(*v[k]);
			indexBuffer.push_back(vertexBuffer.size() - 1);
		}
	}
}

bool ModelResource::GetVertexIndex(std::vector<Vertex>& list, const Vertex& vertex, size_t& index)
{
	size_t len = list.size();
	for(size_t i=0; i < len; i++)
	{
		if (list[i] == vertex)
		{
			index = i;
			return true;
		}
	}

	return false;
}

void ModelResource::ComputeVertex(const Vertex& a, const Vertex& b, float t, Vertex& dest)
{
	dest.normal = (1-t)*a.normal + t*b.normal;
	dest.u = (1-t)*a.u + t*b.u;
	dest.v = (1-t)*a.v + t*b.v;
	//dest.binormal = (1-t)*a.binormal + t*b.binormal;
	//dest.tangent = (1-t)*a.tangent + t*b.tangent;
}

bool ModelResource::Optimize()
{
	return false;
}

ModelResource& ModelResource::operator=(const ModelResource& V)
{
	//don't forget about the ResourceObject object....
	ResourceObject* res = static_cast<ResourceObject*>(this);
	*res = V;


	m_kVertexType = V.m_kVertexType;
	vertex_count = V.vertex_count;
	index_count = V.index_count;
	m_pCollTree = V.m_pCollTree;
	m_visible = V.m_visible;

	//create my own vertex list / index list / materials here
	//m_pVertexList = CreateVertices(vertex_count);
	CreateIndexBuffer(index_count);
	

	//copy data over.....
	//udword uPacketSize = V.GetVertexStructSize();
	//memcpy(m_pVertexList, V.m_pVertexList, uPacketSize * vertex_count);
	m_pVertexList = V.m_pVertexList;
	memcpy(m_pPolyList, V.m_pPolyList, sizeof(u16) * index_count);

	if (V.m_pMaterialList != NULL)
	{
		CreateAttributeBuffer(index_count/3);
		memcpy(m_pMaterialList, V.m_pMaterialList, sizeof(ulong) * (index_count/3));
	}
	
	return *this;
}