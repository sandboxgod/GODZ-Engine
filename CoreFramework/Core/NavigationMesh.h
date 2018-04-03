/* ===========================================================
	NavigationMesh


	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__NavigationMesh_H__)
#define __NavigationMesh_H__

#include "CoreFramework.h"
#include "GodzAtomics.h"
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/WPlane.h>
#include <CoreFramework/Math/WSphere.h>
#include "GDZArchive.h"
#include <vector>

namespace GODZ
{
	class MeshInstance;
	class ModelResource;
	class RenderDeviceObject;

	/*
	* Link between two navmeshpolygons. Designed so that it can be used to link to other navigation meshes
	* that may be possibly loaded / unloaded via it's layer
	*/
	struct GODZ_API NavMeshPolygonLink
	{
	public:
		NavMeshPolygonLink();

		//layer id (future use) -- 0 = parent navmesh, anything else indicates a link to an external navmesh 
		//at the LayerID
		HashCodeID mLayerID;	
		size_t index;			//index of the sibling within it's navmesh

		//number of vertices shared between the polygons. AI might want to only consider polys linked 
		//by edges (sharedVertices == 2) or perhaps only do extra testing if shared verts == 1 only
		int sharedVertices;

		bool isStrongLink() const;

		friend GDZArchive& operator<<(GDZArchive& ar, NavMeshPolygonLink& p) 
		{
			ar << p.mLayerID << p.index << p.sharedVertices;
			return ar;
		}
	};

	/*
	* Polygon shape (like a triangle, etc)
	*/
	struct GODZ_API NavMeshPolygon
	{
		Vector3 vertex[3];							//vertex
		WPlane mPlane;								//plane
		Vector3 mCenter;							//center of this polygon
		std::vector<NavMeshPolygonLink> mSiblings;

		//Will test the polygon to see if they share verts
		int WeldSharedVerts(const NavMeshPolygon& p);
		bool Find(size_t index);

		friend GDZArchive& operator<<(GDZArchive& ar, NavMeshPolygon& p)
		{
			ar << p.vertex[0] << p.vertex[1] << p.vertex[2];
			ar << p.mPlane;
			ar << p.mCenter;

			size_t len = p.mSiblings.size();
			ar << len;

			if (!ar.IsSaving())
			{
				p.mSiblings.reserve(len);
			}

			
			for(size_t i=0;i<len;i++)
			{
				if (!ar.IsSaving())
				{
					NavMeshPolygonLink link;
					ar << link;
					p.mSiblings.push_back(link);
				}
				else
					ar << p.mSiblings[i];
			}

			return ar;
		}
	};

	//NOTE: navmesh positioned at identity matrix; vertices already transformed into world-space
	class GODZ_API NavigationMesh
	{
	public:

		//angle-- deviation from UP vector. 0 degrees = UP Vector
		//trans- transformation matrix of the entity that contains this mesh. Basically where mesh should be positioned in world-space
		void AddMesh(MeshInstance* model, const WMatrix& trans, float angle = 0.10f);

		//Should be called after all meshes have been added. Let's other threads know this mesh is safe to access
		void Finalize();

		size_t GetNumPolygons() const;
		const NavMeshPolygon& GetPolygon(size_t index) const;

		//Returns true if the ray hits a polygon; polyIndex will store the polygon that was hit if true
		bool IsPolygonHit(const WRay& ray, size_t& poly_index) const;

		bool IsPolygonHit(const WSphere& s, const Vector3& velocity, size_t& poly_index) const;

		//submit a debug primitive to the renderer
		void SubmitDebugPrimitive();

		void Serialize(GDZArchive& ar);

		void Load(const char* name);
		void Save(const char* name);

		void Clear();

	private:
		void AddModel(ModelResource* model, std::vector<NavMeshPolygon>& polyArray, float angle = 0.10f);
		
		struct GODZ_API NavMeshPolygonBuildInfo
		{
			u16 mVertexIndex[3];				//vertex_index
		};

		std::vector<NavMeshPolygon> mPolys;

		//Interface to the Render Device Object which is used by the Render pipeline exclusively
		Future<GenericReference<RenderDeviceObject> >	m_renderDeviceObject;
		
		GodzAtomic m_closed;					//set when this navmesh is considered done and read-only
	};

}

#endif //__NavigationMesh_H__