/* ===========================================================
	Render State Information / FPS Counter

	Created Jan 8, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_RENDEROPTIONS_H_)
#define _RENDEROPTIONS_H_

#include "Godz.h"
#include "InputSystem.h"
#include "ShadowMapManager.h"

namespace GODZ
{
	/*
	* Stores options set by the user config. Like any other GenericSingleton<> it shouldn't contain public
	* static varables which would allow users to bypass our thread access checks.
	*/
	class GODZ_API RenderOptions : public GenericSingleton<RenderOptions>
	{
	public:
		//Load Settings from configuration file.
		RenderOptions();
		~RenderOptions();


		bool m_bManualSplitPlanes;
		bool bRenderOnlyNodeWeTouch;			//special debug mode in which we only render node we collide with
		bool bRenderShadowMaps;					//turns on shadow mapping
		bool bShowFPS;
		bool bUseMultisampling;
		bool bRenderNodeTree;					//Driver should render the tree for debugging
		bool bRenderEntityBounds;				//Driver should render actor bound for debugging
		bool bRenderCollisionTree;				//Driver should render collision tree
		bool bShowCameraInfo;					//display camera debug
		bool bWireframeMode;
		bool bRenderBones;
		bool bUseVSync;							//Use the monitor's v-sync
		bool bWindowed;							//windowed mode
		bool bShowNormals;						//Render normal maps (shaders)
		bool m_renderShadowmaps;
		bool m_bUseDeferred;
		bool m_bDebugRenderDeferredTexture;
		bool mIsDeferredRenderingEnabled;
		bool mIsHDREnabled;
		float mHDRExposureLevel;
		bool mForceViewBoxSize;					//set for low detail computers, clip viewbox for better shadows
		int mShadowMapQuality;
		bool mDebugObjectStateEvents;

		float mousespeed;
		unsigned int m_nMajorVSVersion;			//vertex shader - set by driver!
		unsigned int m_nMajorPSVersion;			//pixel shader - set by driver

		float m_split[ShadowMapManager::MAX_NUM_SHADOW_MAPS];
		
		bool m_bClearShadowMap;					//means not to render anything into it (for debug)
		unsigned int m_nSplitToRender;			//only render this "split"
		float m_fSplitSchemeLambda;				//lamba determines our split scheme (used by PSSM)
		bool	m_bDebugFirstPassOnly;
		float m_viewerBoxSize;					//shadow viewbox size
		unsigned int m_numSplits;			//# of parallel splits
		float mLightFadeDist;					//max dist to change shadow caster light into non-shadow caster

		//max # of splits set in config file or forced by the driver at startup
		void SetMaxNumSplits(unsigned int num) { m_preferredNumSplits = num; }
		unsigned int GetMaxNumSplits() { return m_preferredNumSplits; }

		ShadowMapQuality GetShadowMapQuality() { return (ShadowMapQuality)mShadowMapQuality; }

	private:
		unsigned int m_preferredNumSplits;			//default # of shadowmaps
	};

#define GlobalOptions RenderOptions::GetInstance()
}

#endif