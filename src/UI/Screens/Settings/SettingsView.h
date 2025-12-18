#pragma once

#include "Hardware/Duet.h"
#include "SettingsPresenter.h"
#include "UI/Components/Button/Button.h"
#include "UI/Components/Containers/TabView.h"
#include "UI/Components/Input/DropdownMenu.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/Input/Slider.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/Modal/Modal.h"
#include "UI/Components/Theme/ThemePreview.h"
#include "UI/Core/View.h"
#include "UI/Widgets/HardwareTest/HardwareTest.h"
#include "UI/Widgets/Network/WifiSelector.h"
#include "i18n/i18n.h"

namespace UI
{
	class SettingsTab : public LvContainer
	{
	  public:
		SettingsTab(const std::string& name, LvObj& parent);

	  protected:
		void createHeader(std::string_view text);
		void createRow(std::string_view label, LvObj& obj);
		void createSpanRow(LvObj& obj);
		void setRowVisibility(LvObj& obj, bool show);

		static constexpr int32_t m_maxRowCount = 10;
		size_t m_rowCount = 0;

		std::array<std::unique_ptr<LvLabel>, m_maxRowCount> m_labels;
		std::array<int32_t, 3> m_colDsc;
		std::array<int32_t, m_maxRowCount + 1> m_rowDsc;
	};

	class GeneralSettings : public View<GeneralSettingsPresenter, SettingsTab>
	{
	  public:
		GeneralSettings(const std::string& name, LvObj& parent);

	  private:
		void onInit() override;
		void onShow() override;

		/* General */
		LvLabel m_buildTime{"build_time", getRoot()};
		DropdownMenu m_language{"language", getRoot()};
		Slider m_brightness{"brightness", getRoot()};
		Slider m_screensaverTimeout{"screensaver_timeout", getRoot()};

		/* Notifications */
		LvCheckbox m_displayConnectedMessage{"display_connected_message", getRoot()};
		LvCheckbox m_notificationAutoCloseError{"notification_auto_close_error", getRoot()};
		DropdownMenu m_notificationLevel{"notification_level", getRoot()};
		Slider m_notificationTimeout{"info_timeout", getRoot()};
	};

	class ConnectionSettings : public View<ConnectionSettingsPresenter, SettingsTab>
	{
	  public:
		ConnectionSettings(const std::string& name, LvObj& parent);

		void setKeyboard(LvKeyboard* keyboard);

	  private:
		void showConnectionMethodSettings(const Comm::CommunicationType method);

		void onInit() override;
		void onShow() override;

		DropdownMenu m_connectionMethod{"connection_method", getRoot()};
		DropdownMenu m_usbMode{"usb_mode", getRoot()};
		Slider m_pollInterval{"poll_interval", getRoot()};

		/* USB Settings */
		// None

		/* Wifi Settings */
		TextBox m_duetIpAddress{"duet_ip_address", getRoot()};
		TextBox m_duetPassword{"duet_password", getRoot()};

		/* UART Settings */
		// None

		/* Network */
		WifiSelector m_wifiSelector{"wifi_selector", getRoot()};

		LvKeyboard* m_keyboard;
	};

	class DisplaySettings : public View<DisplaySettingsPresenter, SettingsTab>
	{
	  public:
		DisplaySettings(const std::string& name, LvObj& parent);

	  private:
		void updateThemePreview();

		void onInit() override;
		void onShow() override;

		DropdownMenu m_theme{"theme", getRoot()};
		DropdownMenu m_font{"font", getRoot()};
		ThemePreview m_themePreview{"theme_preview", getRoot()};
	};

	class DeveloperSettings : public View<DeveloperSettingsPresenter, SettingsTab>
	{
	  public:
		DeveloperSettings(const std::string& name, LvObj& parent);

		HardwareTest& getHardwareTest() { return m_hardwareTest; }

	  private:
		static void onDebugLevelEvent(lv_event_t* e);
#if DEBUG_BORDERS
		static void onDebugBordersEvent(lv_event_t* e);
#endif
		static void onEnableSSHEvent(lv_event_t* e);
		static void onRestartEvent(lv_event_t* e);
		static void onEraseAndRestartEvent(lv_event_t* e);
		static void onRebootEvent(lv_event_t* e);

		void onInit() override;
		void onShow() override;

		DropdownMenu m_debugLevel{"debug_level", getRoot()};
		LvCheckbox m_enableAdvancedSettings{"enable_advanced_settings", getRoot()};
#if DEBUG_BORDERS
		LvCheckbox m_debugBorders{"debug_borders", getRoot()};
#endif
		LvCheckbox m_enableSSH{"enable_ssh", getRoot()};
#if LV_USE_SYSMON
		LvCheckbox m_enableSystemMonitor{"enable_system_monitor", getRoot()};
#endif
		LvCheckbox m_systemLogging{"system_logging", getRoot()};

		/* Controls */
		LvContainer m_controls{"controls", getRoot()};
		Button m_restart{"restart", m_controls};
		Button m_eraseAndRestart{"erase_and_restart", m_controls};
		Button m_reboot{"reboot", m_controls};
		Button m_startHardwareTest{"start_hardware_test", m_controls};

		HardwareTest m_hardwareTest;
	};

#if 0
	class DuetSettingsView : public SettingsSubView
	{
	  public:
		DuetSettingsView(LvObj& parent, SettingsView& mainSettingsView);

		class UsbSettings : public LvContainer
		{
		  public:
			UsbSettings(DuetSettingsView& parent);

		  private:
		};

		class WifiSettings : public LvContainer
		{
		  public:
			WifiSettings(DuetSettingsView& parent);

		  private:
			TextBox m_hostname{"hostname", getRoot()};
			TextBox m_password{"password", getRoot()};
		};

		class UartSettings : public LvContainer
		{
		  public:
			UartSettings(DuetSettingsView& parent);

		  private:
		};

	  private:
		static void onConnectionMethodEvent(lv_event_t* e);

		void showConnectionMethodSettings(const Comm::CommunicationType method);
		void onInit() override;
		void onShow() override;

		DropdownMenu m_connectionMethod{"connection_method", getRoot()};
		UsbSettings m_usbSettings;
		WifiSettings m_wifiSettings;
		UartSettings m_uartSettings;
		Slider m_pollInterval{"poll_interval", getRoot()};
	};

	class ScreenSettingsView : public SettingsSubView
	{
	  public:
		ScreenSettingsView(LvObj& parent, SettingsView& mainSettingsView);

	  private:
		void onInit() override;
		void onShow() override;

		LvLabel m_firmwareVersion{"firmware_version", getRoot()};
		LvLabel m_buildTime{"build_time", getRoot()};
		DropdownMenu m_language{"language", getRoot()};
		DropdownMenu m_usbMode{"usb_mode", getRoot()};
		Slider m_brightness{"brightness", getRoot()};
		Slider m_screensaverTimeout{"screensaver_timeout", getRoot()};
		LvCheckbox m_systemLogging{"system_logging", getRoot()};
		LvCheckbox m_displayConnectedMessage{"display_connected_message", getRoot()};
		DropdownMenu m_notificationLevel{"notification_level", getRoot()};
		Slider m_notificationTimeout{"info_timeout", getRoot()};
		LvCheckbox m_notificationAutoCloseError{"notification_auto_close_error", getRoot()};
	};

	class ThemeSettingsView : public SettingsSubView
	{
	  public:
		ThemeSettingsView(LvObj& parent, SettingsView& mainSettingsView);

	  private:
		void updateThemePreview();
		void onInit() override;
		void onShow() override;

		DropdownMenu m_font{"font", getRoot()};
		DropdownMenu m_theme{"theme", getRoot()};
		ThemePreview m_themePreview{"theme_preview", getRoot()};
	};

	class NetworkSettingsView : public View<NetworkSettingsPresenter, SettingsSubView>
	{
	  public:
		NetworkSettingsView(LvObj& parent, SettingsView& mainSettingsView);

		void setIpAddress(const std::string& ipAddress);
		void setEnabled(bool enabled);
		void setNetworkCount(size_t count);
		void setNetworkDetails(size_t index, const std::string& ssid, int32_t signalLevel, bool known, bool connected);

	  private:
		static void onNetworkSelectionEvent(lv_event_t* e);
		static void onRefreshEvent(lv_event_t* e);
		void onPasswordCloseEvent();
		void onPasswordConfirmEvent();

		void onShow() override;
		void onHide() override;

		LvContainer m_topBar;
		LvLabel m_ipAddress;
		Button m_refresh;

		lv_obj_t* m_networkList;
		Modal<MessageBox> m_passwordWindow;
		TextBox m_passwordInput;
	};

	class DeveloperSettingsView : public SettingsSubView
	{
	  public:
		DeveloperSettingsView(LvObj& parent, SettingsView& mainSettingsView);

	  private:
		static void onDebugLevelEvent(lv_event_t* e);
#  if DEBUG_BORDERS
		static void onDebugBordersEvent(lv_event_t* e);
#  endif
		static void onEnableSSHEvent(lv_event_t* e);
		static void onRestartEvent(lv_event_t* e);
		static void onEraseAndRestartEvent(lv_event_t* e);
		static void onRebootEvent(lv_event_t* e);

		lv_obj_t* m_debugLevelCont;
		lv_obj_t* m_debugLevelLabel;
		lv_obj_t* m_debugLevel;

#  if DEBUG_BORDERS
		lv_obj_t* m_debugBorders;
#  endif
		lv_obj_t* m_enableSSH;
#  if LV_USE_SYSMON
		LvCheckbox m_enableSystemMonitor{"enable_system_monitor", getRoot()};
#  endif

		Button m_restart;
		Button m_eraseAndRestart;
		Button m_reboot;
		Button m_startHardwareTest;
	};
#endif

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
		friend class SettingsSubView;
		friend class DuetSettingsView;
		friend class ScreenSettingsView;
		friend class ThemeSettingsView;

	  public:
		SettingsView(const std::string& name, LvObj& parent);

		void showKeyboard(bool show,
						  lv_keyboard_mode_t mode = LV_KEYBOARD_MODE_TEXT_LOWER,
						  LvTextArea* textArea = nullptr);
		void setKeyboardTextArea(LvTextArea* textArea);

		bool back() override;

	  protected:
		LvKeyboard& getKeyboard() { return m_keyboard; }

		void onShow() override;
		void onHide() override;

		TabView m_tabs{"tabs", getRoot()};

		GeneralSettings m_generalSettings{"general_settings", m_tabs.addTab(_("settings.tabs.general"))};
		ConnectionSettings m_connectionSettings{"connection_settings", m_tabs.addTab(_("settings.tabs.connection"))};
		DisplaySettings m_displaySettings{"display_settings", m_tabs.addTab(_("settings.tabs.display"))};
		DeveloperSettings m_developerSettings{"developer_settings", m_tabs.addTab(_("settings.tabs.developer"))};

		LvKeyboard m_keyboard{"keyboard", getRoot()};
	};
} // namespace UI