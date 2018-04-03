
#include "Material.h"
#include "Mesh.h"
#include "ModelResource.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "ShaderResource.h"
#include "IReader.h"
#include "SerializedContentHandler.h"
#include "WEntity.h"
#include <CoreFramework/Animation/AnimationInstance.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>


using namespace GODZ;

///////////////////////////////////////////////////////////////

ImplementGeneric(Mesh)

REGISTER_VECTOR(Mesh, OffsetLocation)
REGISTER_BOX(Mesh, Bounds)
REGISTER_OBJECT(Mesh, DefaultInstance, MeshInstance)

Mesh::Mesh()
: m_pAnimInstance(NULL)
, m_offsetLoc(Vector3::Zero())
, box(WBox::Zero())
{
}

Mesh::~Mesh()
{
	if(m_pAnimInstance != NULL)
	{
		delete m_pAnimInstance;
		m_pAnimInstance=NULL;
	}

	m_pMeshInstance = NULL;
}

GenericClass* Mesh::GetMeshType()
{
	return MeshInstance::GetClass();
}

void Mesh::SetDefaultInstance(atomic_ptr<MeshInstance>& ptr)
{
	//Once the default instance is set, it should not change
	godzassert(m_pMeshInstance == NULL);
	m_pMeshInstance = ptr;
}

void Mesh::CreateRenderDeviceObjects()
{
	//this ever called?
	if (m_pMeshInstance!= NULL)
	{
		m_pMeshInstance->CreateRenderDeviceObjects();
	}
}

AnimationInstance* Mesh::GetAnimationInstance()
{
	return m_pAnimInstance;
}

void Mesh::CreateAnimationInstance()
{
	if (m_pAnimInstance == NULL)
	{
		m_pAnimInstance=new AnimationInstance();
	}	
}


void Mesh::ComputeBounds()
{
	if (m_pMeshInstance != NULL)
	{
		MeshInstance::MeshMap& map = m_pMeshInstance->GetMeshMap();
		MeshInstance::MeshMap::iterator iter;

		for(iter = map.begin(); iter != map.end(); iter++)
		{
			ModelContainer* mc = iter->second;

			std::vector<Vector3> points;
			size_t numModels = mc->size();

			for(size_t i=0;i<numModels;i++)
			{
				ModelResource* resource = mc->get(i);
				const WBox& myBox = resource->GetBox();
				points.push_back(myBox.GetMin());
				points.push_back(myBox.GetMax());
			}

			if (points.size() > 0)
			{
				Vector3 tmin, tmax;
				SetMinMaxBound<Vector3>(&points[0], points.size(), tmin, tmax);
				box.SetMinMax(tmin, tmax);
			}
		}
	}
}

const Vector3& Mesh::GetOffsetLocation() const
{
	return m_offsetLoc;
}

const WBox& Mesh::GetBounds() const
{
	return box;
}

void Mesh::OnPropertiesUpdated()
{
}


void Mesh::SetOffsetLocation(const Vector3& newOffset)
{
	m_offsetLoc = newOffset;
}

void Mesh::SetBounds(const WBox& _box)
{
	box = _box;
}

//note: called from serialize thread
void Mesh::Serialize(GDZArchive& ar)
{
	GenericObject::Serialize(ar);

	ArchiveVersion version(1,1);
	ar << version;

	bool bHasAnimationData = m_pAnimInstance != NULL;
	ar << bHasAnimationData;

	if (bHasAnimationData && m_pAnimInstance == NULL)
	{
		CreateAnimationInstance();
	}

	if (m_pAnimInstance != NULL)
	{
		m_pAnimInstance->Serialize(ar);
	}
}

