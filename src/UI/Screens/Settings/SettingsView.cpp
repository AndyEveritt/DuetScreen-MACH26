#include "SettingsView.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	SettingsView::SettingsView(lv_obj_t* parent)
		: View("settings_view", parent, layout_t(0, 0, 100, 100))
		, m_settingsTabView(lv_tabview_create(getCont()))
		, m_developerSettingsTab(lv_tabview_add_tab(m_settingsTabView, _("settings_developer")))
		, m_developerSettingsView(m_developerSettingsTab)
	{
		lv_obj_set_style_border_width(getCont(), 2, LV_PART_MAIN);
		lv_obj_set_style_border_color(getCont(), lv_color_hex(0x000000), LV_PART_MAIN);
		lv_obj_set_style_border_opa(getCont(), LV_OPA_100, LV_PART_MAIN);

		lv_tabview_set_tab_bar_position(m_settingsTabView, LV_DIR_LEFT);
	}

	void SettingsView::onShow()
	{
		// m_developerSettingsView.hide();
	}

	DeveloperSettingsView::DeveloperSettingsView(lv_obj_t* parent)
		: BaseView("developer_settings_view", parent, layout_t(0, 0, 100, 100))
		, m_debugLevelCont(lv_obj_create(getCont()))
		, m_debugLevelLabel(lv_label_create(m_debugLevelCont))
		, m_debugLevel(lv_dropdown_create(m_debugLevelCont))
#if DEBUG_BORDERS
		, m_debugBorders(lv_checkbox_create(getCont()))
#endif
	{
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN_WRAP);

		// Debug Level
		lv_obj_set_flex_flow(m_debugLevelCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_size(m_debugLevelCont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_column(m_debugLevelCont, 5, 0);
		lv_label_set_text(m_debugLevelLabel, _("settings_debug_level"));
		std::string options;
		for (const auto& level : DebugLevelStrings)
		{
			options += level;
			options += "\n";
		}
		lv_dropdown_set_options(m_debugLevel, options.c_str());
		lv_dropdown_set_selected(m_debugLevel, static_cast<uint32_t>(GetDebugLevel()));
		lv_dropdown_set_selected_highlight(m_debugLevel, true);
		lv_obj_add_event_cb(m_debugLevel, onDebugLevelEvent, LV_EVENT_VALUE_CHANGED, NULL);

#if DEBUG_BORDERS
		lv_checkbox_set_text(m_debugBorders, _("settings_debug_borders"));
		lv_obj_set_state(m_debugBorders,
						 LV_STATE_CHECKED,
						 Styles::instance().hasStyle(lv_screen_active(), &Styles::instance().debugBorders.style));
		lv_obj_add_event_cb(m_debugBorders, onDebugBordersEvent, LV_EVENT_VALUE_CHANGED, this);
#endif
	}

	void DeveloperSettingsView::onDebugLevelEvent(lv_event_t* e)
	{
		lv_obj_t* dropdown = (lv_obj_t*)lv_event_get_target(e);
		size_t lvl = lv_dropdown_get_selected(dropdown);
		SetDebugLevel(static_cast<DebugLevel>(lvl));
	}

#if DEBUG_BORDERS
	void DeveloperSettingsView::onDebugBordersEvent(lv_event_t* e)
	{
		lv_obj_t* cb = (lv_obj_t*)lv_event_get_target(e);
		bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
		StorageHelper::setData<bool>(ID_DEBUG_BORDERS, checked);
		Styles::instance().showDebugBorders(lv_screen_active(), checked);
	}
#endif
} // namespace UI
