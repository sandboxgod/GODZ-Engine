

#include "Mesh.h"
#include "ModelResource.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "WEntity.h"
#include <CoreFramework/Animation/AnimationInstance.h>
#include <CoreFramework/Collision/AABBTree.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include "GenericObjData.h"
#include <CoreFramework/Reflection/ClassProperty.h>


namespace GODZ
{
ImplementGeneric(ModelContainer)
REGISTER_RESOURCE_ARRAY(ModelContainer, Meshes, GenericReference<ModelResource> )

ImplementGeneric(MeshInstance)
REGISTER_ARRAY(MeshInstance, Materials, atomic_ptr<Material> )
REGISTER_INT(MeshInstance, CollisionShape)
REGISTER_MAP(MeshInstance, MeshMap2)

////////////////////////////////////////////////////////////////////

MeshResource ModelContainer::get(size_t index)
{
	godzassert(index < mList.size());
	return mList[index];
}

bool ModelContainer::drop(ModelResource* resource)
{
	std::vector<MeshResource>::iterator resourceIterator;
	for (resourceIterator=mList.begin(); resourceIterator!=mList.end();resourceIterator++)
	{
		if (resource == *resourceIterator)
		{
			mList.erase(resourceIterator);
			return true;
		}
	}

	return false;
}

void ModelContainer::add(ModelContainer& source)
{
	size_t num = source.size();
	for(size_t i=0; i<num; i++)
	{
		mList.push_back( source.get(i) );
	}
}

///////////////////////////////////////////////////////////////////

MeshInstance::MeshInstance()
: mesh(NULL)
, m_kCollType(CT_Box)
{
}

MeshInstance::~MeshInstance()
{
	mMeshMap.clear();
	mesh = NULL;
}

Skeleton* MeshInstance::GetSkeleton()
{
	Skeleton* s = new Skeleton();

	//Add child nodes to the base skeleton...
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		s->AddNode(m_nodes[i]);
	}

	return s;
}

void MeshInstance::Transform(float deltaTime, Skeleton* skeleton, AnimController* controller)
{
	//We let the skeleton nodes stay in local space....
}

void MeshInstance::AddNode(const SkeletonNode& node)
{
	m_nodes.push_back(node);
}

int MeshInstance::GetCollisionShape()
{
	return m_kCollType;
}

void MeshInstance::SetCollisionShape(int shape)
{
	m_kCollType = (ECollisionType)shape;

	switch(m_kCollType)
	{
	default:
		{
			godzassert(0); //unknown shape
		}
		break;
	case CT_Tree:
		{
			MeshInstance::MeshMap2& map = GetMeshMap2();
			MeshInstance::MeshMap2::iterator iter;

			for(iter = map.begin(); iter != map.end(); iter++)
			{
				ModelContainer* mc = iter->second;
				size_t num = mc->size();
				for(size_t i=0;i<num;i++)
				{
					ModelResource* rez = mc->get(i);
					rez->ComputeCollisionTree();
				}
			}
		}
		break;
	case CT_Box:
		{
			MeshInstance::MeshMap2& map = GetMeshMap2();
			MeshInstance::MeshMap2::iterator iter;

			for(iter = map.begin(); iter != map.end(); iter++)
			{
				ModelContainer* mc = iter->second;
				size_t num = mc->size();
				for(size_t i=0;i<num;i++)
				{
					ModelResource* rez = mc->get(i);

					//just destroy the coll tree if it exists. Models automatically switch
					//back to Box shapes then
					rez->DestroyCollisionTree();
				}
			}
		}
		break;
	}


}

void MeshInstance::AddMaterial(const atomic_ptr<Material>& material_ptr)
{
	materials.push_back(material_ptr);
}

size_t MeshInstance::GetNumMaterials() 
{
	return materials.size();
}

GodzVector<atomic_ptr<Material> >& MeshInstance::GetMaterials() 
{
	return materials;
}

atomic_ptr<Material> MeshInstance::GetMaterial(size_t index) 
{
	return materials[index];
}

Material* MeshInstance::GetMaterialByName(HString text)
{
	size_t num = materials.size();
	for(size_t i=0;i<num;i++)
	{
		Material* m = materials[i];
		if (m != NULL && m->GetName() == text)
		{
			return m;
		}
	}

	return NULL;
}

void MeshInstance::AddModelResource(Material* material, ModelResource* resource)
{
	UInt32 index;
	FindMaterialIndex(material, index);

	MeshMap2::iterator iter = mMeshMap.find(index);
	if (iter == mMeshMap.end())
	{
		ModelContainer* mc = new ModelContainer();
		mMeshMap[index] = mc;
		mc->add(resource);
		return;
	}

	iter->second->add(resource);
}

ModelContainer* MeshInstance::GetModelResources(Material* material)
{
	UInt32 index;
	FindMaterialIndex(material, index);
	MeshMap2::iterator iter = mMeshMap.find(index);
	if (iter != mMeshMap.end())
	{
		return mMeshMap[index];
	}

	return NULL;
}

ECollisionType MeshInstance::GetCollisionType()
{
	return this->m_kCollType;
}

void MeshInstance::Initialize(Mesh* pMesh)
{
	mesh=pMesh;
}

void MeshInstance::OnPropertiesUpdated()
{
	//update collision incase it was changed....
	SetCollisionShape(m_kCollType);
}

Mesh* MeshInstance::GetMesh()
{
	return mesh;
}

void MeshInstance::SetMesh(Mesh* pMesh)
{
	mesh = pMesh;
}

void MeshInstance::SetShader(ShaderResource* pShader)
{
	size_t numMats = materials.size();
	for(size_t i=0;i<numMats;i++)
	{
		Material* pMat = this->GetMaterial(i);
		pMat->SetShader(pShader);
	}
}

//We expect to be passed an array of planes in our model space that neatly divides this mesh into multiple pieces
void MeshInstance::Slice(std::vector<WPlane>& planes, size_t minNumberTriangles)
{
	size_t numplanes = planes.size();
	for(size_t i = 0; i < numplanes; i++)
	{
		for(MeshMap2::iterator iter = mMeshMap.begin(); iter != mMeshMap.end(); iter++)
		{
			//just keep going
			SliceModels(planes[i], iter->second, minNumberTriangles);
		} //loop meshmap iter
	}
}

//Note: current issue is when we come back along z plane and chop same resources again....
bool MeshInstance::SliceModels(const WPlane& plane, ModelContainer* mc, size_t minNumberTriangles)
{
	std::vector<MeshResource> newResources;
	std::vector<MeshResource> removedResources;

	size_t nummodels = mc->size();
	for(size_t i = 0; i < nummodels; i++)
	{
		ModelResource* resource = mc->get(i);

		//check to see if the model will be split along the plane before we bother checking
		//the triangles...
		const WBox& s = resource->GetBox();

		if (plane.ClassifyBox(s.GetMin(), s.GetMax()) == Halfspace_ON_PLANE)
		{
			ModelContainer temp;

			resource->Slice(plane, &temp, minNumberTriangles);

			//If the model didn't split in half, then just keep it...
			if (temp.size() > 1)
			{
				removedResources.push_back(resource);

				for(size_t j = 0; j < temp.size(); j++)
				{
					ModelResource* r = temp.get(j);
					newResources.push_back(temp.get(j));
				}
			}
		}
	}

	nummodels = removedResources.size();
	for(size_t j=0; j<nummodels;j++)
	{
		mc->drop(removedResources[j]);
	}

	nummodels = newResources.size();
	for(size_t j=0; j<nummodels;j++)
	{
		mc->add(newResources[j]);
	}

	return true;
}

void MeshInstance::CreateRenderDeviceObjects()
{
	//serialize the model resources
	for(MeshMap2::iterator iter = mMeshMap.begin(); iter != mMeshMap.end(); iter++)
	{
		size_t num = iter->second->size();
		for(size_t i = 0; i < num; i++)
		{
			ModelResource* res = iter->second->get(i);
			res->CreateRenderDeviceObject();
		}
	}
}

void MeshInstance::Serialize(GDZArchive& ar)
{
	//Added serialization support after version 1007
	if (ar.GetFileVersion() > 1007)
	{
		ArchiveVersion version(1,2);
		ar << version;

		//save nodes...
		size_t numNodes = m_nodes.size();
		ar << numNodes;

		for (size_t i = 0; i < numNodes; i++)
		{
			if(ar.IsSaving())
			{
				ar << m_nodes[i];
			}
			else
			{
				SkeletonNode node;
				m_nodes.push_back(node);

				//This way we only allocate primitives, etc once
				ar << m_nodes[m_nodes.size() - 1];
			}
		}
	}
}

MeshInstance::MeshMap MeshInstance::GetMeshMap()
{
	MeshMap map;
	MeshMap2::iterator iter;
	for( iter = mMeshMap.begin(); iter != mMeshMap.end(); ++iter)
	{
		Material* m = materials[iter->first];
		map[m] = iter->second;
	}

	return map;
}

bool MeshInstance::FindMaterialIndex(Material* m, UInt32& index)
{
	for(UInt32 i = 0; i < materials.size();i++)
	{
		if(materials[i] == m)
		{
			index = i;
			return true;
		}
	}

	godzassert(0); // material not found
	return false;
}

} //namespace