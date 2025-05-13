#include "View.h"
#include "Debug.h"

namespace UI
{
	static lv_coord_t getPct(lv_coord_t value, lv_coord_t base)
	{
		return (value * 100 + base / 2) / base;
	}

	BaseView::BaseView(const std::string& name, lv_obj_t* parent)
		: m_name(name)
	{
		UI_LOCK();
		m_cont = lv_obj_create(parent);

		LOG_VERBOSE("Creating view '{:s}' ({})", getName(), static_cast<const void*>(m_cont));
		lv_obj_set_style_pad_all(getCont(), 5, 0);
		lv_obj_null_on_delete(&m_cont);
	}

	BaseView::BaseView(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent)
	{
		UI_LOCK();
		lv_obj_set_pos(getCont(), lv_pct(layout.x), lv_pct(layout.y));
		lv_obj_set_width(getCont(), layout.w == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : lv_pct(layout.w));
		lv_obj_set_height(getCont(), layout.h == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : lv_pct(layout.h));
	}

	BaseView::~BaseView()
	{
		UI_LOCK();
		LOG_VERBOSE("Deleting view '{:s}' ({})", getName(), static_cast<const void*>(m_cont));
		lv_obj_delete(getCont());
	}

	lv_obj_t* BaseView::getScreen() const
	{
		UI_LOCK();
		return lv_obj_get_screen(getCont());
	}

	lv_obj_t* BaseView::getParent() const
	{
		UI_LOCK();
		return lv_obj_get_parent(getCont());
	}

	lv_obj_t* BaseView::getChild(int32_t id) const
	{
		UI_LOCK();
		return lv_obj_get_child(getCont(), id);
	}

	uint32_t BaseView::getChildCnt() const
	{
		UI_LOCK();
		return lv_obj_get_child_count(getCont());
	}

	layout_t BaseView::getLayout()
	{
		UI_LOCK();
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

	BaseView* BaseView::setLayoutStyle(lv_layout_t style)
	{
		UI_LOCK();
		lv_obj_set_layout(getCont(), style);
		return this;
	}

	BaseView* BaseView::setFlexGrow(uint8_t grow)
	{
		UI_LOCK();
		lv_obj_set_flex_grow(getCont(), grow);
		return this;
	}

	BaseView* BaseView::setFlexFlow(lv_flex_flow_t flow)
	{
		UI_LOCK();
		lv_obj_set_flex_flow(getCont(), flow);
		return this;
	}

	BaseView* BaseView::setFlexAlign(lv_flex_align_t main, lv_flex_align_t cross, lv_flex_align_t mid)
	{
		UI_LOCK();
		lv_obj_set_flex_align(getCont(), main, cross, mid);
		return this;
	}

	BaseView* BaseView::setGridDsc(const int32_t col_dsc[], const int32_t row_dsc[])
	{
		UI_LOCK();
		lv_obj_set_grid_dsc_array(getCont(), col_dsc, row_dsc);
		return this;
	}

	BaseView* BaseView::setGridCell(lv_obj_t* obj,
									lv_grid_align_t x_align,
									int32_t col_pos,
									int32_t col_span,
									lv_grid_align_t y_align,
									int32_t row_pos,
									int32_t row_span)
	{
		UI_LOCK();
		lv_obj_set_grid_cell(obj, x_align, col_pos, col_span, y_align, row_pos, row_span);
		return this;
	}

	BaseView* BaseView::setLayout(layout_t layout)
	{
		UI_LOCK();
		lv_obj_set_pos(getCont(), lv_pct(layout.x), lv_pct(layout.y));
		lv_obj_set_size(getCont(), lv_pct(layout.w), lv_pct(layout.h));
		return this;
	}

	BaseView* BaseView::setWidth(lv_coord_t width)
	{
		UI_LOCK();
		lv_obj_set_width(getCont(), lv_pct(width));
		return this;
	}

	BaseView* BaseView::setHeight(lv_coord_t height)
	{
		UI_LOCK();
		lv_obj_set_height(getCont(), height);
		return this;
	}

	BaseView* BaseView::setSize(lv_coord_t width, lv_coord_t height)
	{
		UI_LOCK();
		lv_obj_set_size(getCont(), width, height);
		return this;
	}

	BaseView* BaseView::setMinWidth(lv_coord_t width, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_min_width(getCont(), width, selector);
		return this;
	}

	BaseView* BaseView::setMinHeight(lv_coord_t height, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_min_height(getCont(), height, selector);
		return this;
	}

	BaseView* BaseView::setX(lv_coord_t x)
	{
		UI_LOCK();
		lv_obj_set_x(getCont(), x);
		return this;
	}

	BaseView* BaseView::setY(lv_coord_t y)
	{
		UI_LOCK();
		lv_obj_set_y(getCont(), y);
		return this;
	}

	BaseView* BaseView::setPos(lv_coord_t x, lv_coord_t y)
	{
		UI_LOCK();
		lv_obj_set_pos(getCont(), x, y);
		return this;
	}

	BaseView* BaseView::setFlag(lv_obj_flag_t flag, bool enable)
	{
		UI_LOCK();
		lv_obj_set_flag(getCont(), flag, enable);
		return this;
	}

	BaseView* BaseView::setAlign(lv_align_t align, lv_coord_t x, lv_coord_t y)
	{
		UI_LOCK();
		lv_obj_align(getCont(), align, x, y);
		return this;
	}

	BaseView* BaseView::setPad(lv_coord_t pad, lv_style_selector_t selector, Padding type)
	{
		UI_LOCK();
		switch (type)
		{
		case Padding::ALL:
			lv_obj_set_style_pad_all(m_cont, pad, selector);
			break;
		case Padding::LEFT:
			lv_obj_set_style_pad_left(m_cont, pad, selector);
			break;
		case Padding::RIGHT:
			lv_obj_set_style_pad_right(m_cont, pad, selector);
			break;
		case Padding::TOP:
			lv_obj_set_style_pad_top(m_cont, pad, selector);
			break;
		case Padding::BOTTOM:
			lv_obj_set_style_pad_bottom(m_cont, pad, selector);
			break;
		case Padding::COLUMN:
			lv_obj_set_style_pad_column(m_cont, pad, selector);
			break;
		case Padding::ROW:
			lv_obj_set_style_pad_row(m_cont, pad, selector);
			break;
		case Padding::HORIZONTAL:
			lv_obj_set_style_pad_hor(m_cont, pad, selector);
			break;
		case Padding::VERTICAL:
			lv_obj_set_style_pad_ver(m_cont, pad, selector);
			break;
		default:
			LOG_WARN("Unknown padding type");
			break;
		}
		return this;
	}

	BaseView* BaseView::addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData)
	{
		UI_LOCK();
		lv_obj_add_event_cb(getCont(), cb, code, userData);
		return this;
	}

	BaseView* BaseView::addStyle(lv_style_t* style, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_add_style(getCont(), style, selector);
		return this;
	}

	/**
	 * @brief Shows the view.
	 *
	 * @note This function calls the `onShow()` virtual method before showing the view.
	 */
	void BaseView::show()
	{
		UI_LOCK();
		if (getCont() == nullptr)
		{
			return;
		}
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
		UI_LOCK();
		if (getCont() == nullptr)
		{
			return;
		}
		lv_obj_move_background(getCont());
		lv_obj_add_flag(getCont(), LV_OBJ_FLAG_HIDDEN);
		onHide();
	}

	bool BaseView::isVisible()
	{
		UI_LOCK();
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
