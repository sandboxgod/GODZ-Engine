
#if !defined(_WPOINTLIGHT_H_)
#define _WPOINTLIGHT_H_

#include "WEntity.h"
#include "SceneLight.h"

namespace GODZ
{
	// Point Light
	// Note: LightRange is indicated by the *uniform* scale of the light. Non-uniform scale is illegal...
	class GODZ_API WPointLight : public WEntity
	{
		DeclareGeneric(WPointLight, WEntity)

	public:
		WPointLight();

		const Color4f&			GetColor() const;
		void					SetColor(const Color4f& value);		//normalized light color 0 to +1

		//Notification this entity has been spawned on the level
		virtual void OnAddToWorld();
		virtual void OnRemoved();
		virtual void OnTick(float dt);

	protected:
		virtual void			UpdateGeometryInfo(GeometryInfo& info);

		SceneLight					m_light;
	};
};

#endif //_WPOINTLIGHT_H_