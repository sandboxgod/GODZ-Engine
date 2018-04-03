#include "DeferredRenderManager.h"
#include "DebugRenderer.h"
#include <CoreFramework/Core/HDRManager.h>
#include <CoreFramework/Core/BlurManager.h>
#include <CoreFramework/Core/IDriver.h>
#include <CoreFramework/Core/RenderOptions.h>
#include <CoreFramework/Core/RenderQueue.h>
#include <CoreFramework/Core/SceneInfo.h>
#include <CoreFramework/Core/ShadowMapManager.h>
#include <CoreFramework/Core/ViewportInterface.h>

#include "GpuEvent.h"
#include "RendererHelper.h"

namespace GODZ
{
//There's a problem with PSSM whereas inbetween the clipped viewports there will be a "space"
//whereas just the pure framebuffer will display. This occurs because the far plane == viewport.maxZ.
//Thus, what we do is add a little extra room to the far plane for each viewport to resolve this
//dead zone whereas pixels can't be rendered into.
static const float		FAR_PLANE_EXTENT2 = 100.0f;

// Calculates the frustum split distances, or in other
// words, the near and far planes for each split.
//
//
void CalculateSplitDistances(float _fCameraNear, float _fCameraFar, SceneInfo& sceneInfo)
{
	// Practical split scheme:
	//
	// CLi = n*(f/n)^(i/numsplits)
	// CUi = n + (f-n)*(i/numsplits)
	// Ci = CLi*(lambda) + CUi*(1-lambda)
	//
	// lambda scales between logarithmic and uniform
	//
	RenderOptions* opt = RenderOptions::GetInstance();

	if (!opt->m_bManualSplitPlanes)
	{
		for(unsigned int i=0; i < opt->m_numSplits;i++)
		{
			float fIDM=i/(float)opt->m_numSplits;
			float fLog=_fCameraNear*powf((_fCameraFar/_fCameraNear),fIDM);
			float fUniform=_fCameraNear+(_fCameraFar-_fCameraNear)*fIDM;
			sceneInfo.m_pSplitDistances[i]=fLog*opt->m_fSplitSchemeLambda+fUniform*(1-opt->m_fSplitSchemeLambda);
		}

		// make sure border values are right
		sceneInfo.m_pSplitDistances[0]=_fCameraNear;
		sceneInfo.m_pSplitDistances[opt->m_numSplits]=_fCameraFar;
	}
	else
	{
		//applies a linear distribution across the splits
		sceneInfo.m_pSplitDistances[0]=_fCameraNear;
		sceneInfo.m_pSplitDistances[opt->m_numSplits]=_fCameraFar;

		for(unsigned int i=1;i<opt->m_numSplits;i++)
		{
			sceneInfo.m_pSplitDistances[i] = opt->m_split[i] * (_fCameraFar - _fCameraNear);
		}
	}
}

// Adjusts the camera planes to contain the visible scene
// as tightly as possible. This implementation is not very
// accurate.
// returns - camera far
float AdjustCameraPlanes(Vector3 sceneAABBPoints[8], const WMatrix& viewMat, float fCameraNear)
{

	// Find the most distant point of AABB
	//
	float fMaxZ=0;
	for(int i=0;i<8;i++)
	{
		// transform z coordinate with view matrix
		float fZ=sceneAABBPoints[i].x*viewMat._13+sceneAABBPoints[i].y*viewMat._23+sceneAABBPoints[i].z*viewMat._33+1*viewMat._43;

		// check if its largest
		if(fZ>fMaxZ) fMaxZ=fZ;
	}

	// use largest Z coordinate as new far plane
	return fMaxZ+fCameraNear;
}

void RenderShadowmapSunLight(ViewportInfo& viewport, SceneInfo& sceneInfo, IDriver* Driver)
{
	RenderOptions* opt = RenderOptions::GetInstance();

	//reset the num of shadowmaps to the max # specified by user config
	opt->m_numSplits = opt->GetMaxNumSplits();

	if (opt->m_numSplits > 0 && opt->bRenderShadowMaps)
	{
#ifdef TRACK_PROFILER_RENDERER
		m_profileShadowmap.profilerBeginTime();
#endif
		GPUEVENT_STATIC(sceneInfo.m_pDriver, RenderShadowmap, "RenderShadowmap", 0xFF000000);

		CameraInfo* camera = sceneInfo.m_camera;



		ViewportInterface* pView = sceneInfo.m_pViewport;

		//tODO: Need to grab all the elements using the CURRENT projection im thinking. But then again I guess thats probably
		//costly to create a new frustum for each split and test each object in the scene again not sure. might be a quick
		//way to sort based on the depth of the object and check to see if its between my near/far
		
		
		size_t numGlobalElements = sceneInfo.m_renderData->GetNumElements();

		ProjectionMatrixInfo projMat = viewport.GetProjectionInfo();

		//get the light associated with this scene
		SceneLight* sunLight = sceneInfo.m_renderData->GetSunLight();

		//set sun so it will be available during editor forms
		sceneInfo.m_light = sunLight;
		sceneInfo.m_hasShadows = true;

		//stores elements that will be submitted this frame for rendering...
		RenderGeoInfoPtrList localVisibleElements;

		if (numGlobalElements > 0)
		{
			localVisibleElements.reserve(numGlobalElements);

			//compute AABB (ViewBox) - We need to do this before we start rendering because the Shadowmap code will need to know
			//the projection.
			std::vector<Vector3> shadowCasterPoints;					//list of AABB points
			shadowCasterPoints.reserve(2 * numGlobalElements); //max # of AABBs (entities) * Min+max (n * 2)

			for(size_t i=0;i<numGlobalElements;i++)
			{
				RenderGeoInfoPtrList::RenderGeoData& data = sceneInfo.m_renderData->GetRenderGeoData(i);
				data.mList.clear();
				RenderableGeometryInfo* sceneElement = data.m_renderInfo;

				if (sceneElement->m_isVisible)
				{
					size_t len = sceneElement->mVisibleDeviceObjs.size();
					for(size_t i=0; i<len; i++)
					{
						if (sceneElement->m_info.mIsShadowCaster)
						{
							RenderDeviceBox& rb = sceneElement->mVisibleDeviceObjs[i];

							shadowCasterPoints.push_back(rb.box.GetMin());
							shadowCasterPoints.push_back(rb.box.GetMax());
						}
						data.mList.push_back(sceneElement->mVisibleDeviceObjs[i]);
					}
					localVisibleElements.add(data);
				}
			}

			//nothing to render?
			if (shadowCasterPoints.size() == 0)
			{
				sceneInfo.m_hasShadows = false;
				return;
			}

			//Now generate the ViewBox in World Space
			Vector3 viewBoxMin, viewBoxMax;
			SetMinMaxBound<Vector3>(&shadowCasterPoints[0], shadowCasterPoints.size(), viewBoxMin, viewBoxMax);
			WBox viewBox(viewBoxMin, viewBoxMax);

			if (sunLight != NULL)
			{
				Vector3 viewpos = sunLight->m_viewerPosition;

				//build a viewbox around the viewer...
				
				float size = sceneInfo.m_renderData->GetViewBoxSize();

				if (size > 0 && opt->mForceViewBoxSize)
				{
					Vector3 vmin = viewpos + Vector3(-size, viewBoxMin.y, -size);
					Vector3 vmax = viewpos + Vector3(size, size, size);
					WBox viewerBox(vmin, vmax);

					//verify the dimensions of our new box; check to see if it's smaller
					if (viewerBox.GetSize() < viewBox.GetSize())
					{
						viewBox = viewerBox;
					}
				}
			}

			sceneInfo.m_viewBox = viewBox;
			viewBox.GetAABBPoints(sceneInfo.m_viewBoxPoints);
		}

		//for parallel split algorithm. TODO: Move this stuff somewhere else perhaps
		const WMatrix& viewMat = camera->m_viewMatrix;

		//Note: there is some problem with adjust camera planes function
		projMat.far_plane = AdjustCameraPlanes(sceneInfo.m_viewBoxPoints, viewMat, projMat.near_plane);
		CalculateSplitDistances(projMat.near_plane, projMat.far_plane, sceneInfo);


		float fScale=1.1f;

		
		
		//compute frustum points for the entire scene
		Vector3 frustum[8];
		
		ViewFrustum::CalculateFrustumCorners(frustum,camera->m_viewMatrixInfo.eye,camera->m_viewMatrixInfo.lookAt,camera->m_viewMatrixInfo.up,
			projMat.near_plane,projMat.far_plane,projMat.fov,projMat.aspectRatio,fScale);

		

		if (sunLight != NULL)
		{
			//TODO: isnt this computed already?
			//Compute the light source view matrix (translation + rotation)
			sceneInfo.m_lightView.MakeIdentity();
			ComputeViewMatrix(sunLight->m_viewMatrixInfo.eye, sunLight->m_viewMatrixInfo.lookAt, sunLight->m_viewMatrixInfo.up, sceneInfo.m_lightView);
		}
		else
		{
			return;
		}

		//sceneInfo.m_lightView = sceneInfo.m_renderData->GetSunlightView();
		

		// do parallel split algorithm
		// Render each split from front to back

		// Build up the frustrum
		ViewFrustum frustums[MAX_NUM_SPLITS];
		for(unsigned int iSplit=0;iSplit<opt->m_numSplits;iSplit++)
		{
			// near and far planes for current frustum split
			projMat.near_plane=sceneInfo.m_pSplitDistances[iSplit];
			projMat.far_plane=sceneInfo.m_pSplitDistances[iSplit+1] + FAR_PLANE_EXTENT2;

			//build a frustum for this new split-plane projection and test the models....
			WMatrix projectionMat(1);
			ComputePerspectiveFovLH(projMat, projectionMat);
			WMatrix combMat = viewMat * projectionMat;

			frustums[iSplit].ExtractPlanes(combMat);
		}

		//get the sunlight frustum
		ViewFrustum lightFrustum = sceneInfo.m_renderData->GetSunlightFrustum();


		RenderGeoInfoPtrList renderListShadow[MAX_NUM_SPLITS];
		//make a local list of elements that overlap the minX / minY clip volume of this viewport. We could
		//let the video card do this culling or do this here (if we're not GPU bound).
		size_t numElements = localVisibleElements.size();
		for(size_t i=0;i<numElements;i++)
		{
			RenderGeoInfoPtrList::RenderGeoData& data = localVisibleElements[i];
			RenderGeoInfoPtrList::RenderGeoData splitData;
			splitData.m_renderInfo = data.m_renderInfo;


			//element not visible this pass (but it may be during the next btw...)
			for(unsigned int iSplit=0;iSplit<opt->m_numSplits;iSplit++)
			{
				size_t len = data.mList.size();
				splitData.mList.clear();
				for(size_t j=0;j<len;j++)
				{
					WBox box = data.mList[j].box;
					WSphere s = box;

					//NOTE: Problem with using the light frustum; its tweaked to cast shadows on what the viewer sees.
					//However, it's FOV is currently so wide it may diminish the effectiveness of our culling here.
					if (data.m_renderInfo->m_info.mIsShadowCaster 
						&& (frustums[iSplit].SphereInFrustum(s) != ViewFrustum::FrustumResult_OUT || lightFrustum.SphereInFrustum(s) != ViewFrustum::FrustumResult_OUT)
						)
					{
						splitData.mList.push_back(data.mList[j]);
					}
				}

				renderListShadow[iSplit].add(splitData);
			}
		}

		ShadowMapManager* shadows = Driver->GetShadowMapManager();
		unsigned long shadowmapsize = (unsigned long)shadows->GetShadowMapSize();

		//render shadow map pass
		shadows->Begin(0, &sceneInfo);

		godzassert(opt->m_numSplits < MAX_NUM_SPLITS);
		float numSplits = static_cast<float>(opt->m_numSplits);
		for(unsigned int iSplit=0;iSplit<opt->m_numSplits;iSplit++)
		{
			Vector2 atlasValue = shadows->GetAtlasSplit(iSplit);

			//note: multiply atlasoffset * shadowmapsize
			pView->SetViewport(atlasValue.x*shadowmapsize, atlasValue.y*shadowmapsize, shadowmapsize/2, shadowmapsize/2, 0, 1);

			GPUEVENT(sceneInfo.m_pDriver, Draw, StringVar("Shadowmap (%d/%d)", iSplit+1, opt->m_numSplits).c_str(), 0xFF000000);

			// near and far planes for current frustum split
			projMat.near_plane=sceneInfo.m_pSplitDistances[iSplit];
			projMat.far_plane=sceneInfo.m_pSplitDistances[iSplit+1] + FAR_PLANE_EXTENT2;

			Driver->SetProjection(projMat);

#ifdef _DEBUG
			//debugging CSM split rendering
			if (opt->m_nSplitToRender < opt->m_numSplits && opt->m_nSplitToRender != iSplit) 
			{
				continue;
			}
#endif

			// Calculate corner points of frustum split
			//
			// To avoid edge problems, scale the frustum so
			// that it's at least a few pixels larger
			//
			ViewFrustum::CalculateFrustumCorners(sceneInfo.m_frustumPoints,camera->m_viewMatrixInfo.eye,camera->m_viewMatrixInfo.lookAt,camera->m_viewMatrixInfo.up,
				projMat.near_plane,projMat.far_plane,projMat.fov,projMat.aspectRatio,fScale);


			//Multiply all 8 frustum vertices by this matrix. Then you have your frustum in lightspace. 
			//You can then calculate a simple AABB around that frustum and use its dimensions to create 
			//an appropriate projection matrix (D3DXMatrixOrthoOffCenterLH).
			Vector3 lightFrustum[NUM_FRUSTUM_POINTS];
			for (int i = 0; i < NUM_FRUSTUM_POINTS; i++)
			{
				lightFrustum[i] = sceneInfo.m_frustumPoints[i];
				sceneInfo.m_lightView.Mul(lightFrustum[i]);
			}

			Vector3 lmin, lmax;
			SetMinMaxBound<Vector3>(lightFrustum, NUM_FRUSTUM_POINTS, lmin, lmax);
			sceneInfo.m_lightViewBox.SetMinMax(lmin, lmax);


			shadows->BeginSplit(iSplit, &sceneInfo); //compute lightview matrix
			Driver->RenderScene(&sceneInfo, &viewport, renderListShadow[iSplit], RP_SHADOW);
		}

		shadows->End(0);


#ifdef TRACK_PROFILER_RENDERER
		m_profileShadowmap.profilerEndTime("Renderer::Shadowmap");
#endif
	}
}



void RenderMain(ViewportInfo& viewport, SceneInfo& sceneInfo, IDriver* Driver)
{
#ifdef TRACK_PROFILER_RENDERER
	m_profileRenderMain.profilerBeginTime();
#endif

	GPUEVENT_STATIC(sceneInfo.m_pDriver, RenderMain, "RenderMain", 0xFF000000);

	RenderOptions* opt = RenderOptions::GetInstance();

	if (sceneInfo.m_renderData->GetNumElements() > 0)
	{
		//we did not cull inidividual resources this frame
		viewport.SetVisibilityFlags(false);

		//reset viewport projection
		Driver->SetProjection(viewport.GetProjectionInfo());

		//Render sky dome
		Driver->RenderSky(&sceneInfo, &viewport, RP_MAIN);

		//Render the scene
		Driver->RenderScene(&sceneInfo, &viewport, sceneInfo.m_renderData->GetRenderableList());

		Driver->RenderPrimitives(sceneInfo);
	}

#ifdef TRACK_PROFILER_RENDERER
	m_profileRenderMain.profilerEndTime("Renderer::Main");
#endif
}

void RendererHelper::Render(ViewportInfo& viewInfo, SceneInfo& sceneInfo, IDriver* Driver, bool isEditor)
{
	Driver->Clear();
	Driver->BeginScene();

	Driver->SetViewMatrix(sceneInfo.m_camera->m_viewMatrix);

	//useful for debugging the viewMatrix used for sunlight culling
	//SceneLight* sunLight = sceneInfo.m_renderData->GetSunLight();
	//Driver->SetViewMatrix(sceneInfo.m_renderData->GetSunlightView());

	RenderShadowmapSunLight(viewInfo, sceneInfo, Driver);


	//Begin the HDR Effect which will capture the framebuffer then perform post effects on it
	Driver->GetHDRManager()->Begin();

	RenderOptions* opt = RenderOptions::GetInstance();

	if(opt->mIsDeferredRenderingEnabled && opt->m_bUseDeferred)
	{
		Driver->GetDeferredRenderManager()->Render(viewInfo, sceneInfo, Driver, isEditor);
	}
	else
	{
		RenderMain(viewInfo, sceneInfo, Driver);
		Driver->RenderEditor(&sceneInfo, &viewInfo, sceneInfo.m_renderData->GetRenderableList());
	}

	Driver->GetHDRManager()->End();
	Driver->GetHDRManager()->Render();

	// Draw the debug
	if (!isEditor)
	{
		DebugRenderer::Render(viewInfo, sceneInfo, Driver);
	}

	Driver->EndScene(sceneInfo); //present the buffer
}

} // namespace GODZ
