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
#include "i18n/i18n.h"

namespace UI
{
	static constexpr int32_t s_mainWindowLayoutColDsc[3] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
	static constexpr int32_t s_mainWindowLayoutRowDsc[3] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	Dashboard::Dashboard(const std::string& name, LvObj& parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
		, m_fileView(m_tabs.addTab(_("app_drawer.files")), this)
		, m_statusView(m_tabs.addTab(_("app_drawer.status")))
	{
		setStylePad(0);
		// setExtDrawSize(100); /* for outer tab buttons */

		m_toolList.addStyle(Themes::getLvglStyles().card);
		m_graph.addStyle(Themes::getLvglStyles().card);
		m_fileView.addStyle(Themes::getLvglStyles().card);

		setLayoutStyle(LV_LAYOUT_GRID);
		setGridDsc(s_mainWindowLayoutColDsc, s_mainWindowLayoutRowDsc);
		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		// m_toolList.setMaxHeight(LV_PCT(50));
		setGridCell(m_toolList, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_graph, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_tabs, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

		/* Graph */
		m_graph.setXRange({.min = -60, .max = 0});
		m_graph.setYRange({.min = 0, .max = 300});
		m_graph.setXCount(-m_graph.getXRange().min * MODEL_TICK_HZ * 2);

		/* Tabs (Jobs & Status) */
		m_tabs.setActiveTab(0);
		// m_tabs.setTabBarPosition(LV_DIR_TOP, true);
	}

	void Dashboard::disableJobsTab(bool disable)
	{
		m_tabs.disableTab(0, disable);
	}

	void Dashboard::clear()
	{
		m_toolList.setToolCount(0);
		m_toolList.hideNumberPad();
		m_graph.clear();
	}

} // namespace UI
