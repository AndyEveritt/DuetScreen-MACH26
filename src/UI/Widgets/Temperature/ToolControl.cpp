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
	ToolControl::ToolControl(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		ZoneScoped;
		UI_LOCK();
		setSize(LV_PCT(100), LV_SIZE_CONTENT);
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_toolInfoCont.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_toolInfoCont.setFlexFlow(LV_FLEX_FLOW_ROW_WRAP);
		m_toolInfoCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_toolName.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_state.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_toolName.addStyle(Themes::getLvglStyles().actionBtn);

		m_toolInfoSpacer.setHeight(0);
		m_toolInfoSpacer.setFlexGrow(1);

		m_extrusionFactors.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_extrusionFactors.setListFlow(LV_FLEX_FLOW_ROW);
		m_extrusionFactors.setStylePad(0);
		m_extrusionFactors.getListContainer().setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		m_filament.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_filament.setMinWidth(LV_SIZE_CONTENT);
		m_filament.setFlexGrow(1);
		m_filament.showHint(false);

		m_heaters.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_heaters.setListFlow(LV_FLEX_FLOW_COLUMN);
		m_heaters.setListSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_toolName.addClickedCallback(
			[](lv_event_t* e)
			{
				auto& control = *(ToolControl*)lv_event_get_user_data(e);
				control.m_presenter->toggleToolState();
			},
			this);

		addStyle(Themes::getComponentStyles().tool_selected, LV_STATE_CHECKED);
	}

	void ToolControl::setToolName(std::string_view name)
	{
		ZoneScoped;
		m_toolName.setText(name);
	}

	void ToolControl::setToolState(ToolControlPresenter::tool_state_t state, std::string_view str)
	{
		ZoneScoped;
		bool active = state == ToolControlPresenter::tool_state_t::active;
		setState(LV_STATE_CHECKED, active);
		m_toolName.setChecked(active);
		m_state.setText(str);
	}

	void ToolControl::setDisabled(bool disabled)
	{
		ZoneScoped;
		m_toolName.setDisabled(disabled);
	}

	void ToolControl::setNumberPad(ModalNumberPad* numberPad)
	{
		ZoneScoped;
		UI_LOCK();
		m_numberPad = numberPad;
		for (auto& heater : m_heaters)
		{
			heater->setNumberPad(numberPad);
		}
	}

	void ToolControl::setExtrusionModal(ModalExtrusionFactor* modal)
	{
		ZoneScoped;
		m_extrusionFactorModal = modal;
	}
} // namespace UI
