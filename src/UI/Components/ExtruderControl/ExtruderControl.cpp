/*
 * ExtruderControl.cpp
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#include "ExtruderControl.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	ExtruderControl::ExtruderControl(const std::string& name, lv_obj_t* parent)
		: LvContainer(name, parent)
		, m_toolSelect(name + "_tool_select", getCont())
		, m_controlsContainer(name + "_controls", getCont())
		, m_retractBtn(name + "_retract", m_controlsContainer, LV_SYMBOL_DOWN)
		, m_extrudeBtn(name + "_extrude", m_controlsContainer, LV_SYMBOL_UP)
		, m_distanceInput(name + "_distance_input", m_controlsContainer)
		, m_feedrateInput(name + "_feedrate_input", m_controlsContainer)
		, m_filamentContainer(name + "_filament", getCont())
		, m_filamentLabel(name + "_filament_label", m_filamentContainer)
		, m_filamentLoadUnloadBtn(name + "_filament_load_unload", m_filamentContainer, _("filament_load_unload"))
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

		m_toolSelect.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_controlsContainer.setWidth(LV_PCT(100));
		m_controlsContainer.setFlexGrow(1);
		m_filamentContainer.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_controlsContainer.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_filamentContainer.setFlexFlow(LV_FLEX_FLOW_ROW);

		m_retractBtn.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_extrudeBtn.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_distanceInput.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_feedrateInput.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_retractBtn.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_extrudeBtn.addStyle(Themes::getLvglStyles().actionBtn, 0);
	}
} // namespace UI
