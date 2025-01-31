#pragma once

#include "SettingsPresenter.h"
#include "UI/Components/Button.h"
#include "UI/Components/NumberPad/NumberPad.h"
#include "UI/Core/View.h"

namespace UI
{
	class SettingsSubView : public BaseView
	{
	  public:
		SettingsSubView(const std::string& name, lv_obj_t* parent)
			: SettingsSubView(name, parent, nullptr)
		{
		}
		SettingsSubView(const std::string& name, lv_obj_t* parent, SettingsView* mainSettingsView);

		void setMainSettingsView(SettingsView* mainSettingsView) { m_mainSettingsView = mainSettingsView; }
		SettingsView* getMainSettingsView() const { return m_mainSettingsView; }
		SettingsPresenter& getMainSettingsPresenter() const;

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

	class NetworkSettingsView : public View<NetworkSettingsPresenter, SettingsSubView>
	{
	  public:
		NetworkSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView);

		void setNetworkCount(size_t count);
		void setNetworkDetails(size_t index, const std::string& ssid, int32_t signalLevel, bool known);

	  private:
		static void onNetworkSelectionEvent(lv_event_t* e);
		static void onPasswordCloseEvent(lv_event_t* e);
		static void onPasswordConfirmEvent(lv_event_t* e);

		void onShow() override;

		lv_obj_t* m_networkList;
		lv_obj_t* m_passwordWindow;
		lv_obj_t* m_passwordInput;
		lv_obj_t* m_passwordSsid;
		// Button m_refresh;
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

		void showKeyboard(bool show,
						  lv_keyboard_mode_t mode = LV_KEYBOARD_MODE_TEXT_LOWER,
						  lv_obj_t* textArea = nullptr);
		void setKeyboardTextArea(lv_obj_t* textArea);

		DuetSettingsView& getDuetSettingsView() { return m_duetSettingsView; }
		NetworkSettingsView& getNetworkSettingsView() { return m_networkSettingsView; }
		DeveloperSettingsView& getDeveloperSettingsView() { return m_developerSettingsView; }

		virtual bool back() override;

	  private:
		static void onWindowSelectEvent(lv_event_t* e);

		virtual void onShow() override;
		virtual void onHide() override;

		lv_obj_t* m_settingsList;
		lv_obj_t* m_subWindow;

		lv_obj_t* m_connectivityHeader;
		lv_obj_t* m_duetSettings;
		lv_obj_t* m_networkSettings;
		lv_obj_t* m_devHeader;
		lv_obj_t* m_developerSettings;

		DuetSettingsView m_duetSettingsView;
		NetworkSettingsView m_networkSettingsView;
		DeveloperSettingsView m_developerSettingsView;

		BaseView* m_currentSubView;

		lv_obj_t* m_keyboard;
		int32_t m_layoutColDsc[3] = {LV_GRID_FR(1), LV_GRID_FR(4), LV_GRID_TEMPLATE_LAST};
		int32_t m_layoutRowDsc[3] = {LV_GRID_FR(2), 0, LV_GRID_TEMPLATE_LAST};
	};
} // namespace UI