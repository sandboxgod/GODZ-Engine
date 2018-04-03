/* ===========================================================
	WDynamicLight.h

	Provides dynamic lighting capabilities. Derives from a
	camera because we want to have the ability to view the scene
	from the point-of-view of the light

	Created Nov 21, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WDYNAMICLIGHT_H_)
#define _WDYNAMICLIGHT_H_

#include "Godz.h"
#include "SceneLight.h"
#include "WCamera.h"

namespace GODZ
{
	// Dynamic Light
	class GODZ_API WDynamicLight : public WCamera
	{
		DeclareGeneric(WDynamicLight, WCamera)

	public:
		WDynamicLight();
		virtual ~WDynamicLight();

		LightID					GetLightID() { return m_light.m_id; }

		//Notification this entity has been spawned on the level
		virtual void			OnAddToWorld();

		//Notification this entity has been removed from the level
		virtual void			OnRemoved();

		const Color4f&			GetColor() const;
		void					SetColor(const Color4f& value);		//normalized light color 0 to +1

		void					GetViewMatrixInfo(ViewMatrixInfo& vmi);

		virtual void			OnTick(float dt);

		LightType				GetLightType();

		virtual ECameraType		GetCameraType() { return CameraType_LightSource; }

		//Returns true if this entity should display an icon in the Editor
		virtual bool			IsIconVisible() { return true; }

	protected:
		virtual void			UpdateGeometryInfo(GeometryInfo& info);

		SceneLight					m_light;
	};
};

#endif //_WDYNAMICLIGHT_H_