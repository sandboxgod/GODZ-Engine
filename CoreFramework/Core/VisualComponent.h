/* ===========================================================
	CameraInfo.h

	Copyright (c) 2012
	========================================================== 
*/

#if !defined(__VISUALCOMPNENT__H__)
#define __VISUALCOMPNENT__H__

#include "atomic_ref.h"
#include "EntityComponent.h"
#include "Mesh.h"
#include "SlotIndex.h"

namespace GODZ
{
	class AnimController;
	class MeshInstance;
	class Skeleton;

	
	class GODZ_API VisualComponent : public EntityComponent
	{
		DeclareGeneric(VisualComponent, EntityComponent)

	public:
		VisualComponent();
		VisualComponent(WEntity* owner);
		static HashCodeID GetVisualComponentHash() { return 0x956B3FBC; } //"VisualComponent"
		virtual HashCodeID GetComponentName() { return GetVisualComponentHash(); }

		/// Exposed to Artists so they can type in Mesh we wish to display
		atomic_ptr<Mesh>		GetMesh();
		void					SetMesh(atomic_ptr<Mesh>& mesh);

		//Returns the animation controller used by this mesh
		AnimController*			GetAnimController();

	private:
		VisualID			m_id;					//unique id #
		atomic_ptr<Mesh>	mesh;					//mesh definition
		MeshInstance*		m_pMeshInstance;		//mesh instance
		Skeleton*			m_pSkeleton;			//node hierarchy
		AnimController*		m_animController;		//current animation instance
	};
}

#endif //__VISUALCOMPNENT__H__
