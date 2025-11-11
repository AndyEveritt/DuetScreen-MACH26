/*
 * ToolList.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "ToolList.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"

namespace UI
{
	ToolList::ToolList(const std::string& name, LvObj& parent, LvObj* numberPadParent)
		: View(name, parent)
		, m_numberPad("number_pad", numberPadParent ? *numberPadParent : parent)
	{
		init();
	}

	void ToolList::init()
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		addStyle(Themes::getLvglStyles().pad_zero);

		m_tools.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_beds.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_chambers.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		/* Number Pad */
		m_numberPad.hide();
	}

	size_t ToolList::setToolCount(size_t count)
	{
		m_tools.setTitle(count > 1 ? _("temperature.tools") : "");
		return m_tools.setItemCount(count,
									[this](size_t index, LvObj& parent)
									{
										auto item = std::make_unique<ToolListTool>(index, parent, *this);
										item->activate();
										return item;
									});
	}

	size_t ToolList::setBedCount(size_t count)
	{
		m_beds.setTitle(count > 1 ? _("temperature.beds") : "");
		return m_beds.setItemCount(count);
	}

	size_t ToolList::setChamberCount(size_t count)
	{
		m_chambers.setTitle(count > 1 ? _("temperature.chambers") : "");
		return m_chambers.setItemCount(count);
	}

	void ToolList::showNumberPad()
	{
		UI_LOCK();
		m_numberPad.clear();
		openModal(&m_numberPad);
	}

	ToolListTool::ToolListTool(size_t index, LvObj& parent, ToolList& toolList)
		: View(index, parent)
	{
		getPresenter()->setToolListPresenter(toolList.getPresenter());

		setSize(LV_PCT(100), LV_SIZE_CONTENT);

		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_toolName.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_heaters.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_heaters.setStylePad(0, LV_PART_MAIN, Padding::TOP);
		m_heaters.setStylePad(0, LV_PART_MAIN, Padding::BOTTOM);
		m_heaters.setStylePad(0, LV_PART_MAIN, Padding::RIGHT);
		m_heaters.getListContainer().setStylePad(0, LV_PART_MAIN);

		m_toolName.addClickedCallback(onNameEvent, this);

		// Styles
		// addStyle(Themes::getLvglStyles().border_color_secondary, LV_STATE_CHECKED);
		addStyle(Themes::getComponentStyles().tool_selected, LV_STATE_CHECKED);

		m_toolName.addStyle(Themes::getLvglStyles().actionBtn);
		m_toolName.addStyle(Themes::getLvglStyles().border_color_card, LV_STATE_CHECKED);

		m_toolName.getLabel().setAlign(LV_ALIGN_LEFT_MID, 0, 0);
	}

	ToolListHeater::ToolListHeater(size_t index, LvObj& parent)
		: ListItem(index, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		setSize(LV_PCT(100), LV_SIZE_CONTENT);
		setStylePad(0, LV_PART_MAIN, Padding::ALL);

		m_label.setStyleTextAlign(LV_TEXT_ALIGN_LEFT);

		m_label.setFlexGrow(3);
		m_status.setFlexGrow(3);
		m_currentTemp.setFlexGrow(2);
		m_activeTemp.setFlexGrow(2);
		m_standbyTemp.setFlexGrow(2);

		// m_activeTemp.setExtClickArea(20);
		// m_standbyTemp.setExtClickArea(20);

		m_status.addClickedCallback(onStatusEvent, this);
		m_activeTemp.addEventCallback(onActiveStandbyEvent, LV_EVENT_CLICKED, this);
		m_standbyTemp.addEventCallback(onActiveStandbyEvent, LV_EVENT_CLICKED, this);

		m_status.addStyle(Themes::getLvglStyles().border_color_card, LV_STATE_CHECKED);
		m_status.addStyle(Themes::getLvglStyles().actionBtn);
		m_activeTemp.addStyle(Themes::getLvglStyles().input);
		m_standbyTemp.addStyle(Themes::getLvglStyles().input);
	}

	void ToolListTool::setLabel(std::string_view text)
	{
		m_toolName.setText(text);
	}

	void ToolListTool::setSelected(const bool selected)
	{
		if (m_selected == selected)
		{
			return;
		}
		UI_LOCK();
		setState(LV_STATE_CHECKED, selected);
		m_selected = selected;
	}

	size_t ToolListTool::setHeaterCount(size_t count)
	{
		m_heaters.setFlag(LV_OBJ_FLAG_HIDDEN, count == 0);
		return m_heaters.setItemCount(count);
	}

	void ToolListTool::onNameEvent(lv_event_t* e)
	{
		UI_LOCK();
		auto view = static_cast<ToolListTool*>(lv_event_get_user_data(e));
		view->m_presenter->toggleState();
	}

	void ToolListHeater::setLabel(std::string_view text)
	{
		m_label.setText(text);
	}

	void ToolListHeater::setStatus(std::string_view status)
	{
		m_status.setText(status);
	}

	void ToolListHeater::setCurrentTemp(float value)
	{
		m_currentTemp.setText(fmt::format("{:.1f}", value));
	}

	void ToolListHeater::setActiveTemp(int32_t value)
	{
		m_activeTemp.setText(fmt::format("{:d}", value));
	}

	void ToolListHeater::setStandbyTemp(int32_t value)
	{
		m_standbyTemp.setText(fmt::format("{:d}", value));
	}

	void ToolListHeater::onStatusEvent(lv_event_t* e)
	{
		UI_LOCK();
		auto view = static_cast<ToolListHeater*>(lv_event_get_user_data(e));
		if (view->m_statusCb)
			view->m_statusCb();
	}

	void ToolListHeater::onActiveStandbyEvent(lv_event_t* e)
	{
		UI_LOCK();
		auto view = static_cast<ToolListHeater*>(lv_event_get_user_data(e));
		LvObj* obj = LvObj::fromPtr(lv_event_get_target_obj(e));

		if (view->m_temperatureCb)
			view->m_temperatureCb(obj == &view->m_activeTemp);
	}
} // namespace UI
