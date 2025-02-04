#include "View.h"
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
		Lock lock;
		verbose("Creating view '%s' (%p)", getName(), m_cont);
		lv_obj_set_style_pad_all(getCont(), 5, 0);
#if DEBUG_BORDERS
		// lv_obj_set_style_border_color(getCont(), lv_color_black(), LV_PART_MAIN);
		// lv_obj_set_style_border_width(getCont(), 2, LV_PART_MAIN);
		// lv_obj_set_style_border_opa(getCont(), LV_OPA_100, LV_PART_MAIN);
#endif
	}

	BaseView::~BaseView()
	{
		Lock lock;
		verbose("Deleting view '%s' (%p)", getName(), m_cont);
		lv_obj_delete(getCont());
	}

	lv_obj_t* BaseView::getScreen() const
	{
		Lock lock;
		return lv_obj_get_screen(getCont());
	}

	lv_obj_t* BaseView::getParent() const
	{
		Lock lock;
		return lv_obj_get_parent(getCont());
	}

	lv_obj_t* BaseView::getChild(int32_t id) const
	{
		Lock lock;
		return lv_obj_get_child(getCont(), id);
	}

	uint32_t BaseView::getChildCnt() const
	{
		Lock lock;
		return lv_obj_get_child_count(getCont());
	}

	layout_t BaseView::getLayout()
	{
		Lock lock;
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

	void BaseView::setLayoutStyle(lv_layout_t style, lv_flex_flow_t flow)
	{
		Lock lock;
		lv_obj_set_layout(getCont(), style);
		lv_obj_set_flex_flow(getCont(), flow);
	}

	void BaseView::setLayout(layout_t layout)
	{
		Lock lock;
		lv_obj_set_pos(getCont(), lv_pct(layout.x), lv_pct(layout.y));
		lv_obj_set_size(getCont(), lv_pct(layout.w), lv_pct(layout.h));
	}

	void BaseView::setWidth(int widthPct)
	{
		Lock lock;
		lv_obj_set_width(getCont(), lv_pct(widthPct));
	}

	void BaseView::setHeight(int heightPct)
	{
		Lock lock;
		lv_obj_set_height(getCont(), lv_pct(heightPct));
	}

	void BaseView::setX(int xPct)
	{
		Lock lock;
		lv_obj_set_x(getCont(), lv_pct(xPct));
	}

	void BaseView::setY(int yPct)
	{
		Lock lock;
		lv_obj_set_y(getCont(), lv_pct(yPct));
	}

	void BaseView::setStyle(lv_style_t* style, lv_style_selector_t selector)
	{
		Lock lock;
		lv_obj_add_style(getCont(), style, selector);
	}

	/**
	 * @brief Shows the view.
	 *
	 * @note This function calls the `onShow()` virtual method before showing the view.
	 */
	void BaseView::show()
	{
		Lock lock;
		lv_obj_move_foreground(getCont());
		lv_obj_remove_flag(getCont(), LV_OBJ_FLAG_HIDDEN);
		onShow();
	}

	/**
	 * @brief Hides the view.
	 *
	 * @note This function calls the `onHide()` virtual method before hiding the view.
	 */
	void BaseView::hide()
	{
		Lock lock;
		lv_obj_move_background(getCont());
		lv_obj_add_flag(getCont(), LV_OBJ_FLAG_HIDDEN);
		onHide();
	}

	bool BaseView::isVisible()
	{
		Lock lock;
		return !lv_obj_has_flag(getCont(), LV_OBJ_FLAG_HIDDEN);
	}

	/**
	 * @brief Handle a back button event
	 * @return true if the view handled the back event
	 */
	bool BaseView::back()
	{
		return false;
	}
} // namespace UI

void lv_obj_set_flag(lv_obj_t* obj, lv_obj_flag_t flag, bool enable)
{
	if (enable)
	{
		lv_obj_add_flag(obj, flag);
	}
	else
	{
		lv_obj_remove_flag(obj, flag);
	}
}
