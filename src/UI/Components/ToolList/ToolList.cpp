/*
 * ToolList.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "ToolList.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	ToolListItem::ToolListItem(ToolList& toolList, const std::string& name, lv_obj_t* parent, layout_t layout)
		: View<ToolListItemPresenter>(name, parent, layout)
		, m_toolList(toolList)
		, m_label(lv_label_create(getCont()))
		, m_icon(nullptr)
		, m_status(lv_label_create(getCont()))
		, m_currentTemp(lv_label_create(getCont()))
		, m_activeTemp(lv_label_create(getCont()))
		, m_standbyTemp(lv_label_create(getCont()))
	{
		Lock lock;
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);

		lv_obj_set_flex_grow(m_label, 4);
		lv_obj_set_flex_grow(m_status, 3);
		lv_obj_set_flex_grow(m_currentTemp, 2);
		lv_obj_set_flex_grow(m_activeTemp, 2);
		lv_obj_set_flex_grow(m_standbyTemp, 2);

		lv_obj_add_flag(m_label, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_flag(m_status, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_flag(m_activeTemp, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_flag(m_standbyTemp, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_event_cb(m_label, onNameEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_status, onStatusEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_activeTemp, onActiveStandbyEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_standbyTemp, onActiveStandbyEvent, LV_EVENT_CLICKED, this);

		// Styles
		lv_obj_set_style_bg_color(
			getCont(), lv_color_darken(lv_obj_get_style_bg_color(getCont(), LV_PART_MAIN), 20), LV_STATE_CHECKED);
	}

	uint8_t ToolListItem::getSlotIndex() const
	{
		return (uint8_t)m_presenter.getSlotIndex();
	}

	void ToolListItem::setSlotIndex(uint8_t index)
	{
		m_presenter.setSlotIndex(index);
	}

	void ToolListItem::setLabel(const char* text)
	{
		Lock lock;
		lv_label_set_text(m_label, text);
	}

	void ToolListItem::setIcon(lv_img_dsc_t* icon)
	{
		Lock lock;
		if (m_icon == nullptr)
		{
			m_icon = lv_image_create(getCont());
			lv_obj_set_flex_grow(m_icon, 1);
		}
		lv_image_set_src(m_icon, icon);
	}

	void ToolListItem::setSelected(const bool selected)
	{
		if (m_selected == selected)
		{
			return;
		}
		Lock lock;
		lv_color_t color = lv_obj_get_style_bg_color(getCont(), LV_PART_MAIN);
		if (selected)
		{
			// lv_obj_set_style_bg_color(getCont(), lv_color_darken(color, 10), LV_PART_MAIN);
			lv_obj_add_state(getCont(), LV_STATE_CHECKED);
		}
		else
		{
			// lv_obj_set_style_bg_color(getCont(), lv_color_lighten(color, 10), LV_PART_MAIN);
			lv_obj_remove_state(getCont(), LV_STATE_CHECKED);
		}
		m_selected = selected;
	}

	void ToolListItem::setStatus(const char* text)
	{
		Lock lock;
		lv_label_set_text(m_status, text);
	}

	void ToolListItem::setCurrentTemp(float value)
	{
		Lock lock;
		lv_label_set_text(m_currentTemp, utils::format("%.1f", value).c_str());
	}

	void ToolListItem::setActiveTemp(int32_t value)
	{
		Lock lock;
		lv_label_set_text(m_activeTemp, utils::format("%d", value).c_str());
	}

	void ToolListItem::setActiveTempText(const char* text)
	{
		Lock lock;
		lv_label_set_text(m_activeTemp, text);
	}

	void ToolListItem::setStandbyTemp(int32_t value)
	{
		Lock lock;
		lv_label_set_text(m_standbyTemp, utils::format("%d", value).c_str());
	}

	void ToolListItem::setStandbyTempText(const char* text)
	{
		Lock lock;
		lv_label_set_text(m_standbyTemp, text);
	}

	void ToolListItem::showTemps(bool show)
	{
		Lock lock;
		if (show)
		{
			lv_obj_remove_flag(m_status, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(m_currentTemp, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(m_activeTemp, LV_OBJ_FLAG_HIDDEN);
			lv_obj_remove_flag(m_standbyTemp, LV_OBJ_FLAG_HIDDEN);
		}
		else
		{
			lv_obj_add_flag(m_status, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(m_currentTemp, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(m_activeTemp, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(m_standbyTemp, LV_OBJ_FLAG_HIDDEN);
		}
	}

	void ToolListItem::onNameEvent(lv_event_t* e)
	{
		ToolListItem* view = static_cast<ToolListItem*>(lv_event_get_user_data(e));
		view->m_presenter.toggleState();
	}

	void ToolListItem::onStatusEvent(lv_event_t* e)
	{
		ToolListItem* view = static_cast<ToolListItem*>(lv_event_get_user_data(e));
		view->m_presenter.toggleSubState();
	}

	void ToolListItem::onActiveStandbyEvent(lv_event_t* e)
	{
		ToolListItem* view = static_cast<ToolListItem*>(lv_event_get_user_data(e));
		lv_obj_t* obj = lv_event_get_target_obj(e);

		if (!view->m_presenter.configureNumberPad(obj == view->m_activeTemp))
		{
			warn("Failed to configure number pad");
			view->getToolList().hideNumberPad();
			return;
		}
		view->getToolList().showNumberPad(*view);
	}

	ToolListNumPad::ToolListNumPad(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_header(lv_label_create(getCont()))
		, m_numberPad("tool_list_number_pad", getCont(), layout_t(0, 0, 100, 100))
	{
		Lock lock;

		// Layout
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_grow(m_header, 0);
		lv_obj_set_flex_grow(m_numberPad.getCont(), 1);
		lv_obj_set_width(m_header, LV_PCT(100));

		// Header
		lv_label_set_text(m_header, "");

		// Number Pad
		m_numberPad.setCloseOnConfirm(false);
		m_numberPad.setValue(0);
	}

	bool ToolListNumPad::back()
	{
		closeScreen(this, false);
		return true;
	}

	ToolList::ToolList(const std::string& name, lv_obj_t* parent)
		: View<ToolListPresenter>(name, parent)
		, m_header(lv_obj_create(getCont()))
		, m_headerTool(lv_label_create(m_header))
		, m_headerStatus(lv_label_create(m_header))
		, m_headerCurrent(lv_label_create(m_header))
		, m_headerActive(lv_label_create(m_header))
		, m_headerStandby(lv_label_create(m_header))
		, m_list(lv_obj_create(getCont()))
		, m_numberPad("tool_list_number_pad", lv_screen_active(), layout_t(65, 0, 35, 100))
	{
		init();
	}

	ToolList::ToolList(const std::string& name, lv_obj_t* parent, layout_t layout)
		: View<ToolListPresenter>(name, parent, layout)
		, m_header(lv_obj_create(getCont()))
		, m_headerTool(lv_label_create(m_header))
		, m_headerStatus(lv_label_create(m_header))
		, m_headerCurrent(lv_label_create(m_header))
		, m_headerActive(lv_label_create(m_header))
		, m_headerStandby(lv_label_create(m_header))
		, m_list(lv_obj_create(getCont()))
		, m_numberPad("tool_list_number_pad", lv_screen_active(), layout_t(65, 0, 35, 100))
	{
		init();
	}

	void ToolList::init()
	{
		Lock lock;
		setLayoutStyle(LV_LAYOUT_FLEX, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_style_pad_row(getCont(), 0, 0);
		// lv_obj_remove_flag(getCont(), LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_remove_flag(m_header, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_add_flag(m_list, LV_OBJ_FLAG_SCROLLABLE);

		lv_obj_set_flex_flow(m_list, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_style_pad_row(m_list, 0, 0);
		lv_obj_set_style_pad_all(m_list, 0, 0);

		lv_obj_set_size(m_header, LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_size(m_list, LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_flex_flow(m_header, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_grow(m_headerTool, 4);
		lv_obj_set_flex_grow(m_headerStatus, 3);
		lv_obj_set_flex_grow(m_headerCurrent, 2);
		lv_obj_set_flex_grow(m_headerActive, 2);
		lv_obj_set_flex_grow(m_headerStandby, 2);

		lv_label_set_text(m_headerTool, _("toollist_tool"));
		lv_label_set_text(m_headerStatus, _("toollist_status"));
		lv_label_set_text(m_headerCurrent, _("toollist_current"));
		lv_label_set_text(m_headerActive, _("toollist_active"));
		lv_label_set_text(m_headerStandby, _("toollist_standby"));

		// Number Pad

		m_numberPad.hide();
	}

	void ToolList::setItemCnt(size_t cnt)
	{
		Lock lock;
		size_t currentCnt = getItemCnt();
		if (cnt <= currentCnt)
		{
			while (m_items.size() > cnt)
			{
				m_items.pop_back();
			}
			return;
		}

		for (size_t i = currentCnt; i < cnt; i++)
		{
			m_items.emplace_back(std::make_shared<ToolListItem>(
				*this, utils::format("%s_%u", getName(), i).c_str(), m_list, layout_t(0, 0, 100, LV_SIZE_CONTENT)));
		}
	}

	size_t ToolList::getItemCnt() const
	{
		return m_items.size();
	}

	std::shared_ptr<ToolListItem> ToolList::getToolListItem(size_t index) const
	{
		if (index >= m_items.size())
		{
			return nullptr;
		}
		return m_items.at(index);
	}

	void ToolList::showNumberPad(const ToolListItem& item)
	{
		Lock lock;
		m_numberPad.clear();
		openScreen(&m_numberPad, false);
		// m_numberPad.show();
	}

} // namespace UI
