/*
 * ToolList.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "ToolList.h"
#include "Debug.h"
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

		lv_obj_add_flag(m_activeTemp, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_event_cb(m_activeTemp, activeTempEvent, LV_EVENT_PRESSED, this);
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

	void ToolListItem::activeTempEvent(lv_event_t* e)
	{
		ToolListItem* view = static_cast<ToolListItem*>(lv_event_get_user_data(e));
		if (!view->m_presenter.configureNumberPad())
		{
			warn("Failed to configure number pad");
			view->getToolList().hideNumberPad();
			return;
		}
		view->getToolList().showNumberPad(*view);

		// view->m_presenter.setActiveTemp(value);
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

		m_numberPad.show();
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
		m_numberPad.show();
	}

} // namespace UI
