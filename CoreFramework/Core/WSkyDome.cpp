
#include "WSkyDome.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>
#include "PackageList.h"
#include "GenericObjData.h"
#include "Mesh.h"
#include "SkyDome.h"
#include "LevelNode.h"
#include "Material.h"

namespace GODZ
{

ImplementGenericFlags(WSkyDome, ClassFlag_Placeable)
//TODO: Allow Editor to pick SkyDome package, Mesh type (half dome, etc) + desired texture

WSkyDome::WSkyDome()
: mType(DomeType_Full)
{
	m_bVisible = true;
	SetCanCollideWithWorld(false);
}

void WSkyDome::UpdateGeometryInfo(GeometryInfo& info)
{
	info.m_type = GeometryBaseInfo::VisualType_ReferenceOnly;
}

void WSkyDome::SetDomeType(DomeType type)
{
	mType = type;
}

void WSkyDome::OnAddToWorld()
{
	WEntity::OnAddToWorld();

	if (mesh == NULL)
	{
		GenericPackage* package = GenericObjData::m_packageList.FindPackageByName("Skydomes");

		//If you get this assert, we didn't do a LoadPackage("Skydomes") earlier!
		//godzassert(package != NULL);

		if (package != NULL)
		{
			switch(mType)
			{
			case DomeType_Full:
				{
					const unsigned int sphere01_hash = 3861174648; //"Sphere01"
					atomic_ptr<GenericObject> objPtr( package->FindObjectRef(sphere01_hash) );
					atomic_ptr<Mesh> meshPtr( objPtr );
					SetMesh( meshPtr );
				}
				break;
			case DomeType_Half:
				{
					atomic_ptr<GenericObject> objPtr( package->FindObjectRef(3435056852) ); //HalfDome01
					atomic_ptr<Mesh> meshPtr( objPtr );

					Material* mat = meshPtr->GetDefaultInstance()->GetMaterial(0);
					if( mat != NULL )
					{
						HString sLuminosity("Luminosity");
						Material* lumin_mat = SafeCast<Material>( package->FindObject(sLuminosity) );
						mat->SetBitmap( lumin_mat->GetBitmap() );
					}

					SetMesh( meshPtr );
				}
				break;
			}
		}
	}

	SkyDomeInfo info;
	info.m_id = m_id;

	CreateUpdateSkyDomeEvent* sky = new CreateUpdateSkyDomeEvent(info);
	m_pLevel->AddObjectStateChangeEvent(sky);
}

void WSkyDome::OnRemoved()
{
	WEntity::OnRemoved();

	SkyDomeInfo info;
	info.m_id = m_id;

	RemoveSkyDomeEvent* sky = new RemoveSkyDomeEvent(info);
	m_pLevel->AddObjectStateChangeEvent(sky);
}

}