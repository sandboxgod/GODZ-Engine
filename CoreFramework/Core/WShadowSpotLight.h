#pragma once

#include "WSpotLight.h"

namespace GODZ
{
	// Spot Light
	class GODZ_API WShadowSpotLight : public WSpotLight
	{
		DeclareGeneric(WShadowSpotLight, WSpotLight)

	public:

		WShadowSpotLight();

		//projection info
		float GetFov() { return m_light.mProj.fov; }
		void SetFov(float fov) { m_light.mProj.fov = fov; }
		float GetAspectRatio() { return m_light.mProj.aspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_light.mProj.aspectRatio = aspectRatio; }

		float GetNear() { return m_light.mProj.near_plane; }
		void SetNear(float zn) { m_light.mProj.near_plane = zn; }


	protected:
		virtual float getLength();
	};
};
