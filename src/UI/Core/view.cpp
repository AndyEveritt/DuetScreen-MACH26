#include "view.h"
#include "Debug.h"

namespace UI
{
	static lv_coord_t getPct(lv_coord_t value, lv_coord_t base)
	{
		return (value * 100 + base / 2) / base;
	}

	BaseView::BaseView(const std::string& name, lv_obj_t* parent)
		: m_cont(lv_obj_create(parent))
		, m_name(name)
	{
		verbose("Creating view '%s' (%p)", getName(), m_cont);
		lv_obj_set_style_pad_all(getCont(), 5, 0);
	}

	BaseView::~BaseView()
	{
		verbose("Deleting view '%s' (%p)", getName(), m_cont);
		lv_obj_delete(getCont());
	}

	lv_obj_t* BaseView::getScreen() const
	{
		return lv_obj_get_screen(getCont());
	}

	lv_obj_t* BaseView::getParent() const
	{
		return lv_obj_get_parent(getCont());
	}

	lv_obj_t* BaseView::getChild(int32_t id) const
	{
		return lv_obj_get_child(getCont(), id);
	}

	uint32_t BaseView::getChildCnt() const
	{
		return lv_obj_get_child_count(getCont());
	}

	layout_t BaseView::getLayout()
	{
		layout_t layout;
		lv_coord_t x = lv_obj_get_x(getCont());
		lv_coord_t y = lv_obj_get_y(getCont());
		lv_coord_t w = lv_obj_get_width(getCont());
		lv_coord_t h = lv_obj_get_height(getCont());
		lv_coord_t wParent = lv_obj_get_width(lv_obj_get_parent(getCont()));
		lv_coord_t hParent = lv_obj_get_height(lv_obj_get_parent(getCont()));

		layout.x = getPct(x, wParent);
		layout.y = getPct(y, hParent);
		layout.w = getPct(w, wParent);
		layout.h = getPct(h, hParent);

		return layout;
	}

	void BaseView::setLayout(layout_t layout)
	{
		lv_obj_set_pos(getCont(), lv_pct(layout.x), lv_pct(layout.y));
		lv_obj_set_size(getCont(), lv_pct(layout.w), lv_pct(layout.h));
	}

	void BaseView::setWidth(int widthPct)
	{
		lv_obj_set_width(getCont(), lv_pct(widthPct));
	}

	void BaseView::setHeight(int heightPct)
	{
		lv_obj_set_height(getCont(), lv_pct(heightPct));
	}

	void BaseView::setX(int xPct)
	{
		lv_obj_set_x(getCont(), lv_pct(xPct));
	}

	void BaseView::setY(int yPct)
	{
		lv_obj_set_y(getCont(), lv_pct(yPct));
	}

	void BaseView::setStyle(lv_style_t* style, lv_style_selector_t selector)
	{
		lv_obj_add_style(getCont(), style, selector);
	}
} // namespace UI
