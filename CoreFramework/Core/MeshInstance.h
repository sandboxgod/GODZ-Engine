

#include "GenericObject.h"
#include "Material.h"
#include "ModelResource.h"
#include "SceneInfo.h"
#include "GenericObjectList.h"
#include "GodzVector.h"
#include <hash_map>
#include <CoreFramework/Animation/Skeleton.h>
#include <CoreFramework/Core/atomic_ref.h>
#include <CoreFramework/Math/WMatrix.h>

#if !defined(__MESH_INSTANCE_H_)
#define __MESH_INSTANCE_H_


namespace GODZ
{
	//foward decl
	class Mesh;
	class ShaderResource;
	class AnimController;

	/*
	* Collection of models
	*/
	class GODZ_API ModelContainer : public GenericObject
	{
		DeclareGeneric(ModelContainer, GenericObject)

	public:
		size_t size() const { return mList.size(); }
		MeshResource get(size_t index);
		void add(ModelResource* res) { mList.push_back(res); }
		void add(ModelContainer& source);
		bool drop(ModelResource* res);
		void clear() { mList.clear(); }

		GodzVector<MeshResource>& GetMeshes() { return mList; }

	private:
		GodzVector<MeshResource> mList;
	};

	//note: changing existing numbers will break existing models
	enum ECollisionType
	{
		CT_Box   = 0,			//use box collision
		CT_Tree  = 1			//use Axis Aligned Box Tree
	};

	//Mesh instances concept is actually here because animated meshes each need
	//their own vertex/index buffer to deform. Each individual mesh instance
	//can be linked directly to an entity. If it is not, then it is treated
	//as a default instance that other mesh instances should use to derive
	//the original data from.
	class GODZ_API MeshInstance 
		: public GenericObject
	{
		DeclareGeneric(MeshInstance, GenericObject)

	public:
		//each material can refer to (n) number of models.
		typedef stdext::hash_map<UInt32, atomic_ptr<ModelContainer> > MeshMap2;
		typedef stdext::hash_map<Material*, atomic_ptr<ModelContainer> > MeshMap;

		MeshInstance();
		virtual ~MeshInstance();

		GodzVector<atomic_ptr<Material> >& GetMaterials();

		void AddMaterial(const atomic_ptr<Material>& material);
		size_t	GetNumMaterials();
		atomic_ptr<Material> GetMaterial(size_t index);
		Material* GetMaterialByName(HString text);

		//reflection --- collision type
		int GetCollisionShape();
		void SetCollisionShape(int shape);
		ECollisionType		GetCollisionType();

		virtual Skeleton* GetSkeleton();
		void AddNode(const SkeletonNode& node);

		// Adds a model resource to this mesh
		void AddModelResource(Material* material, ModelResource* resource);
		ModelContainer* GetModelResources(Material* material);

		//Creates Render Device Objects for all attached models
		virtual void CreateRenderDeviceObjects();

		virtual void Initialize(Mesh* mesh);

		//notification this mesh instance properties may have been updated
		virtual void OnPropertiesUpdated();

		//returns mesh associated with this instance.
		Mesh* GetMesh();
		void SetMesh(Mesh* pMesh);

		//Copies the shader to all materials
		void SetShader(ShaderResource* pShader);

		virtual void Transform(float deltaTime, Skeleton* skeleton, AnimController* controller);

		// Tells the model resource to split within the Bounding Box Array.
		void Slice(std::vector<WPlane>& planes, size_t minNumberTriangles);

		MeshMap2& GetMeshMap2() { return mMeshMap; }
		MeshMap GetMeshMap();

		virtual void Serialize(GDZArchive& ar);

	protected:
		bool SliceModels(const WPlane& plane, ModelContainer* mc, size_t minNumberTriangles);
		bool FindMaterialIndex(Material* m, UInt32& index);

		GodzVector<atomic_ptr<Material> > materials;			//materials list
		Mesh* mesh;									//owner of this instance
		ECollisionType m_kCollType;					//collision setting

		MeshMap2 mMeshMap;
		std::vector<SkeletonNode> m_nodes;
	};
}

#endif
