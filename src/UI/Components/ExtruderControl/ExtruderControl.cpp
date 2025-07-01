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
		, m_filamentContainer(name + "_filament", getCont())
		, m_filamentSelect(name + "_filament_select", m_filamentContainer)
		, m_filamentLoadUnloadBtn(name + "_filament_load_unload", m_filamentContainer, _("filament_load_unload"))
		, m_controlsContainer(name + "_controls", getCont())
		, m_retractBtn(name + "_retract", m_controlsContainer, LV_SYMBOL_UP)
		, m_extrudeBtn(name + "_extrude", m_controlsContainer, LV_SYMBOL_DOWN)
		, m_distanceInput(name + "_distance_input", m_controlsContainer)
		, m_feedrateInput(name + "_feedrate_input", m_controlsContainer)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

		m_toolSelect.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_filamentContainer.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_controlsContainer.setWidth(LV_PCT(100));
		m_controlsContainer.setFlexGrow(1);

		m_toolSelect.setLabel(_("tool_select"));

		m_filamentContainer.setFlexFlow(LV_FLEX_FLOW_ROW);

		m_filamentSelect.setSize(LV_PCT(80), LV_SIZE_CONTENT);
		m_filamentLoadUnloadBtn.setHeight(LV_PCT(100));
		m_filamentLoadUnloadBtn.setFlexGrow(1);
		m_filamentSelect.setLabel(_("filament_select"));

		static int32_t col_dsc[] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		m_controlsContainer.setGridDsc(col_dsc, row_dsc);
		m_controlsContainer.setGridCell(m_distanceInput, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		m_controlsContainer.setGridCell(m_feedrateInput, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		m_controlsContainer.setGridCell(m_retractBtn, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		m_controlsContainer.setGridCell(m_extrudeBtn, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

		m_distanceInput.setLabel(_("distance_input"));
		m_feedrateInput.setLabel(_("feedrate_input"));

		m_toolSelect.addStyle(Themes::getLvglStyles().no_border);
		m_filamentContainer.addStyle(Themes::getLvglStyles().no_border);
		m_filamentContainer.addStyle(Themes::getLvglStyles().pad_zero);
		m_filamentSelect.addStyle(Themes::getLvglStyles().no_border);
		m_controlsContainer.addStyle(Themes::getLvglStyles().no_border);
		m_retractBtn.addStyle(Themes::getLvglStyles().actionBtn);
		m_extrudeBtn.addStyle(Themes::getLvglStyles().actionBtn);
	}
} // namespace UI
