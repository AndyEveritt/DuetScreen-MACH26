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

	static constexpr int32_t s_layoutColDsc[3] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static constexpr int32_t s_layoutRowDsc[3] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	static constexpr int32_t s_mainWindowLayoutColDsc[3] = {LV_GRID_FR(3), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
	static constexpr int32_t s_mainWindowLayoutRowDsc[3] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	HomeView::HomeView()
		: View("HomeView", lv_screen_active(), layout_t(0, 0, 100, 100))
		, m_statusBar(getRoot())
		, m_sideBar("sidebar", getRoot())
		, m_mainWindow("main_window", getRoot())
		, m_toolList("tool_list", m_mainWindow, m_mainWindow)
		, m_graph("graph", m_mainWindow)
		, m_windowSelect("window_select", m_mainWindow)
		, m_moveWindow("move_window_select",
					   m_windowSelect,
					   _("move"),
					   layout_t(0, 0, s_windowSelectorItemWidth, s_windowSelectorItemHeight))
		, m_temperatureWindow("temperature_window_select",
							  m_windowSelect,
							  _("temperature"),
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
		, m_consoleView(m_mainWindow)
		, m_moveView(m_mainWindow)
		, m_temperatureView(m_mainWindow)
		, m_fanView(m_mainWindow)
		, m_fileView(m_mainWindow)
		, m_heightmapView(m_mainWindow)
		, m_settingsView(m_mainWindow)
		, m_statusView(m_mainWindow)
		, m_alert("alert", getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
		, m_updatePrompt("update_prompt", getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT))
		, m_numberpad("numberpad", m_mainWindow, layout_t(0, 0, LV_SIZE_CONTENT, 100))
		, m_kb("keyboard", m_mainWindow)
	{
		UI_LOCK();
		LOG_INFO("Creating UI");
		addHomeScreen(this);

		setLayoutStyle(LV_LAYOUT_GRID);
		setGridDsc(s_layoutColDsc, s_layoutRowDsc);
		setGridCell(m_statusBar, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_sideBar, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
		setGridCell(m_mainWindow, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

		lv_obj_set_flex_grow(m_mainWindow, 1);
		lv_obj_set_height(m_mainWindow, LV_PCT(100));
		lv_obj_add_style(m_mainWindow, Themes::getLvglStyles().pad_zero, 0);
		lv_obj_add_style(m_mainWindow, Themes::getLvglStyles().no_border, 0);

		// Main Window Layout
		lv_obj_set_layout(m_mainWindow, LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(m_mainWindow, s_mainWindowLayoutColDsc, s_mainWindowLayoutRowDsc);
		lv_obj_set_grid_cell(m_toolList.getRoot(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_graph.getRoot(), LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_windowSelect, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

		// Tool List
		m_toolList.activate();

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
		m_moveWindow.setUserData(&m_moveView);
		m_temperatureWindow.setUserData(&m_temperatureView);
		m_statusWindow.setUserData(&m_statusView);
		m_heightmapWindow.setUserData(&m_heightmapView);
		m_fansWindow.setUserData(&m_fanView);
		m_filesWindow.setUserData(&m_fileView);
		m_settingsWindow.setUserData(&m_settingsView);

		m_moveWindow.addClickedCallback(onWindowSelectEvent, this);
		m_temperatureWindow.addClickedCallback(onWindowSelectEvent, this);
		m_fansWindow.addClickedCallback(onWindowSelectEvent, this);
		m_filesWindow.addClickedCallback(
			[](lv_event_t* e)
			{
				FileView* view = static_cast<FileView*>(lv_obj_get_user_data((lv_obj_t*)lv_event_get_target(e)));
				view->getPresenter()->setBaseFolder(FilePresenter::BaseFolder::GCODES);
				openScreen(view, false);
			},
			this);
		m_heightmapWindow.addClickedCallback(onWindowSelectEvent, this);
		m_statusWindow.addClickedCallback(onWindowSelectEvent, this);
		m_settingsWindow.addClickedCallback(onWindowSelectEvent, this);

		m_consoleView.hide();
		m_moveView.hide();
		m_temperatureView.hide();
		m_fanView.hide();
		m_fileView.hide();
		m_heightmapView.hide();
		m_statusView.hide();
		m_settingsView.hide();

		// Message Box
		m_alert.hide();
		m_alert.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_alert.setAlign(LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_style_max_height(m_alert.getRoot(), LV_PCT(70), LV_PART_MAIN);

		// Update Prompt
		m_updatePrompt.hide();
		m_updatePrompt.setTitle(_("update_available"));
		m_updatePrompt.setText(_("update_available_text"));
		m_updatePrompt.setOkBtnText(_("update_confirm"));
		m_updatePrompt.setCancelBtnText(_("update_cancel"));
		m_updatePrompt.setOkCallback([this]() { m_presenter->update(); });
		m_updatePrompt.okVisible(true);
		m_updatePrompt.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_updatePrompt.setAlign(LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_style_max_height(m_updatePrompt.getRoot(), LV_PCT(70), LV_PART_MAIN);

		// Keyboard
		showKeyboard(false);
		m_kb.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_kb.setAlign(LV_ALIGN_BOTTOM_MID, 0, 0);
		m_kb.setSize(LV_PCT(100), LV_PCT(50));

		m_numberpad.hide();

		// Styles::instance().removeTheme(getCont());
		// lv_obj_remove_style(getCont(), &Styles::instance().debugBorders.style, 0);
		// lv_theme_apply(getCont());
		// lv_obj_refresh_style(getCont(), LV_PART_ANY, LV_STYLE_PROP_ANY);

		// lv_obj_t* cont = lv_obj_create(getCont());
		// lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_PCT(100));
		// lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
		// lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

		// lv_obj_t* image = lv_image_create(cont);
		// lv_obj_set_size(image, 100, 200);
		// lv_obj_align(image, LV_ALIGN_CENTER, 0, 0);
		// lv_image_set_src(image, IMAGE_ASSET("ffmpeg.png"));
		// lv_image_set_inner_align(image, LV_IMAGE_ALIGN_CONTAIN);

		// lv_obj_t* image2 = lv_image_create(cont);
		// lv_obj_set_size(image2, 100, 100);
		// lv_obj_align(image2, LV_ALIGN_CENTER, 0, 0);
		// lv_image_set_src(image2, IMAGE_ASSET("qoi.bmp"));
		// lv_image_set_inner_align(image2, LV_IMAGE_ALIGN_STRETCH);

		// lv_obj_t* player = lv_ffmpeg_player_create(cont);
		// lv_ffmpeg_player_set_src(player, VIDEO_ASSET("birds.mp4"));
		// lv_ffmpeg_player_set_auto_restart(player, true);
		// lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
		// lv_obj_set_size(player, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		// m_presenter->init();
	}

	void HomeView::clear()
	{
		m_graph.clear();
		m_toolList.setItemCnt(0);
		clearMessageBoxes();
		m_toolList.hideNumberPad();
	}

	void HomeView::onShow()
	{
		// m_heightmapView.show();
		m_toolList.activate();
		m_statusBar.activate();
	}

	void HomeView::onHide()
	{
		// Clear the tool list
		m_graph.clear();
		m_toolList.deactivate();
		m_toolList.setItemCnt(0);
		m_toolList.hideNumberPad();
	}

	void HomeView::onWindowSelectEvent(lv_event_t* e)
	{
		UI_LOCK();
		HomeView* view = (HomeView*)lv_event_get_user_data(e);
		LvObj* selectedWindow = (LvObj*)lv_obj_get_user_data((lv_obj_t*)lv_event_get_target(e));

		// Don't close the home screen as it contains the side bar an the screen that is being opened
		openScreen(selectedWindow, false);
	}

	std::shared_ptr<MessageBox> HomeView::createMessageBox()
	{
		UI_LOCK();
		m_messageBoxList.emplace_back(
			std::make_shared<MessageBox>("home_message_box", getRoot(), layout_t(0, 0, 70, LV_SIZE_CONTENT)));
		std::shared_ptr<MessageBox> msgBox = m_messageBoxList.back();
		lv_obj_add_flag(msgBox->getRoot(), LV_OBJ_FLAG_FLOATING);
		lv_obj_align(msgBox->getRoot(), LV_ALIGN_TOP_MID, 0, 2);
		lv_obj_set_style_max_height(msgBox->getRoot(), LV_PCT(70), LV_PART_MAIN);
		msgBox->hide();
		return msgBox;
	}

	std::shared_ptr<MessageBox> HomeView::getMessageBox(size_t index) const
	{
		UI_LOCK();
		if (index >= getMessageBoxCount())
		{
			return nullptr;
		}
		auto it = m_messageBoxList.cbegin();
		std::advance(it, index);
		return (*it);
	}

	void HomeView::popMessageBox()
	{
		UI_LOCK();
		if (!m_messageBoxList.empty())
		{
			m_messageBoxList.erase(m_messageBoxList.begin());
		}
	}

	void HomeView::showKeyboard(bool show)
	{
		UI_LOCK();
		m_alert.setAlign(LV_ALIGN_TOP_MID, 0, show ? 5 : 0);
		m_alert.setMaxHeight(show ? LV_PCT(45) : LV_PCT(70));
		m_kb.setFlag(LV_OBJ_FLAG_HIDDEN, !show);
	}

	void HomeView::showUpdatePrompt(bool show)
	{
		UI_LOCK();
		m_updatePrompt.setVisible(show, true);
	}
} // namespace UI
