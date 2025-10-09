/*
 * Dashboard.cpp
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#include "Dashboard.h"
#include "Configuration.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	static constexpr int32_t s_mainWindowLayoutColDsc[3] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
	static constexpr int32_t s_mainWindowLayoutRowDsc[3] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	Dashboard::Dashboard(const std::string& name, LvObj& parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
	{
		setStylePad(0);

		m_toolList.addStyle(Themes::getLvglStyles().card);
		m_graph.addStyle(Themes::getLvglStyles().card);
		m_fileView.addStyle(Themes::getLvglStyles().card);

		setLayoutStyle(LV_LAYOUT_GRID);
		setGridDsc(s_mainWindowLayoutColDsc, s_mainWindowLayoutRowDsc);
		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		// m_toolList.setMaxHeight(LV_PCT(50));
		setGridCell(m_toolList, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_graph, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_fileView, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
		setGridCell(m_statusView, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

		/* Graph */
		m_graph.setXRange({.min = -60, .max = 0});
		m_graph.setYRange({.min = 0, .max = 300});
		m_graph.setXCount(-m_graph.getXRange().min * MODEL_TICK_HZ * 2);

		/* Status */
		m_statusView.hide();
	}

	void Dashboard::clear()
	{
		m_toolList.setToolCount(0);
		m_toolList.hideNumberPad();
		m_graph.clear();
	}

} // namespace UI
