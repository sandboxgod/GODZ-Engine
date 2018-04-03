/* ===========================================================
	SpriteWidget

	This is a GUI element used on HUDs to render images

	Created Dec 11, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/


#if !defined(__WIDGET_H__)
#define __WIDGET_H__

#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/ResourceObject.h>
#include <CoreFramework/Core/Color4f.h>
#include <CoreFramework/Core/MaterialInfo.h>
#include <CoreFramework/Math/Vector2.h>
#include "WidgetInfo.h"


namespace GODZ
{
	class Material;

	/**
	*
	*/
	class GODZ_API Widget
	{
	public:
		Widget(WidgetId id);
		~Widget();

		const Vector3&				GetPosition();
		Vector2						GetScale();
		Color4f						GetColor();
		Material*					GetMaterial();
		WidgetId					GetId() { return m_data.m_id; }

		void						SetMaterial(Material*);
		void						SetScale(float x, float y);
		void						SetColor(const Color4f&);
		void						SetPosition(const Vector3& pos);

		//Update
		void						Update(float dt);
		void						Release();

		friend GDZArchive& operator<<(GDZArchive& ar, Widget& sprite) 
		{
			//serialize current material and internal data
			ar << sprite.m_data << *(GenericObject**)&sprite.m_pMaterial;
		}

	protected:
		WidgetInfo			m_data;
		Material*			m_pMaterial;		//texture
		bool				m_hasBeenUpdated;
	};

}

#endif //__SPRITEWIDGET_H__
