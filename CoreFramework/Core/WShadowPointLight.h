#pragma once

#include "WEntity.h"

namespace GODZ
{
	class WShadowSpotLight;

	// Omni-directional shadow caster light source
	class GODZ_API WShadowPointLight : public WEntity
	{
		DeclareGeneric(WShadowPointLight, WEntity)

	public:
		WShadowPointLight();

		virtual void OnAddToWorld();
		virtual void OnRemoved();

		void SetFar(float far);
		float GetFar();

		void SetLightRange(float r);
		float GetLightRange();

		const Color4f&				GetColor() const;
		virtual void	SetColor(const Color4f& value);

	protected:
		virtual void			UpdateGeometryInfo(GeometryInfo& info);

		atomic_weak_ptr<WShadowSpotLight> mLights[6];
		float mLightRange;
		float mFar;
		Color4f mColor;
	};
};

