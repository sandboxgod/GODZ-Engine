/* ===========================================================
	GeometryInfo

	========================================================== 
*/

#if !defined(__GEOMETRYINFO_H__)
#define __GEOMETRYINFO_H__

#include "Godz.h"
#include "GenericObject.h"
#include <CoreFramework/Math/WBox.h>
#include <CoreFramework/Math/WMatrix.h>
#include "MaterialInfo.h"
#include "GizmoAxis.h"
#include "RenderDeviceBox.h"
#include <hash_map>

namespace GODZ
{
	struct SceneInfo;
	class ViewportInfo;

	struct GODZ_API RenderDeviceObjectInfo
	{
		bool mIsVisible;
		MaterialID matID;
		WBox box;
		Future<GenericReference<RenderDeviceObject> > mFuture;
	};

	class GODZ_API RenderDeviceContainer
	{
	public:
		void add(RenderDeviceObjectInfo& info);
		RenderDeviceObjectInfo& get(size_t index);
		size_t size() { return mList.size(); }
		void clear() { mList.clear(); }
		bool getMaterial(RenderDeviceObject* obj, MaterialID& matID);

	private:
		std::vector<RenderDeviceObjectInfo> mList;
	};

	class GODZ_API GeometryBaseInfo
	{
	public:
		enum VisualType
		{
			VisualType_Renderable,			//renderable type
			VisualType_ReferenceOnly		//shouldn't be rendered by normal pipeline
		};

	public:
		GeometryBaseInfo();

		//copies data from source (this) into the destination
		void	CopyInto(GeometryBaseInfo& dest) const
		{
			dest.m_type = m_type;
			dest.mIsShadowCaster = mIsShadowCaster;
			dest.m_nCurrentMaterial = m_nCurrentMaterial;	//current texture we should be rendering
			dest.m_name = m_name;
			dest.m_editorMaterial = m_editorMaterial;
		}

		VisualType			m_type;
		VisualID			m_id;				//unique id #
		bool				mIsShadowCaster;
		size_t				m_nCurrentMaterial;	//current texture we should be rendering
		HString				m_name;				//for debugging
		MaterialID			m_editorMaterial;	//Editor material
	};

	typedef stdext::hash_map<UInt64, RenderDeviceContainer > RenderDeviceMap;

	class GODZ_API GeometryInfo : public GeometryBaseInfo
	{
	public:
		GeometryInfo();		

		void				ClearRenderDeviceObjects();
		RenderDeviceContainer* GetContainer(const MaterialID& matID);

		void				AddMaterial(const MaterialID& matIndex);
		MaterialID			GetMaterial(size_t index);
		size_t				GetNumMaterials() const;
		void				ClearMaterials() { m_materials.clear(); }
		void				ReserveMaterials(size_t num);

		void				AddBone(const WMatrix16f& boneMat);

		//The bone matrices are guaranteed to be aligned in contigious memory locations. Do not ever
		//change the way this function works w/o changing all the callers in all the projects
		WMatrix16f&			GetBone(size_t index);
		size_t				GetNumBones() const;
		void				ClearBones();
		void				ReserveBones(size_t numBones);		//reserve # of bones in the vector
		bool				HasBones() { return m_boneTM.size() > 0;}

		//4x4 matrix containing scale / orientation / translation
		WMatrix				m_transform;	
		RenderDeviceMap mMap;

		std::vector<MaterialID>	m_materials;
		std::vector<WMatrix16f>	m_boneTM;		//skeletal anim data
	};

	/*
	* Render thread only info
	*/
	class GODZ_API RenderableGeometryInfo
	{
	public:
		RenderableGeometryInfo();
		~RenderableGeometryInfo();

		void				Render(SceneInfo* sceneInfo, RenderDeviceObject* obj, size_t matIndex);
		void				Render(SceneInfo* sceneInfo, size_t matIndex);


		GeometryInfo m_info;		//encapsulated info from main thread
		bool				m_isVisible;
		bool				m_isSelected;
		GizmoAxis mActiveAxis;

		std::vector<RenderDeviceBox> mVisibleDeviceObjs;
	};
}

#endif