
#include "RenderOptions.h"
#include <CoreFramework/Math/WGeometry.h>

using namespace GODZ;

RenderOptions::RenderOptions()
: bRenderNodeTree(0)
, bRenderEntityBounds(0)
, bWireframeMode(0)
, bUseVSync(0)
, GenericSingleton<RenderOptions>(*this)
, mousespeed(1.7f)
, bRenderBones(0)
, bUseMultisampling(false)
, m_viewerBoxSize(6000)
, m_bUseDeferred(true)
, mIsDeferredRenderingEnabled(true)
, m_bDebugRenderDeferredTexture(false)
, bRenderOnlyNodeWeTouch(false)
, bRenderShadowMaps(false)
, bShowFPS(false)
, m_bClearShadowMap(false)
, m_numSplits(3)
, m_nSplitToRender(909) //should be init to a large number....
, m_fSplitSchemeLambda(0.45f)
, m_bDebugFirstPassOnly(false)
, m_bManualSplitPlanes(false)
, mForceViewBoxSize(false)
, mShadowMapQuality(0)
, mDebugObjectStateEvents(false)
, mLightFadeDist(8000)
, mHDRExposureLevel(1.3f)
{
	IGenericConfig* GConfig = GlobalConfig::m_pConfig;

	mDebugObjectStateEvents = GConfig->ReadBoolean("Debug", "DebugObjectStateEvents");

	bRenderEntityBounds = (GConfig->ReadBoolean("Driver", "RenderEntityBound"));
	bRenderNodeTree = (GConfig->ReadBoolean("Driver", "DebugNodeTree"));
	bRenderCollisionTree = (GConfig->ReadBoolean("Driver","RenderCollisionTree"));
	bShowFPS = GConfig->ReadBoolean("Driver", "ShowFPS");
	bShowCameraInfo = GConfig->ReadBoolean("Driver", "ShowCameraInfo");
	bRenderBones = GConfig->ReadBoolean("Driver", "RenderBones");
	bUseVSync = GConfig->ReadBoolean("Driver", "UseVSync");
	bWindowed = !GConfig->ReadBoolean("Application","FullScreen");

	mIsHDREnabled = GConfig->ReadBoolean("PostEffects","HDRLighting");
	
	bRenderOnlyNodeWeTouch = GConfig->ReadBoolean("Collision","RenderOnlyNodeWeTouch");
	bRenderShadowMaps = GConfig->ReadBoolean("Driver","UseShadowMaps");

	bUseMultisampling = GConfig->ReadBoolean("PostEffects","Multisampling");

	const char* hdrExposure = GlobalConfig::m_pConfig->ReadNode("PostEffects","HDRExposureLevel");
	if (hdrExposure != NULL)
	{
		mHDRExposureLevel = atof( hdrExposure );
	}

	m_bManualSplitPlanes = GConfig->ReadBoolean("Shadows","ManualSplitPlanes");
	const char* numSplits = GConfig->ReadNode("Shadows","NumSplits");
	m_renderShadowmaps = GConfig->ReadBoolean("Shadows","DisplayShadowmaps");

	const char* lightFadeDist = GConfig->ReadNode("Shadows","ShadowLightFadeDist");
	if (lightFadeDist != NULL)
	{
		mLightFadeDist = atof(lightFadeDist);
	}

	//0 = Low, 1 = Med, 2 = High
	const char* shadowMapQuality = GlobalConfig::m_pConfig->ReadNode("Shadows","ShadowMapQuality");
	if (shadowMapQuality != NULL)
	{
		mShadowMapQuality = atoi(shadowMapQuality);
	}

	if (numSplits!=NULL)
	{
		m_numSplits = atoi(numSplits);
		m_preferredNumSplits = m_numSplits;
	}

	if (m_bManualSplitPlanes)
	{
		godzassert(m_numSplits < ShadowMapManager::MAX_NUM_SHADOW_MAPS); //need to increase MAX_SPLITS if exceeded
		for(unsigned int i=0; i < m_numSplits; i++)
		{
			rstring nodename = "SplitRange";
			nodename += GetString("%d", i);

			const char* text = GConfig->ReadNode("Shadows",nodename);
			if (text != NULL)
			{
				m_split[i] = atof(text);
			}
			else
			{
				Log("ManualSplitPlanes error: Invalid split encountered\n");
				m_bManualSplitPlanes=false;
			}
		}
	}

	const char* lamdaStr = GlobalConfig::m_pConfig->ReadNode("Shadows","Lamda");
	if (lamdaStr)
	{
		m_fSplitSchemeLambda = Clamp(static_cast<float>(atof(lamdaStr)), 0.0f, 1.0f);
	}

	const char* text = GConfig->ReadNode("Shadows","ViewBoxSize");
	if (text != NULL)
	{
		m_viewerBoxSize = atof(text);
	}

	m_bDebugFirstPassOnly = GlobalConfig::m_pConfig->ReadBoolean("Shadows", "DebugFirstPassOnly");

	bShowNormals = false;
	m_nMajorVSVersion = 0;
	m_nMajorPSVersion = 0;
}

RenderOptions::~RenderOptions()
{

}


