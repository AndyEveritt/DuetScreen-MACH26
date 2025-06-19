/*
 * ExtruderControl.cpp
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#include "ExtruderControl.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	ExtruderControl::ExtruderControl(const std::string& name, lv_obj_t* parent)
		: LvContainer(name, parent)
		, m_toolSelect(name + "_tool_select", getCont())
		, m_retractBtn(name + "_retract", getCont(), LV_SYMBOL_DOWN)
		, m_extrudeBtn(name + "_extrude", getCont(), LV_SYMBOL_UP)
		, m_distanceInput(name + "_distance_input", getCont())
		, m_feedrateInput(name + "_feedrate_input", getCont())
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

		m_toolSelect.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_retractBtn.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_extrudeBtn.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_distanceInput.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_feedrateInput.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_retractBtn.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_extrudeBtn.addStyle(Themes::getLvglStyles().actionBtn, 0);
	}
} // namespace UI
