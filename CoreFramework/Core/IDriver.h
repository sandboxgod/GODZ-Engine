/* ===========================================================
	Render Interface

	Provides state management and an abstract layer that provides
	access to rendering.

	Created Jan 2,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_IDRIVER_H_)
#define _IDRIVER_H_

#include "DeviceCreation.h"
#include "RenderData.h"
#include <CoreFramework/Math/Vector3.h>
#include <CoreFramework/Math/WMatrix.h>

namespace GODZ
{
	//Forward Declarations
	class Mesh;
	class ModelResource;
	class TextureResource;
	struct SceneInfo;
	class ViewportInterface;
	class WidgetInfo;
	class ViewportInfo;
	class RenderDeviceEvent;
	class RenderDeviceObject;
	class ObjectStateChangeEvent;
	class RenderData;
	class GpuEvent;
	class DeferredRenderManager;
	class ShadowMapManager;
	class HDRManager;
	class BlurManager;

	enum ERenderDeviceState
	{
		RDS_Fill,			//fill state
		RDS_Lighting,		//Turn lighting on/off
		RDS_Ambient,
		RDS_ZWriteEnable,	//ZWrite Enable for the z buffer
		RDS_MAX
	};

	enum ERenderDeviceValue
	{
		RDV_False = 0,
		RDV_True = 1,
		RDV_FillSolid = 0x000004,
		RDV_FillWireframe = 0x000008
	};

	enum ERenderPass
	{
		RP_SHADOW = 0,
		RP_GBUFFER,
		RP_MAIN,
		RP_UNLIT,

		RP_COUNT,
		RP_DEFAULT = RP_MAIN,
	};

	class GODZ_API IDriver
	{

	public:
		virtual ~IDriver() {}

		//handles an object change event
		virtual bool HandleObjectStateChangeEvent(ObjectStateChangeEvent* event, RenderData* m_renderData) = 0;

		//Notification we have received a request for a render-device data.....
		virtual void HandleRenderDeviceEvent(RenderDeviceEvent* event) = 0;

		//Begin preparing the scene in the back buffer
		virtual void BeginScene()=0;

		//Scene preparation has completed. Ready to present the back buffer to the user.
		virtual void EndScene(SceneInfo& sceneInfo)=0;

		//clears the background
		virtual void Clear()=0;

		//Notifies driver to render out primitives and the HUD
		virtual void RenderPrimitives(SceneInfo& sceneInfo)=0;

		//Computes a ray transformed from screen-to-world space
		virtual WRay ComputePickingRay(int x, int y)=0;

		//Sets the Window handle this driver should render to.
		virtual void CreateDevice(const DeviceCreation& deviceCreation) = 0;

		//Creates a GPU event
		virtual GpuEvent* CreateGpuEvent(rstring label, u32 color) = 0;
		virtual void DestroyGpuEvent(GpuEvent* pEvent) = 0;

		//Renders a model resource
		virtual void DrawModel(SceneInfo& sceneInfo, RenderDeviceObject* rdo)=0;

		//Destroys the viewport represented by this interface.
		virtual void DestroyViewport(ViewportInterface* VI)=0;

		virtual udword GetWidth() = 0;
		virtual udword GetHeight() = 0;
		virtual float  GetFOV() = 0;
		virtual float GetNearPlane() = 0;
		virtual float GetFarPlane() = 0;

		virtual ShadowMapManager* GetShadowMapManager() = 0;
		virtual HDRManager* GetHDRManager() = 0;
		virtual DeferredRenderManager* GetDeferredRenderManager() = 0;
		virtual BlurManager* GetBlurManager() = 0;

		//Returns the viewport 
		virtual ViewportInterface* GetViewport(size_t id)=0;
		virtual ViewportInterface* GetViewportByWindow(GODZHWND window)=0;

		//Returns the number of viewports currently created.
		virtual size_t GetNumViewports()=0;

		//Renders the text buffer
		virtual void RenderText(const char* text, int x, int y, int width, int height)=0;

		//sets the background color of the display
		virtual void SetBackgroundColor(udword r, udword g, udword b)=0;

		//sets the foreground color
		virtual void SetForegroundColor(udword r, udword g, udword b, udword a)=0;

		//Set RenderDeviceState
		//[in] state - render state
		//[in] value - ERenderDeviceValue (see ERenderDeviceState struct) or combination of values
		virtual void SetRenderState(ERenderDeviceState state, ulong value) = 0;

		virtual void SetTexture(ulong stage, TextureResource* pResource)=0;

		//sets current world matrix
		virtual void SetTransform(const WMatrix16f& matrix)=0;

		//Set the view/projection matrices
		virtual void SetViewMatrix(const WMatrix& viewMatrix)=0;
		virtual void SetViewMatrix(const Vector3& eye, const Vector3& lookAt, const Vector3& up)=0;

		virtual void SetViewport(ViewportInterface* viewport)=0;

		virtual void SetProjection(const ProjectionMatrixInfo& projMat) = 0;
		virtual void GetProjectionMatrix(WMatrix& mat) = 0;


		virtual void RenderSky(SceneInfo* pScene, ViewportInfo* pView, ERenderPass pass) = 0;
		virtual void RenderScene(SceneInfo* pScene, ViewportInfo* pView, RenderGeoInfoPtrList & list, ERenderPass pass = RP_DEFAULT) = 0;
		virtual void RenderEditor(SceneInfo* pScene, ViewportInfo* pView, RenderGeoInfoPtrList & list) = 0;

		virtual void DrawQuad() = 0;
		virtual void DrawQuad(int x, int y, int sizeX, int sizeY, RenderDeviceObject* pTextureObject) = 0;
	};
}

#endif