
#pragma once


#include "ObjectTemplate.h"
#include "GodzVector.h"
#include "Mesh.h"
#include "atomic_ref.h"
#include "EntityComponent.h"
#include <CoreFramework/Collision/Physics.h>
#include <CoreFramework/Animation/AnimationDesc.h>


namespace GODZ
{
	class AnimationDesc;

	class GODZ_API EntityTemplate : public ObjectTemplate
	{
		DeclareGeneric(EntityTemplate, ObjectTemplate)

	public:

		virtual MotionType		GetMotionType() { return MotionType_Static; }

		void SetModel(atomic_ptr<Mesh> mesh) { mMesh = mesh; }
		atomic_ptr<Mesh> GetModel() { return mMesh; }

		GodzVector<atomic_ptr<AnimationDesc> >& GetAnimationList() { return mAnimationList; }
		GodzVector<atomic_ptr<EntityComponent> >& GetComponents() { return mComponentList; }

	protected:
		GodzVector<atomic_ptr<AnimationDesc> > mAnimationList;
		GodzVector<atomic_ptr<EntityComponent> > mComponentList;

		atomic_ptr<Mesh> mMesh;
	};
}
