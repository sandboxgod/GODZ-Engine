
#include "VisualComponent.h"
#include "Godz.h"
#include "WEntity.h"
#include "SceneData.h"
#include <CoreFramework/Animation/AnimationController.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/ClassProperty.h>

using namespace GODZ;

ImplementGeneric(VisualComponent)

/// Exposed to Artists so they can type in Mesh we wish to display
REGISTER_OBJECT(VisualComponent, Mesh, Mesh)

VisualComponent::VisualComponent()
: m_pMeshInstance(NULL)
, m_pSkeleton(NULL)
, m_animController(NULL)
{
	//m_id = SceneData::CreateRuntimeID();
}

VisualComponent::VisualComponent(WEntity* owner)
: EntityComponent(owner)
, m_pMeshInstance(NULL)
, m_pSkeleton(NULL)
, m_animController(NULL)
{
	//m_id = SceneData::CreateRuntimeID();
}


atomic_ptr<Mesh> VisualComponent::GetMesh()
{
	return mesh;
}

AnimController* VisualComponent::GetAnimController()
{
	return m_animController;
}

void VisualComponent::SetMesh(atomic_ptr<Mesh>& NewMesh)
{
	mesh=NewMesh;

	//by default, we just grab the default model
	m_pMeshInstance = NewMesh->GetDefaultInstance();

	if (m_pSkeleton != NULL)
	{
		delete m_pSkeleton;
	}

	m_pSkeleton = m_pMeshInstance->GetSkeleton();
	m_pSkeleton->SetInitTransform(mOwner->GetLocation(), mOwner->GetOrientationMatrix());

	AnimationInstance* animInstance = NewMesh->GetAnimationInstance();
	if (animInstance != NULL)
	{
		if (m_animController != NULL)
		{
			delete m_animController;
			m_animController=NULL;
		}

		//Setup an animation controller that allows this entity to specify
		//the animation it wishes to present.
		if (m_animController == NULL)
		{
			m_animController = new AnimController();
			m_animController->SetEntity(mOwner);
		}

		godzassert(m_animController != NULL);
		m_animController->SetAnimationInstance(animInstance);

		//Play a default animation, this way the bones will be set to the default pose and
		//the mesh will render...
		m_animController->PlayAnim(0);
	}
	else
	{
		if (m_animController)
		{
			m_animController->SetAnimationInstance(NULL);
		}
	}

	//ResetCollision();

	//now that our visuals have changed, we need to make sure we upload visibility information
	//on the next pass
	//m_visualsHasChanged = true;

	//OnMeshChanged();
}
