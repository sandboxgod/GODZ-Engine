/* ===========================================================
	Mesh element consisting of vertex and index buffers.

	This class serves mainly as a metaclass for other 
	implementations to extend and provide direct rendering
	capabilities. ModelResources represent raw mesh data.

	Created Jan 19, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_MODELRESOURCE_H_)
#define _MODELRESOURCE_H_

#include "CoreFramework.h"
#include "VertexTypes.h"
#include "ResourceObject.h"
#include "GenericReferenceCounter.h"
#include "RenderDeviceObject.h"
#include <CoreFramework/Collision/CollisionResult.h>
#include <CoreFramework/Collision/CollisionSystems.h>
#include <CoreFramework/Math/WPlane.h>
#include <CoreFramework/Math/WBox.h>

namespace GODZ
{
	//Forward Declarations (compile time optimization)
	class AABBNode;
	class ICollisionCallback;
	class CollisionPrimitive;
	class MeshInstance;
	class ModelContainer;
	class ModelResource;
	class Shader;
	struct SceneInfo;
	class Mesh;

	class GODZ_API ModelResource
		: public ResourceObject
	{
	public:
		ModelResource(HashCodeID hash, EVertexType vertexType = VT_PerPixel);
		~ModelResource();

		//notification to batch shaders (that a shader has been appened,etc)
		void		BatchShaders();

		// Computes an AABB for this mesh resource. When this bound is updated,
		// the mesh storing this resource is notified to expand it's boundary.
		void		ComputeBounds();

		//Builds a collision tree model that can be used for complex (polygon level)
		//collision detection. 
		void		ComputeCollisionTree(int max_tree_depth=24);

		void		CreateMesh();

		//submits a request to instantiate a render device object for this object
		void		CreateRenderDeviceObject();

		//splits mesh up into sections for shaders
		void		CreateMeshChunks(BlendVertex* pVertexBuffer);

		//Creates a model defined by the vertex/attribute buffer within system memory
		ModelResource* CreateModelResource(std::vector<Vertex>& vertexBuffer, std::vector<UInt16>& indexBuffer, std::vector<ulong>& attributeBuffer);

		//Creates a model defined by the vertex/attribute buffer within system memory
		ModelResource*		CreateModelResourceInMemory(std::vector<Vertex>& vertexBuffer, std::vector<UInt16>& indexBuffer, std::vector<ulong>& attributeBuffer) ;

		//Creates a model defined by the vertex/attribute buffer within system memory
		ModelResource*		CreateModelResourceInMemory(std::vector<PerPixelVertex>& vertexBuffer, std::vector<UInt16>& indexBuffer) ;

		//returns the attribute buffer in use by this mesh
		ulong*				GetAttributeBuffer();
		
		//Sets a value in the attribute buffer
		void				SetAttribute(size_t index, unsigned long value);

		//Returns the axis aligned bounding box tree used by this model.
		AABBNode*			GetCollisionTree() const;
		void				DestroyCollisionTree();

		//Returns the desired vertex. In order to find out the type of vertex this is,
		//you must query the resource type of this model -- GetFlags(). Most models use
		//PerPixel vertices by default (VT_PerPixel)
		BaseVertex*			GetVertex(size_t index) const;

		//Sets a value in the vertex buffer. If the internal vertex structure is different,
		//this function will attempt to convert.
		void				SetVertex(size_t index, Vertex& v);

		//returns the format of the vertices contained within this buffer
		EVertexType			GetVertexType() { return m_kVertexType; }

		u16&				GetIndex(size_t index) const; // Returns vertex index for this face
		void				SetIndex(size_t index, u16 word); //Sets a value in the index buffer

		//Returns the number of vertices
		size_t				GetNumVertices();

		//Returns number of indicies
		size_t				GetNumIndicies();

		//returns the internal data as type Vertex if this operation is supported.
		Vertex*				GetVertexBuffer();
		u16*				GetIndexBuffer();

		//returns the size of the internal vertex structure in bytes
		udword				GetVertexStructSize() const;

		const WBox&			GetBox() { return m_box; }
		void				SetBounds(const WBox& box) { m_box = box; }

		//Returns true if a collision tree was computed for this resource.
		bool				HasLeafNodes();

		//serialize this object's content to the file stream
		void		Serialize(GDZArchive& ar);

		//mesh splitter utility function
		void		Slice(const WPlane& plane, ModelContainer* container, size_t minNumberTriangles);

		//optimizes this mesh
		bool		Optimize();

		void SetVisible(bool vis) { m_visible = vis; }
		bool IsVisible() { return  m_visible; }

		void		SetRecomputeNormals(bool flag) { mIsRecomputingNormals = flag; }

		ModelResource& operator=(const ModelResource& other);

	private:
		//Creates buffer in system memory (used by default mesh instances)
		void		CreateVertices(size_t number_of_verts);

		//Creates buffer in system memory (used by default mesh instances)
		void		CreateIndexBuffer(size_t number_of_polys);

		//Creates buffer in system memory (used by default mesh instances)
		void		CreateAttributeBuffer(size_t size_of_buffer);

		bool		GetVertexIndex(std::vector<Vertex>& list, const Vertex& vertex, size_t& index);
		void		ComputeVertex(const Vertex& a, const Vertex& b, float t, Vertex& dest);
		bool		ComputeTriangles(const WPlane& plane, int numVertsFit, std::vector<Vertex>& list, std::vector<u16>& indexBuffer, size_t containedVerts[3], size_t triangleIndicies[3], Vertex tri[3]);
		void		AddTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, std::vector<Vertex>& list, std::vector<u16>& indexBuffer);
		void		TestPlane(const WPlane& plane, const Vector3& start, const Vector3& end, float& t, Vector3& q);

	protected:

		EVertexType		m_kVertexType;
		size_t			vertex_count;			//# of vertices
		size_t			index_count;			//# of indicies
		GenericReference<AABBNode>		m_pCollTree;			//Axis Aligned collision tree
		bool m_visible;
		bool mIsRecomputingNormals;				// Set if normals should be recomputed
		WBox m_box;

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Cache Mesh Data so the Renderer can create this mesh later
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		u16 *			m_pPolyList;				//vertex indicies
		VertexArray		m_pVertexList;				//packed vertex structures
		ulong *			m_pMaterialList;			//material ID List (material buffer is size of index_count/3)
	};

	//Reference Counted pointer
	typedef GenericReference<ModelResource> MeshResource;
}

#endif