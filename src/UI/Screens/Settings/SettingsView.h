#pragma once

#include "SettingsPresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"

namespace UI
{
	class SettingsView;

	class SettingsSubView : public BaseView
	{
	  public:
		SettingsSubView(const std::string& name, lv_obj_t* parent, SettingsView* mainSettingsView);

		// void setMainSettingsView(SettingsView* mainSettingsView) { m_mainSettingsView = mainSettingsView; }
		SettingsView* getMainSettingsView() const { return m_mainSettingsView; }

	  protected:
		static void onTextAreaEvent(lv_event_t* e);
		SettingsView* m_mainSettingsView;
	};

	class DuetSettingsView : public SettingsSubView
	{
	  public:
		DuetSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView);

	  private:
		static void onSaveEvent(lv_event_t* e);

		lv_obj_t* m_connectionMethod;
		lv_obj_t* m_hostname;
		lv_obj_t* m_password;
		lv_obj_t* m_pollInterval;
		Button m_save;
	};

	class DeveloperSettingsView : public SettingsSubView
	{
	  public:
		DeveloperSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView);

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

		lv_obj_t* getTabView() const { return m_settingsTabView; }

		void showKeyboard(bool show, lv_keyboard_mode_t mode = LV_KEYBOARD_MODE_TEXT_LOWER);
		void setKeyboardTextArea(lv_obj_t* textArea);

	  private:
		virtual void onShow() override;
		virtual void onHide() override;

		lv_obj_t* m_settingsTabView;
		lv_obj_t* m_duetSettingsTab;
		lv_obj_t* m_developerSettingsTab;

		DuetSettingsView m_duetSettingsView;
		DeveloperSettingsView m_developerSettingsView;

		lv_obj_t* m_keyboard;
		int32_t m_layoutColDsc[2] = {LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
		int32_t m_layoutRowDsc[3] = {LV_GRID_FR(2), 0, LV_GRID_TEMPLATE_LAST};
	};
} // namespace UI