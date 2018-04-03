
#if !defined(_WSPOTLIGHT_H_)
#define _WSPOTLIGHT_H_

#include "WDynamicLight.h"
#include "atomic_ref.h"

namespace GODZ
{
	// Spot Light (just like in 3dsmax). Aims at a Target
	class GODZ_API WSpotLight : public WDynamicLight
	{
		DeclareGeneric(WSpotLight, WDynamicLight)

	public:

		WSpotLight();

		//camera interface
		virtual Vector3				GetTarget();
		virtual Vector3				GetEye();

		virtual void			OnAddToWorld();
		virtual void			OnTick(float deltaTime);

		void SetLightRange(float r) { m_light.m_lightRange = r; }
		float GetLightRange() { return m_light.m_lightRange; }

		float GetFar() { return m_light.mProj.far_plane; }
		void SetFar(float zf) { m_light.mProj.far_plane = zf; }

		float GetConeRadius() { return mConeRadius; }
		void SetConeRadius(float radius) { mConeRadius = radius; }

		void SetZAxisFlip(bool invert) { mZAxisFlip = invert; }
		bool GetZAxisFlip() { return mZAxisFlip; }

	protected:
		virtual float getLength();
		virtual void			UpdateWorldScaleOffset();

	private:
		GenericReference<ModelResource> mModelResource;		//cylinder model
		atomic_ptr<Material> mMaterial;		//note: this could probably be shared amongst all spotlights
		float mConeRadius;

		//hack to workaround restriction that prevents me from rotating a spotlight along z :(
		bool mZAxisFlip;

	};
};

#endif //WSpotLight