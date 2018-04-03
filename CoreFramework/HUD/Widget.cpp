
#include <CoreFramework/HUD/Widget.h>
#include <CoreFramework/Core/Material.h>
#include <CoreFramework/Core/TextureResource.h>
#include <CoreFramework/Core/SceneData.h>



namespace GODZ
{



Widget::Widget(WidgetId id)
: m_pMaterial(NULL)
, m_hasBeenUpdated(true)
{
	m_data.m_id = id;
	m_data.m_color = Color4f(1,1,1,1);
	m_data.m_pos = Vector3(0.0f, 0.0f,0.0f);
	m_data.m_scale = Vector2(1.0f, 1.0f);

	WidgetCreationEvent* newEvent = new WidgetCreationEvent();
	newEvent->m_id = m_data.m_id;
	newEvent->m_color = m_data.m_color;
	newEvent->m_pos = m_data.m_pos;
	newEvent->m_scale = m_data.m_scale;

	SceneData::AddObjectStateChangeEvent(newEvent);

	m_hasBeenUpdated = false;
}

Widget::~Widget()
{
}

const Vector3& Widget::GetPosition()
{
	return m_data.m_pos;
}

//Set z to 0 for a 2D Sprite
void Widget::SetPosition(const Vector3& pos)
{
	m_data.m_pos = pos;
	m_hasBeenUpdated = true;
}

Vector2 Widget::GetScale()
{
	return m_data.m_scale;
}

Material* Widget::GetMaterial()
{
	return m_pMaterial;
}

Color4f Widget::GetColor()
{
	return m_data.m_color;
}

void Widget::SetMaterial(Material* pMat)
{
	m_pMaterial = pMat;
	m_data.m_matInfo = pMat->GetRuntimeID();

	//widget material id
	WidgetMaterialChangeEvent* ev = new WidgetMaterialChangeEvent(m_data.m_id);
	ev->m_matid = pMat->GetRuntimeID();
	SceneData::AddObjectStateChangeEvent(ev);
}

void Widget::SetColor(const Color4f& color)
{
	m_data.m_color = color;
	m_hasBeenUpdated = true;
}

void Widget::SetScale(float x, float y)
{
	m_data.m_scale.x=x;
	m_data.m_scale.y=y;
	m_hasBeenUpdated = true;
}


void Widget::Update(float dt)
{
	//we let all the changes get buffered then send them all off at once....
	if (m_hasBeenUpdated)
	{
		WidgetUpdateEvent* newEvent = new WidgetUpdateEvent();
		newEvent->m_id = m_data.m_id;
		newEvent->m_color = m_data.m_color;
		newEvent->m_pos = m_data.m_pos;
		newEvent->m_scale = m_data.m_scale;

		SceneData::AddObjectStateChangeEvent(newEvent);

		m_hasBeenUpdated = false;
	}
}

void Widget::Release()
{
	//the driver needs to remove this widget....
	WidgetDestructionEvent* ev = new WidgetDestructionEvent(m_data.m_id);
	SceneData::AddObjectStateChangeEvent(ev);
}

} //namespace GODZ
