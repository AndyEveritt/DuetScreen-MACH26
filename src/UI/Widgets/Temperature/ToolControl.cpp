/*
 * ToolControl.cpp
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#include "ToolControl.h"
#include "Debug.h"

namespace UI
{
	ToolControl::ToolControl(const std::string& name, lv_obj_t* parent)
		: View(name, parent)
		, m_toolInfoCont(name + "_tool_info", getRoot())
		, m_name(name + "_tool_name", m_toolInfoCont)
		, m_state(name + "_tool_state", m_toolInfoCont)
		, m_heaters(name + "_heaters", getRoot())
	{
		UI_LOCK();
		setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_toolInfoCont.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_toolInfoCont.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_toolInfoCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_name.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_state.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		m_heaters.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_heaters.setListFlow(LV_FLEX_FLOW_COLUMN);
		m_heaters.setListSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_name.addClickedCallback(
			[](lv_event_t* e)
			{
				auto& control = *(ToolControl*)lv_event_get_user_data(e);
				control.m_presenter->toggleToolState();
			},
			this);

		m_toolInfoCont.addStyle(Themes::getLvglStyles().no_border);
	}

	void ToolControl::setToolName(std::string_view name)
	{
		m_name.setText(name);
	}

	void ToolControl::setToolState(ToolControlPresenter::tool_state_t state, std::string_view str)
	{
		m_name.setChecked(state == ToolControlPresenter::tool_state_t::active);
		m_state.setText(str);
	}
} // namespace UI
