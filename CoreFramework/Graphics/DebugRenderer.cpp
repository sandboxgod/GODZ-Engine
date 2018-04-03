#include "DebugRenderer.h"
#include "GpuEvent.h"
#include "DeferredRenderManager.h"
#include <CoreFramework/Core/FPSCounter.h>
#include <CoreFramework/Core/IDriver.h>
#include <CoreFramework/Core/RenderOptions.h>
#include <CoreFramework/Core/SceneInfo.h>
#include <CoreFramework/Core/ShadowMapManager.h>

namespace GODZ
{

void RenderStats(SceneInfo &sceneInfo, IDriver* Driver)
{
	GPUEVENT_STATIC(sceneInfo.m_pDriver, RenderStats, "RenderStats", 0xFF000000);

	static const int poly_buf_size = 512;
	static char poly_buf[poly_buf_size];

	RenderOptions* renderState = RenderOptions::GetInstance();

	FPSCounter *fpsCounter = FPSCounter::GetInstance();
	if (renderState != NULL && renderState->bShowFPS)
	{
		//scale to the viewport dimension
		int h = int(0.075f * 500);
		int w = int(0.46f * 560);
		int x = 5;
		int y = 5;

		float fps = fpsCounter->GetFPS();
		_snprintf_s(poly_buf, poly_buf_size, sizeof(poly_buf), "Resources %d Vertices: %d FPS: %f", sceneInfo.m_nModelsDrawn, sceneInfo.m_nVertices, fps );
		Driver->RenderText(poly_buf, x, y, w, h);

		y += 15;
		_snprintf_s(poly_buf, poly_buf_size, sizeof(poly_buf), "Deferred Shading: (%s). Press [ to toggle.", RenderOptions::GetInstance()->m_bUseDeferred?"On":"Off" );
		Driver->RenderText(poly_buf, x, y, w, h);

		// Render the tips
		y += 15;
		_snprintf_s(poly_buf, poly_buf_size, sizeof(poly_buf), "Alt-Shift-V: Toggle debug render targets");
		Driver->RenderText(poly_buf, x, y, w, h);
		y += 15;
		_snprintf_s(poly_buf, poly_buf_size, sizeof(poly_buf), "Alt-Shift-X: Reload shaders");
		Driver->RenderText(poly_buf, x, y, w, h);
	}
}

void RenderDebugShadowmaps(SceneInfo &sceneInfo, IDriver* Driver)
{
	RenderOptions* opt = RenderOptions::GetInstance();
	// Render the debug shadowmap info if needed
	if(	opt->m_numSplits > 0 &&
		opt->bRenderShadowMaps &&
		opt->m_renderShadowmaps
		)
	{
		GPUEVENT_STATIC(sceneInfo.m_pDriver, RenderStats, "RenderDebugShadowmaps", 0xFF000000);

		int size, border = 20;

		size = Driver->GetHeight() / 6;
		int startY = Driver->GetHeight() - border - size;

		//right now only 1 shadowmap used
		for(unsigned int iSplit=0;iSplit<1;iSplit++)
		{
			int startX = border + (size + border) * iSplit;
			Driver->DrawQuad(startX, startY, size, size, 
								Driver->GetShadowMapManager()->GetShadowMapTextureObject(iSplit));
		}
	}
}

void RenderDebugDeferredRenderTargets(SceneInfo &sceneInfo, IDriver* Driver)
{
	int numTargets  = RT_COUNT;
	int border = 20;
	int sizeX = (Driver->GetWidth() - (numTargets+1)*border)/numTargets;
	int sizeY = (sizeX * Driver->GetHeight())/Driver->GetWidth();

	int startY = Driver->GetHeight() - border - sizeY;
	for(unsigned int i=0;i<RT_COUNT;++i)
	{
		int startX = border + (sizeX + border) * i;
		Driver->DrawQuad(startX, startY, sizeX, sizeY,
							Driver->GetDeferredRenderManager()->GetTextureObject(static_cast<RenderTargetType>(i)));
	}

}

void DebugRenderer::Render(ViewportInfo &viewInfo, SceneInfo &sceneInfo, IDriver* Driver)
{
	GPUEVENT_STATIC(sceneInfo.m_pDriver, RenderStats, "DebugRenderer::Render", 0xFF000000);

	if(RenderOptions::GetInstance()->m_bDebugRenderDeferredTexture)
	{
		RenderDebugDeferredRenderTargets(sceneInfo, Driver);
	}
	else
	{
		RenderDebugShadowmaps(sceneInfo, Driver);
	}
	RenderStats(sceneInfo, Driver);
}

} // namespace GODZ
