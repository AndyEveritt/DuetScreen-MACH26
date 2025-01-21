#pragma once

#include "SettingsPresenter.h"
#include "UI/Core/View.h"

namespace UI
{
	class DeveloperSettingsView : public BaseView
	{
	  public:
		DeveloperSettingsView(lv_obj_t* parent);

	  private:
		static void onDebugLevelEvent(lv_event_t* e);
#if DEBUG_BORDERS
		static void onDebugBordersEvent(lv_event_t* e);
#endif
		lv_obj_t* m_debugLevelCont;
		lv_obj_t* m_debugLevelLabel;
		lv_obj_t* m_debugLevel;

#if DEBUG_BORDERS
		lv_obj_t* m_debugBorders;
#endif
	};

	/**
	 * @brief View to configure the screen settings
	 *
	 * This class provides a user interface for configuring various screen settings.
	 *
	 * @note The following subviews are defined:
	 * @note - DuetSettingsView
	 * @note - DisplaySettingsView
	 * @note - LanguageSettingsView
	 * @note - NetworkSettingsView
	 * @note - DeveloperSettingsView.
	 * @note - ThemeSettingsView.
	 *
	 * @param parent The parent LVGL object.
	 */
	class SettingsView : public View<SettingsPresenter>
	{
	  public:
		SettingsView(lv_obj_t* parent);

	  private:
		virtual void onShow() override;

		lv_obj_t* m_settingsTabView;
		lv_obj_t* m_developerSettingsTab;
		DeveloperSettingsView m_developerSettingsView;
	};
} // namespace UI