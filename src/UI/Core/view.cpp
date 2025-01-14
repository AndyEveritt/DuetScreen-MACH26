#include "view.h"
#include "Debug.h"

namespace UI
{
	static lv_coord_t getPct(lv_coord_t value, lv_coord_t base)
	{
		return (value * 100 + base / 2) / base;
	}

	BaseView::~BaseView()
	{
		verbose("Deleting view '%s' (%p)", m_name, m_cont);
		lv_obj_delete(m_cont);
	}

	lv_obj_t* BaseView::getScreen() const
	{
		return lv_obj_get_screen(m_cont);
	}

	lv_obj_t* BaseView::getParent() const
	{
		return lv_obj_get_parent(m_cont);
	}

	lv_obj_t* BaseView::getChild(int32_t id) const
	{
		return lv_obj_get_child(m_cont, id);
	}

	uint32_t BaseView::getChildCnt() const
	{
		return lv_obj_get_child_count(m_cont);
	}

	layout_t BaseView::getLayout()
	{
		layout_t layout;
		lv_coord_t x = lv_obj_get_x(m_cont);
		lv_coord_t y = lv_obj_get_y(m_cont);
		lv_coord_t w = lv_obj_get_width(m_cont);
		lv_coord_t h = lv_obj_get_height(m_cont);
		lv_coord_t wParent = lv_obj_get_width(lv_obj_get_parent(m_cont));
		lv_coord_t hParent = lv_obj_get_height(lv_obj_get_parent(m_cont));

		layout.x = getPct(x, wParent);
		layout.y = getPct(y, hParent);
		layout.w = getPct(w, wParent);
		layout.h = getPct(h, hParent);

		return layout;
	}

	void BaseView::setStyle(lv_style_t* style, lv_style_selector_t selector)
	{
		lv_obj_add_style(m_cont, style, selector);
	}
} // namespace UI
