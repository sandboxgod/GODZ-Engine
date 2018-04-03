/* ==============================================================
	Class: Mesh

	Mesh Instances consist of 'model resources'- geometric 
	elements that directly store vertex/index buffers for a 
	given model. Normally, a mesh will only reference one model 
	resource and perhaps reference another resource that may 
	visualize debug data so engineers can verify bounding boxes 
	are constructed properly, etc.

	Created Jan 3, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#if !defined(_Mesh_H_)
#define _Mesh_H_

#include "MeshInstance.h"
#include "GenericObject.h"
#include "atomic_ref.h"

namespace GODZ
{
	//forward decl
	class AnimationInstance;
	class SceneManager;
	struct AnimationTable;


	// Mesh (Geometric Object composed of vertices/indicies)
	class GODZ_API Mesh : public GenericObject
	{
		DeclareGeneric(Mesh, GenericObject)

	public:
		Mesh();
		~Mesh();

		//Set the model's draw offset from the origin. This property needs
		//to be set when a model was not properly centered when exported from the modeler.
		void SetOffsetLocation(const Vector3& newOffset);
		
		//Returns the offset this model is drawn from (world space origin)
		const Vector3& GetOffsetLocation() const;

		atomic_ptr<MeshInstance> GetDefaultInstance() { return m_pMeshInstance; }
		void SetDefaultInstance(atomic_ptr<MeshInstance>& ptr);

		//Creates Render Device Objects for all attached models
		void CreateRenderDeviceObjects();

		// Recomputes the bounding primitive
		void ComputeBounds();

		//returns the animation instance for this mesh
		AnimationInstance* GetAnimationInstance();

		// Returns a box that encapsulates all the model resources that composes this
		// mesh
		const WBox& GetBounds() const;

		//for save/load
		void SetBounds(const WBox& box);

		//sets animation instance
		void CreateAnimationInstance();

		//Notification (editor mode) this mesh has possibly been updated.
		virtual void OnPropertiesUpdated();

		void Serialize(GDZArchive& ar);

	protected:
		//Should return true if this Mesh type only stores 1 static instance
		virtual bool ShouldAlwaysUseDefault() { return true; }

		//Should return the MeshInstance subclass that will be spawned for new instances
		virtual GenericClass* GetMeshType();

	
		Vector3 m_offsetLoc;									//draw offset
		WBox box;												//Render bounds
		AnimationInstance* m_pAnimInstance;						//my animation instance
		atomic_ptr<MeshInstance> m_pMeshInstance;			//default mesh instance
	};
}

#endif