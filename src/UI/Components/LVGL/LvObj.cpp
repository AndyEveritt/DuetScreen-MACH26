/*
 * LvObj.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "LvObj.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	static lv_coord_t getPct(lv_coord_t value, lv_coord_t base)
	{
		return (value * 100 + base / 2) / base;
	}

	LvObj::LvObj(lv_create_t initFunc, const std::string& name, lv_obj_t* parent)
		: m_name(name)
	{
		UI_LOCK();
		m_root = initFunc(parent);

		LOG_VERBOSE("Creating view '{:s}' ({})", getName(), static_cast<const void*>(m_root));
		lv_obj_null_on_delete(&m_root);
	}

	LvObj::LvObj(lv_create_t initFunc, const std::string& name, lv_obj_t* parent, layout_t layout)
		: LvObj(initFunc, name, parent)
	{
		UI_LOCK();
		lv_obj_set_pos(getRoot(), lv_pct(layout.x), lv_pct(layout.y));
		lv_obj_set_width(getRoot(), layout.w == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : lv_pct(layout.w));
		lv_obj_set_height(getRoot(), layout.h == LV_SIZE_CONTENT ? LV_SIZE_CONTENT : lv_pct(layout.h));
	}

	LvObj::~LvObj()
	{
		UI_LOCK();
		LOG_VERBOSE("Deleting view '{:s}' ({})", getName(), static_cast<const void*>(m_root));
		lv_obj_delete(getRoot());
	}

	lv_obj_t* LvObj::getScreen() const
	{
		UI_LOCK();
		return lv_obj_get_screen(getRoot());
	}

	lv_obj_t* LvObj::getParent() const
	{
		UI_LOCK();
		return lv_obj_get_parent(getRoot());
	}

	lv_obj_t* LvObj::getChild(int32_t id) const
	{
		UI_LOCK();
		return lv_obj_get_child(getRoot(), id);
	}

	uint32_t LvObj::getChildCnt() const
	{
		UI_LOCK();
		return lv_obj_get_child_count(getRoot());
	}

	layout_t LvObj::getLayout() const
	{
		UI_LOCK();
		layout_t layout;
		layout.x = lv_obj_get_x(getRoot());
		layout.y = lv_obj_get_y(getRoot());
		layout.w = lv_obj_get_width(getRoot());
		layout.h = lv_obj_get_height(getRoot());

		return layout;
	}

	layout_t LvObj::getLayoutPct() const
	{
		UI_LOCK();
		layout_t layout = getLayout();
		lv_coord_t wParent = lv_obj_get_width(getParent());
		lv_coord_t hParent = lv_obj_get_height(getParent());

		layout.x = getPct(layout.x, wParent);
		layout.y = getPct(layout.y, hParent);
		layout.w = getPct(layout.w, wParent);
		layout.h = getPct(layout.h, hParent);

		return layout;
	}

	lv_area_t LvObj::getCoords() const
	{
		UI_LOCK();
		lv_area_t area;
		lv_obj_get_coords(getRoot(), &area);
		return area;
	}

	lv_coord_t LvObj::getX() const
	{
		UI_LOCK();
		return lv_obj_get_x(getRoot());
	}

	lv_coord_t LvObj::getY() const
	{
		UI_LOCK();
		return lv_obj_get_y(getRoot());
	}

	lv_coord_t LvObj::getX2() const
	{
		UI_LOCK();
		return lv_obj_get_x2(getRoot());
	}

	lv_coord_t LvObj::getY2() const
	{
		UI_LOCK();
		return lv_obj_get_y2(getRoot());
	}

	lv_coord_t LvObj::getWidth() const
	{
		UI_LOCK();
		return lv_obj_get_width(getRoot());
	}

	lv_coord_t LvObj::getHeight() const
	{
		UI_LOCK();
		return lv_obj_get_height(getRoot());
	}

	lv_coord_t LvObj::getContentWidth() const
	{
		UI_LOCK();
		return lv_obj_get_content_width(getRoot());
	}

	lv_coord_t LvObj::getContentHeight() const
	{
		UI_LOCK();
		return lv_obj_get_content_height(getRoot());
	}

	lv_coord_t LvObj::getSelfWidth() const
	{
		UI_LOCK();
		return lv_obj_get_self_width(getRoot());
	}

	lv_coord_t LvObj::getSelfHeight() const
	{
		UI_LOCK();
		return lv_obj_get_self_height(getRoot());
	}

	void LvObj::setUserData(void* user_data)
	{
		UI_LOCK();
		lv_obj_set_user_data(getRoot(), user_data);
	}

	void* LvObj::getUserData() const
	{
		UI_LOCK();
		return lv_obj_get_user_data(getRoot());
	}

	void LvObj::setLayoutStyle(lv_layout_t style)
	{
		UI_LOCK();
		lv_obj_set_layout(getRoot(), style);
	}

	void LvObj::setFlexGrow(uint8_t grow)
	{
		UI_LOCK();
		lv_obj_set_flex_grow(getRoot(), grow);
	}

	void LvObj::setFlexFlow(lv_flex_flow_t flow)
	{
		UI_LOCK();
		lv_obj_set_flex_flow(getRoot(), flow);
	}

	void LvObj::setFlexAlign(lv_flex_align_t main, lv_flex_align_t cross, lv_flex_align_t mid)
	{
		UI_LOCK();
		lv_obj_set_flex_align(getRoot(), main, cross, mid);
	}

	void LvObj::setGridDsc(const int32_t col_dsc[], const int32_t row_dsc[])
	{
		UI_LOCK();
		lv_obj_set_grid_dsc_array(getRoot(), col_dsc, row_dsc);
	}

	void LvObj::setGridCell(lv_obj_t* obj,
							lv_grid_align_t x_align,
							int32_t col_pos,
							int32_t col_span,
							lv_grid_align_t y_align,
							int32_t row_pos,
							int32_t row_span)
	{
		UI_LOCK();
		lv_obj_set_grid_cell(obj, x_align, col_pos, col_span, y_align, row_pos, row_span);
	}

	void LvObj::setLayout(layout_t layout)
	{
		UI_LOCK();
		lv_obj_set_pos(getRoot(), lv_pct(layout.x), lv_pct(layout.y));
		lv_obj_set_size(getRoot(), lv_pct(layout.w), lv_pct(layout.h));
	}

	void LvObj::setWidth(lv_coord_t width)
	{
		UI_LOCK();
		lv_obj_set_width(getRoot(), width);
	}

	void LvObj::setHeight(lv_coord_t height)
	{
		UI_LOCK();
		lv_obj_set_height(getRoot(), height);
	}

	void LvObj::setSize(lv_coord_t width, lv_coord_t height)
	{
		UI_LOCK();
		lv_obj_set_size(getRoot(), width, height);
	}

	void LvObj::setMinWidth(lv_coord_t width, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_min_width(getRoot(), width, selector);
	}

	void LvObj::setMinHeight(lv_coord_t height, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_min_height(getRoot(), height, selector);
	}

	void LvObj::setMaxWidth(lv_coord_t width, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_max_width(getRoot(), width, selector);
	}

	void LvObj::setMaxHeight(lv_coord_t height, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_max_height(getRoot(), height, selector);
	}

	void LvObj::setX(lv_coord_t x)
	{
		UI_LOCK();
		lv_obj_set_x(getRoot(), x);
	}

	void LvObj::setY(lv_coord_t y)
	{
		UI_LOCK();
		lv_obj_set_y(getRoot(), y);
	}

	void LvObj::setPos(lv_coord_t x, lv_coord_t y)
	{
		UI_LOCK();
		lv_obj_set_pos(getRoot(), x, y);
	}

	static void lv_obj_set_flag(lv_obj_t* obj, lv_obj_flag_t flag, bool enable, bool recursive)
	{
		lv_obj_set_flag(obj, flag, enable);
		if (recursive)
		{
			for (size_t i = 0; i < lv_obj_get_child_count(obj); i++)
			{
				lv_obj_t* child = lv_obj_get_child(obj, i);
				if (child == nullptr)
				{
					continue;
				}
				lv_obj_set_flag(child, flag, enable, true);
			}
		}
	}

	void LvObj::setFlag(lv_obj_flag_t flag, bool enable, bool recursive)
	{
		UI_LOCK();
		lv_obj_set_flag(getRoot(), flag, enable, recursive);
	}

	bool LvObj::hasFlag(lv_obj_flag_t flag) const
	{
		UI_LOCK();
		return lv_obj_has_flag(getRoot(), flag);
	}

	static void lv_obj_set_state(lv_obj_t* obj, lv_state_t state, bool enable, bool recursive)
	{
		lv_obj_set_state(obj, state, enable);
		if (recursive)
		{
			for (size_t i = 0; i < lv_obj_get_child_count(obj); i++)
			{
				lv_obj_t* child = lv_obj_get_child(obj, i);
				if (child == nullptr)
				{
					continue;
				}
				lv_obj_set_state(child, state, enable, true);
			}
		}
	}

	void LvObj::setState(lv_state_t state, bool enable, bool recursive)
	{
		UI_LOCK();
		lv_obj_set_state(getRoot(), state, enable, recursive);
	}

	bool LvObj::hasState(lv_state_t state) const
	{
		UI_LOCK();
		return lv_obj_has_state(getRoot(), state);
	}

	void LvObj::setAlign(lv_align_t align, lv_coord_t x, lv_coord_t y)
	{
		UI_LOCK();
		lv_obj_align(getRoot(), align, x, y);
	}

	void LvObj::updateLayout()
	{
		UI_LOCK();
		lv_obj_update_layout(getRoot());
	}

	bool LvObj::refreshSelfSize()
	{
		UI_LOCK();
		return lv_obj_refresh_self_size(getRoot());
	}

	void LvObj::invalidate()
	{
		UI_LOCK();
		lv_obj_invalidate(getRoot());
	}

	void LvObj::setExtClickArea(int32_t size)
	{
		UI_LOCK();
		lv_obj_set_ext_click_area(getRoot(), size);
	}

	void LvObj::getClickArea(lv_area_t* area) const
	{
		UI_LOCK();
		lv_obj_get_click_area(getRoot(), area);
	}

	void LvObj::scrollToX(lv_coord_t x, lv_anim_enable_t anim)
	{
		UI_LOCK();
		lv_obj_scroll_to_x(getRoot(), x, anim);
	}

	void LvObj::scrollToY(lv_coord_t y, lv_anim_enable_t anim)
	{
		UI_LOCK();
		lv_obj_scroll_to_y(getRoot(), y, anim);
	}

	void LvObj::addStyle(const lv_style_t* style, const lv_style_selector_t selector, bool recursive)
	{
		UI_LOCK();
		lv_obj_add_style(getRoot(), style, selector, recursive);
	}

	void LvObj::setStylePad(lv_coord_t pad, lv_style_selector_t selector, Padding type)
	{
		UI_LOCK();
		switch (type)
		{
		case Padding::ALL:
			lv_obj_set_style_pad_all(m_root, pad, selector);
			break;
		case Padding::LEFT:
			lv_obj_set_style_pad_left(m_root, pad, selector);
			break;
		case Padding::RIGHT:
			lv_obj_set_style_pad_right(m_root, pad, selector);
			break;
		case Padding::TOP:
			lv_obj_set_style_pad_top(m_root, pad, selector);
			break;
		case Padding::BOTTOM:
			lv_obj_set_style_pad_bottom(m_root, pad, selector);
			break;
		case Padding::COLUMN:
			lv_obj_set_style_pad_column(m_root, pad, selector);
			break;
		case Padding::ROW:
			lv_obj_set_style_pad_row(m_root, pad, selector);
			break;
		case Padding::HORIZONTAL:
			lv_obj_set_style_pad_hor(m_root, pad, selector);
			break;
		case Padding::VERTICAL:
			lv_obj_set_style_pad_ver(m_root, pad, selector);
			break;
		default:
			LOG_WARN("Unknown padding type");
			break;
		}
	}

	void LvObj::setStyleBgColor(lv_color_t color, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_bg_color(getRoot(), color, selector);
	}

	void LvObj::setStyleTextAlign(lv_text_align_t align, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_text_align(getRoot(), align, selector);
	}

	lv_event_dsc_t* LvObj::addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData)
	{
		UI_LOCK();
		return lv_obj_add_event_cb(getRoot(), cb, code, userData);
	}

	bool LvObj::removeEvent(size_t index)
	{
		UI_LOCK();
		return lv_obj_remove_event(getRoot(), index);
	}

	uint32_t LvObj::removeEventCallback(lv_event_cb_t cb)
	{
		UI_LOCK();
		return lv_obj_remove_event_cb(getRoot(), cb);
	}

	uint32_t LvObj::removeEventCallbackWithUserData(lv_event_cb_t cb, void* userData)
	{
		UI_LOCK();
		return lv_obj_remove_event_cb_with_user_data(getRoot(), cb, userData);
	}

	uint32_t LvObj::getEventCount()
	{
		UI_LOCK();
		return lv_obj_get_event_count(getRoot());
	}

	lv_result_t LvObj::sendEvent(lv_event_code_t code, void* param)
	{
		UI_LOCK();
		return lv_obj_send_event(getRoot(), code, param);
	}

	/**
	 * @brief Shows the view.
	 *
	 * @note This function calls the `onShow()` virtual method before showing the view.
	 */
	void LvObj::show()
	{
		UI_LOCK();
		if (getRoot() == nullptr)
		{
			return;
		}
		if (!lv_obj_has_flag(getRoot(), LV_OBJ_FLAG_HIDDEN))
		{
			LOG_VERBOSE("'{:s}' is already visible", getName());
			return;
		}

		LOG_DBG("Showing '{:s}'", getName());
		lv_obj_move_foreground(getRoot());
		lv_obj_remove_flag(getRoot(), LV_OBJ_FLAG_HIDDEN);
		onShow();
	}

	/**
	 * @brief Hides the view.
	 *
	 * @note This function calls the `onHide()` virtual method before hiding the view.
	 */
	void LvObj::hide()
	{
		UI_LOCK();
		if (getRoot() == nullptr)
		{
			return;
		}
		if (lv_obj_has_flag(getRoot(), LV_OBJ_FLAG_HIDDEN))
		{
			LOG_VERBOSE("'{:s}' is already hidden", getName());
			return;
		}

		LOG_DBG("Hiding '{:s}'", getName());
		lv_obj_move_background(getRoot());
		lv_obj_add_flag(getRoot(), LV_OBJ_FLAG_HIDDEN);
		onHide();
	}

	bool LvObj::isVisible()
	{
		UI_LOCK();
		return !lv_obj_has_flag(getRoot(), LV_OBJ_FLAG_HIDDEN);
	}

	/**
	 * @brief Handle a back button event
	 * @return true if the view handled the back event
	 */
	bool LvObj::back()
	{
		return false;
	}
} // namespace UI
