/* ===========================================================
	RenderDeviceEvent.h


	Copyright (c) 2008, Richard Osborne
	========================================================== 
*/

#if !defined(__RenderDeviceEvent__H__)
#define __RenderDeviceEvent__H__

#include "Godz.h"
#include "GodzAtomics.h"
#include "RenderDeviceObject.h"
#include "VertexTypes.h"
#include "ShaderTechniqueList.h"
#include "TriangleList.h"
#include <CoreFramework/Math/WMatrix.h>
#include "Color4f.h"
#include "TextureInfo.h"

namespace GODZ
{
	class NavigationMesh;
	class ModelResource;
	struct Vertex;
	struct ShaderTechniqueList;

	enum GODZ_API RenderDeviceEventID
	{
		RenderDeviceEventID_CreateModelResource,		//create a model resource
		RenderDeviceEventID_CreateTexture,				//msg to create a texture
		RenderDeviceEventID_CreateHBITMAP,				//msg to create an HBITMAP
		RenderDeviceEventID_CreateShader,
		RenderDeviceEventID_HighDynamicRange,
		RenderDeviceEventID_DestroyEvent,
		RenderDeviceEventID_CreateTriMesh,
		RenderDeviceEventID_UpdateTriMesh,
		RenderDeviceEventID_RemoveTriMesh,
		RenderDeviceEventID_CreateLine,
		RenderDeviceEventID_CreateCylinder,
		RenderDeviceEventID_CreateSphere,
		RenderDeviceEventID_Max
	};

	struct GODZ_API VertexParams
	{
		VertexArray m_vb;				//packed vertex structures
		std::vector<UInt16> m_ib;			//index buffer
		std::vector<ulong> m_attrb;		//attribute buffer
		udword m_ResourceType;
		unsigned long m_numVerts;
		unsigned long m_numInd;				//num of indicies
		EVertexType	m_kVertexType;
		udword m_vertexStructSize;
	};

	/*
	*	Base class for Render Device Events. Member variables should not member accessed directly from main thread.
	*	If main thread wishes to receive data from renderer it needs to declare a 'Future' that will be populated later
	*/
	class GODZ_API RenderDeviceEvent
	{
	public:
		virtual ~RenderDeviceEvent()
		{
		}

		virtual RenderDeviceEventID GetEventType() = 0;

		//Note: any data that the client wishes to access from this object should be a future...
		//Note: main thread should not ever try to access data in this object directly (TODO: restrict access to this object)
	};

	/*
	* RenderDeviceObject
	*/
	class GODZ_API RenderDeviceObjectEvent : public RenderDeviceEvent
	{
	public:
		Future<GenericReference<RenderDeviceObject> >	m_renderDeviceObject;
	};

	class GODZ_API DestroyEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_DestroyEvent; }
	};

	class GODZ_API HDREvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_HighDynamicRange; }
	};

	/*
	* VertexParamsEvent
	*/
	class GODZ_API VertexParamsEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateModelResource; }

		VertexParams					m_params;
		rstring							m_filename;
		bool							m_recomputeNormals;
	};

	/*
	* TexureEvent
	*/
	class GODZ_API TextureEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateTexture; }

		Future<TextureInfo>				m_texInfo;
		std::vector<UInt8>				m_pImageData;							//image data
		long							m_numBytes;
		rstring							m_filename;
	};

	/*
	* 
	*/
	class GODZ_API BitmapEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateHBITMAP; }

		Future<GODZHANDLE>				m_bitmap;
	};

	/*
	* 
	*/
	class GODZ_API ShaderEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateShader; }


		Future<ShaderTechniqueList>	m_techniqueList;
		rstring							m_filename;
	};

	class GODZ_API TriMeshEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateTriMesh; }

		TriangleList m_mesh;
		Color4f color;
		WMatrix m;
	};

	//Updates the transform of a Graphics Primitive/Mesh
	class GODZ_API UpdateMeshTransformEvent : public RenderDeviceEvent
	{
	public:
		UpdateMeshTransformEvent()
			: tm(1)
		{
		}

		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_UpdateTriMesh; }

		WMatrix tm;
		GenericReference<RenderDeviceObject>	m_renderDeviceObject;
	};

	class GODZ_API RemoveMeshEvent : public RenderDeviceEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_RemoveTriMesh; }
		GenericReference<RenderDeviceObject>	m_renderDeviceObject;
	};

	class GODZ_API LineMeshEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateLine; }

		Vector3 start;
		Vector3 end;
		Color4f color;
	};

	class GODZ_API CreateCylinderEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateCylinder; }

		float radius1;
		float radius2;
		float length;
		unsigned int slices;
		unsigned int stacks;
	};

	//Sphere
	class GODZ_API CreateSphereEvent : public RenderDeviceObjectEvent
	{
	public:
		virtual RenderDeviceEventID GetEventType() { return RenderDeviceEventID_CreateSphere; }

		float radius;
		Vector3 center;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	* Creates a Cylinder or Cone modelresource
	*/
	GODZ_API CreateCylinderEvent* CreateCylinder(Future<GenericReference<RenderDeviceObject> >& future, float radius1, float radius2, float length, unsigned int slices, unsigned int stacks);

	/*
	*
	*/
	GODZ_API RenderDeviceEvent* CreateTriMesh(Future<GenericReference<RenderDeviceObject> >& future, TriangleList& m, Color4f& color, const WMatrix& trans);

	/*
	*	Notification to the render device to set HDR fx
	*/
	RenderDeviceObjectEvent* CreateHDREvent(Future<GenericReference<RenderDeviceObject> >& future);

	/*
	*	Notification to the render device to destroy this object
	*/
	RenderDeviceObjectEvent* CreateDestroyEvent(Future<GenericReference<RenderDeviceObject> >& future);

	/*
	*	Notification to the render device to create a model based on the following data....
	*	ResourceType - see EResourceFlag enum in ResourceObject.h
	*/
	VertexParamsEvent* CreateModelEvent(Future<GenericReference<RenderDeviceObject> >& future, VertexParams& params, rstring filename, bool requiresTangentBinormalRebuild);

	/*
	*	Notification to the render device to create a shader
	*/
	ShaderEvent* CreateShaderEvent(Future<GenericReference<RenderDeviceObject> >& future, Future<ShaderTechniqueList>& techniqueList, rstring filename);

	/*
	*	Notification to the render device to create a texture
	*/
	TextureEvent* CreateTextureEvent(Future<GenericReference<RenderDeviceObject> >& future, Future<TextureInfo>& textureInfoFuture, UInt8* imgData, long numBytes, rstring filename);

	/*
	*	Assuming we have a pointer to the texture device obj- this event will pursue
	*	acquiring the HBITMAP
	*/
	BitmapEvent* CreateHBITMAPEvent(Future<GenericReference<RenderDeviceObject> >& deviceObj, Future<GODZHANDLE>& future);
}

#endif //__RenderDeviceEvent__H__
