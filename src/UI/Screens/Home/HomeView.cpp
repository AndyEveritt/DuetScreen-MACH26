#include "HomeView.h"

#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/utils.h"

namespace UI
{
	HomeView::HomeView()
		: View("HomeView", layout_t(0, 0, 100, 100))
		, m_sideBar("sidebar", getCont())
		, m_mainWindow(lv_obj_create(getCont()))
		, m_toolList("home_tool_list", m_mainWindow, layout_t(0, 0, 60, 50))
		, m_graph("home_graph", m_mainWindow, layout_t(0, 50, 60, 50))
	{
		setLayoutStyle(LV_LAYOUT_FLEX, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_grow(m_mainWindow, 1);
		lv_obj_set_height(m_mainWindow, LV_PCT(100));

		addHomeScreen(this);

		m_graph.setXRange({.min = -100, .max = 0});
		m_graph.setYRange({.min = 0, .max = 300});
		m_graph.createSeries("series1", {0xff, 0, 0}, "Series 1");
		m_graph.createSeries("series2", {0, 0xff, 0}, "Series 2");
		m_graph.createSeries("series3", {0, 0, 0xff}, "Series 3");

		m_graph.setXCount(20);

		for (size_t i = 0; i < 100; i++)
		{
			m_graph.addData("series1", rand() % 300);
			m_graph.addData("series2", rand() % 300);
			m_graph.addData("series3", rand() % 300);
		}
		// m_graph.clear();
	}

	void HomeView::onHide()
	{
		// Clear the tool list
		m_toolList.setItemCnt(0);
		m_toolList.hideNumberPad();
	}
} // namespace UI
