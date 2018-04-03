
#include "NavigationMesh.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include "ModelResource.h"
#include "MeshInstance.h"
#include "RenderDeviceEvent.h"
#include "SceneData.h"
#include "TriangleList.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "ResourceManager.h"
#include <CoreFramework/Collision/RayCollider.h>

namespace GODZ
{

NavMeshPolygonLink::NavMeshPolygonLink()
: mLayerID(0)
{
}

bool NavMeshPolygonLink::isStrongLink() const 
{ 
	return sharedVertices == 2; 
}

//////////////////////////////////////////////////////////////////////////////////////////////

//weld vertices so that we can do ray traces....
int NavMeshPolygon::WeldSharedVerts(const NavMeshPolygon& p)
{
	int numSharedVertices = 0;

	for(int i=0; i < 3; i++)
	{
		for(int j=0;j<3;j++)
		{
			float d = (p.vertex[i] - vertex[j]).GetLength();

			//Max distance between vertices we allow in order for them to be considered to be 'linked'
			//What vexes me though is that I've seen some vertices less than 1.0 (around 0.95f). not sure if those
			//should be considered? Most distances are behind the decimal though like 0.00017263349 which should be
			//counted for sure. This number may require a lot of tweaking
			//TODO: would love to flag polys we suspect are supposed to be linked but their verts not welded proper...
			static const float MAX_DIST_BETWEEN_VERTS = 0.1f;

			if (d < MAX_DIST_BETWEEN_VERTS)
			{
				numSharedVertices++;

				vertex[j] = p.vertex[i];
			}
		}
	}

	//this would be bad if we have two perfectly overlapping polys
	//godzassert(numSharedVertices < 3);

	return numSharedVertices; //they share an edge or a vertex
}

bool NavMeshPolygon::Find(size_t index)
{
	size_t num = mSiblings.size();
	for(size_t i=0;i<num;i++)
	{
		if (index == mSiblings[i].index)
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void NavigationMesh::Finalize()
{
	m_closed.Set();
}

void NavigationMesh::Clear()
{
	mPolys.clear();
}

void NavigationMesh::SubmitDebugPrimitive()
{
	//for debugging & rendering NavigationMeshes
	TriangleList list;

	size_t num = mPolys.size();
	for(size_t i=0; i<num;i++)
	{
		TriPolygon p;
		p.v[0] = mPolys[i].vertex[0];
		p.v[1] = mPolys[i].vertex[1];
		p.v[2] = mPolys[i].vertex[2];
		p.n = mPolys[i].mPlane.N;
		list.AddPoly(p);
	}

	Color4f c(0.2f, 0.2f, 1.0f, 1.0f);

	//pass in an identity matrix because the vertices are already in world-space
	WMatrix m(1);
	Vector3 t(0.0f,20.0f,0.0f);
	m.SetTranslation(t);

	RenderDeviceEvent* event = CreateTriMesh(m_renderDeviceObject, list, c, m);
	SceneData::AddRenderDeviceEvent(event);
}

void NavigationMesh::AddMesh(MeshInstance* mesh, const WMatrix& trans, float angle)
{
	MeshInstance::MeshMap& map = mesh->GetMeshMap();
	MeshInstance::MeshMap::iterator iter;

	for(iter = map.begin(); iter != map.end(); iter++)
	{
		ModelContainer* mc = iter->second;
		size_t num = mc->size();
		for(size_t i=0;i<num;i++)
		{
			std::vector<NavMeshPolygon> polygons;
			AddModel( mc->get(i), polygons, angle );

			size_t num = polygons.size();
			for(size_t i=0; i<num;i++)
			{
				//transform all the polygons into world-space from model-space
				trans.Mul(polygons[i].vertex[0]);
				trans.Mul(polygons[i].vertex[1]);
				trans.Mul(polygons[i].vertex[2]);
				mPolys.push_back(polygons[i]);
			}
		}
	}

	//Now connect the related polygons (takes forever)....
	size_t num = mPolys.size();
	for(size_t i=0;i<num;i++)
	{
		for(size_t j=0;j<num;j++)
		{
			if (i != j)
			{
				if (!mPolys[i].Find(j) )
				{
					int count = mPolys[i].WeldSharedVerts(mPolys[j]);
					if (count > 0)
					{
						NavMeshPolygonLink link;
						link.index = j;
						link.sharedVertices = count;
						mPolys[i].mSiblings.push_back(link);

						//tell the other guy bout us
						link.index = i;
						mPolys[j].mSiblings.push_back(link);
					}
				}
			}
		}
	}
}

void NavigationMesh::AddModel(ModelResource* model, std::vector<NavMeshPolygon>& polyArray, float angle)
{
	size_t num = model->GetNumIndicies();
	godzassert(num % 3 == 0); //index buffer should be a multiple of 3
	NavMeshPolygon poly;
	NavMeshPolygonBuildInfo buildInfo;

	//Build info vector has a 1-to-1 relationship with the NavMeshPolygon array
	std::vector<NavMeshPolygonBuildInfo> polys;	

	for(size_t i=0;i<num;i+=3)
	{
		int idx0 = model->GetIndex(i);
		int idx1 = model->GetIndex(i+1);
		int idx2 = model->GetIndex(i+2);

		poly.vertex[0] = model->GetVertex(idx0)->pos;
		poly.vertex[1] = model->GetVertex(idx1)->pos;
		poly.vertex[2] = model->GetVertex(idx2)->pos;

		buildInfo.mVertexIndex[0] = idx0;
		buildInfo.mVertexIndex[1] = idx1;
		buildInfo.mVertexIndex[2] = idx2;


		WPlane p(poly.vertex[0],poly.vertex[1],poly.vertex[2]);
		poly.mPlane = p;
		float dir = ComputeAngle(p.N, UP_VECTOR);
		if (dir <= angle)
		{
			poly.mCenter.x = (poly.vertex[0].x + poly.vertex[1].x + poly.vertex[2].x) / 3.0f;
			poly.mCenter.y = (poly.vertex[0].y + poly.vertex[1].y + poly.vertex[2].y) / 3.0f;
			poly.mCenter.z = (poly.vertex[0].z + poly.vertex[1].z + poly.vertex[2].z) / 3.0f;

			//add this polygon
			polyArray.push_back(poly);
			polys.push_back(buildInfo);
		}
	} // loop: build polygons
}

//possibly called by serialize thread...
void NavigationMesh::Serialize(GDZArchive& ar)
{
	if (ar.IsSaving())
	{
		size_t len = mPolys.size();
		ar << len;
		for(size_t i=0;i<len;i++)
		{
			ar << mPolys[i];
		}
	}
	else
	{
		size_t numPolygons;
		ar << numPolygons;
		mPolys.reserve(numPolygons);

		for(size_t i=0;i<numPolygons;i++)
		{
			NavMeshPolygon n;
			ar << n;
			mPolys.push_back(n);
		}

		//last step! Set that we are done building the mesh
		Finalize();
	}
} 

size_t NavigationMesh::GetNumPolygons() const
{
	godzassert(m_closed.IsSet());
	return mPolys.size();
}

const NavMeshPolygon& NavigationMesh::GetPolygon(size_t index) const
{
	godzassert(m_closed.IsSet());
	return mPolys[index];
}

bool NavigationMesh::IsPolygonHit(const WRay& ray, size_t& polyIndex) const
{
	//TODO: organize navmesh into a tree then use that to optimize the search....
	//NOTE: Right now we check all the polys just incase there's *multiple levels*. Such as
	//multiple floors in a building or something....

	float bestDist = 0;
	size_t bestpoly = 0;
	size_t count = 0 ;

	size_t len = mPolys.size();
	for(polyIndex=0; polyIndex<len;polyIndex++)
	{
		CollisionFace face;
		if (RayTriOverlap(ray, mPolys[polyIndex].vertex[0], mPolys[polyIndex].vertex[1], mPolys[polyIndex].vertex[2], face))
		{
			if (count == 0 || face.mDistance < bestDist)
			{
				bestpoly = polyIndex;
				bestDist = face.mDistance;
				count++;
			}
		}
	}

	polyIndex = bestpoly;

	return count > 0;
}

bool NavigationMesh::IsPolygonHit(const WSphere& s, const Vector3& velocity, size_t& polyIndex) const
{
	float bestDist = 0;
	size_t bestpoly = 0;
	size_t count = 0 ;

	Vector3 hit;
	float d;

	size_t len = mPolys.size();
	for(polyIndex=0; polyIndex<len;polyIndex++)
	{
		if (s.IntersectTriangle(mPolys[polyIndex].vertex[0], mPolys[polyIndex].vertex[1], mPolys[polyIndex].vertex[2], mPolys[polyIndex].mPlane.N, velocity, hit, d) )
		{
			if (count == 0 || d < bestDist)
			{
				bestpoly = polyIndex;
				bestDist = d;
				count++;
			}
		}
	}

	polyIndex = bestpoly;

	return count > 0;
}

void NavigationMesh::Load(const char* name)
{
	rstring dir = GetBaseDirectory();
	dir += "\\Data\\Navigation\\";
	dir += name;
	dir += ".nav";

	if (FileExists(dir))
	{
		ResourceManager* resMngr = ResourceManager::GetInstance();

		FileReader reader(dir, &resMngr->GetResourceFactory());
		GDZArchive archive(&reader);
		Serialize(archive);
	}
}

void NavigationMesh::Save(const char* name)
{
	rstring dir = GetBaseDirectory();
	dir += "\\Data\\Navigation\\";
	dir += name;
	dir += ".nav";

	FileWriter writer(dir);
	GDZArchive archive(&writer);
	Serialize(archive);
}


}