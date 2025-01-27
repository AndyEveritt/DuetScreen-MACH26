#include "HomeView.h"

#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Settings/SettingsView.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/utils.h"

namespace UI
{
	static constexpr lv_coord_t s_windowSelectorItemWidth = 30;	 // %
	static constexpr lv_coord_t s_windowSelectorItemHeight = 25; // %

	static constexpr int32_t s_layoutColDsc[3] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
	static constexpr int32_t s_layoutRowDsc[3] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	HomeView::HomeView()
		: View("HomeView", layout_t(0, 0, 100, 100))
		, m_sideBar("sidebar", getCont())
		, m_mainWindow(lv_obj_create(getCont()))
		, m_toolList("home_tool_list", m_mainWindow)
		, m_graph("home_graph", m_mainWindow)
		, m_windowSelect(lv_obj_create(m_mainWindow))
		, m_moveWindow("move_window_select",
					   m_windowSelect,
					   _("move"),
					   layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_extrudeWindow("extrude_window_select",
						  m_windowSelect,
						  _("extrude"),
						  layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_statusWindow("status_window_select",
						 m_windowSelect,
						 _("status"),
						 layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_heightmapWindow("heightmap_window_select",
							m_windowSelect,
							_("heightmap"),
							layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_fansWindow("fans_window_select",
					   m_windowSelect,
					   _("fans"),
					   layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_objectCancelWindow("object_cancel_window_select",
							   m_windowSelect,
							   _("object_cancel"),
							   layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_filesWindow("files_window_select",
						m_windowSelect,
						_("files"),
						layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_settingsWindow("settings_window_select",
						   m_windowSelect,
						   _("settings"),
						   layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_moveView(m_mainWindow)
		, m_extrudeView(m_mainWindow)
		, m_fileView(m_mainWindow)
		, m_settingsView(m_mainWindow)
	{
		addHomeScreen(this);

#if DEBUG_BORDERS
		lv_obj_add_style(getCont(), &Styles::instance().debugBorders.style, LV_PART_MAIN);
#endif

		setLayoutStyle(LV_LAYOUT_FLEX, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_grow(m_mainWindow, 1);
		lv_obj_set_height(m_mainWindow, LV_PCT(100));

		// Main Window Layout
		lv_obj_set_layout(m_mainWindow, LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(m_mainWindow, s_layoutColDsc, s_layoutRowDsc);
		lv_obj_set_grid_cell(m_toolList.getCont(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_graph.getCont(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_windowSelect, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

		// Tool List
		m_toolList.show();

		// Graph
		m_graph.setXRange({.min = -60, .max = 0});
		m_graph.setYRange({.min = 0, .max = 300});
		m_graph.setXCount(-m_graph.getXRange().min * MODEL_TICK_HZ * 2);

		// Window select
		lv_obj_set_style_pad_all(m_windowSelect, 2, LV_PART_MAIN);
		lv_obj_set_style_pad_row(m_windowSelect, 2, LV_PART_MAIN);
		lv_obj_set_style_pad_column(m_windowSelect, 2, LV_PART_MAIN);
		lv_obj_set_pos(m_windowSelect, LV_PCT(60), 0);
		lv_obj_set_size(m_windowSelect, LV_PCT(40), LV_PCT(100));
		lv_obj_set_flex_flow(m_windowSelect, LV_FLEX_FLOW_ROW_WRAP);
		lv_obj_set_flex_align(m_windowSelect, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Window select buttons
		m_moveWindow.setCallback(onWindowSelectEvent, LV_EVENT_CLICKED, &m_moveView);
		m_extrudeWindow.setCallback(onWindowSelectEvent, LV_EVENT_CLICKED, &m_extrudeView);
		m_filesWindow.setCallback(onWindowSelectEvent, LV_EVENT_CLICKED, &m_fileView);
		m_settingsWindow.setCallback(onWindowSelectEvent, LV_EVENT_CLICKED, &m_settingsView);

		m_moveView.hide();
		m_extrudeView.hide();
		m_fileView.hide();
		m_settingsView.hide();

		// Styles::instance().removeTheme(getCont());
		// lv_obj_remove_style(getCont(), &Styles::instance().debugBorders.style, 0);
		// lv_theme_apply(getCont());
		// lv_obj_refresh_style(getCont(), LV_PART_ANY, LV_STYLE_PROP_ANY);
	}

	void HomeView::refresh()
	{
		m_graph.clear();
	}

	void HomeView::onHide()
	{
		// Clear the tool list
		m_graph.clear();
		m_toolList.setItemCnt(0);
		m_toolList.hideNumberPad();
	}

	void HomeView::onWindowSelectEvent(lv_event_t* e)
	{
		BaseView* view = (BaseView*)lv_event_get_user_data(e);

		// Don't close the home screen as it contains the side bar an the screen that is being opened
		openScreen(view, false);
	}
} // namespace UI
