#include "component.h"
#include "Debug.h"

namespace UI
{
	Component::Component(const char* name, lv_obj_t* parent)
		: m_name(name)
		, m_obj(lv_obj_create(parent))
	{
		lv_obj_set_style_pad_all(m_obj, 5, 0);
	}

	Component::~Component()
	{
		verbose("Deleting component '%s' (%p)", m_name, m_obj);
		lv_obj_delete(m_obj);
	}

	lv_obj_t* Component::getScreen() const
	{
		return lv_obj_get_screen(m_obj);
	}

	lv_obj_t* Component::getParent() const
	{
		return lv_obj_get_parent(m_obj);
	}

	lv_obj_t* Component::getChild(int32_t id) const
	{
		return lv_obj_get_child(m_obj, id);
	}

	uint32_t Component::getChildCnt() const
	{
		return lv_obj_get_child_count(m_obj);
	}

} // namespace UI
