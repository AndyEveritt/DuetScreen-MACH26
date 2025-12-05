#include "SettingsView.h"
#include "BuildDate.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "Hardware/Reset.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "UI/Styles/Styles.h"
#include "UI/Styles/Themes/CustomTheme.h"
#include "i18n/i18n.h"
#include "utils/DisplayHelper.h"
#include "utils/StorageHelper.h"
#include "version.h"

namespace UI
{
	SettingsView::SettingsView(const std::string& name, LvObj& parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
		, m_settingsList(lv_list_create(getRootPtr()))
		, m_subWindow("sub_window", getRoot())
		, m_keyboard("keyboard", getRoot())
		, m_screenHeader(lv_list_add_text(m_settingsList, _("settings.screen_header").c_str()))
		, m_screenSettings(lv_list_add_button(m_settingsList, NULL, _("settings.screen").c_str()))
		, m_themeSettings(lv_list_add_button(m_settingsList, NULL, _("settings.theme").c_str()))
		, m_connectivityHeader(lv_list_add_text(m_settingsList, _("settings.connectivity_header").c_str()))
		, m_duetSettings(lv_list_add_button(m_settingsList, NULL, _("settings.duet").c_str()))
		, m_networkSettings(lv_list_add_button(m_settingsList, LV_SYMBOL_WIFI, _("settings.network").c_str()))
		, m_devHeader(lv_list_add_text(m_settingsList, _("settings.dev_header").c_str()))
		, m_developerSettings(lv_list_add_button(m_settingsList, LV_SYMBOL_SETTINGS, _("settings.developer").c_str()))
		, m_duetSettingsView(m_subWindow, *this)
		, m_deviceSettingsView(m_subWindow, *this)
		, m_themeSettingsView(m_subWindow, *this)
		, m_networkSettingsView(m_subWindow, *this)
		, m_developerSettingsView(m_subWindow, *this)
		, m_currentSubView(&m_deviceSettingsView)
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);

		// Layout
		setGridDsc(m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_settingsList, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_subWindow, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_keyboard, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 1, 1);

		// List
		lv_obj_set_user_data(m_duetSettings, &m_duetSettingsView);
		lv_obj_set_user_data(m_screenSettings, &m_deviceSettingsView);
		lv_obj_set_user_data(m_themeSettings, &m_themeSettingsView);
		lv_obj_set_user_data(m_networkSettings, &m_networkSettingsView);
		lv_obj_set_user_data(m_developerSettings, &m_developerSettingsView);

		lv_obj_add_event_cb(m_duetSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_screenSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_themeSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_networkSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_developerSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);

		m_hardwareTest.hide();

		// Sub window
		m_subWindow.setStylePad(0);

		/* Keyboard */
		m_keyboard.setSize(LV_PCT(100), LV_PCT(40));
	}

	void SettingsView::onWindowSelectEvent(lv_event_t* e)
	{
		UI_LOCK();
		SettingsView* view = (SettingsView*)lv_event_get_user_data(e);
		SettingsSubView* subView = (SettingsSubView*)lv_obj_get_user_data(lv_event_get_target_obj(e));
		SettingsSubView* currentSubView = view->m_currentSubView;

		if (currentSubView == subView)
		{
			return;
		}

		if (currentSubView != nullptr)
		{
			currentSubView->hide();
		}

		view->showKeyboard(false);
		subView->show(true);
		view->m_currentSubView = subView;
	}

	void SettingsView::showKeyboard(bool show, lv_keyboard_mode_t mode, LvTextArea* textArea)
	{
		UI_LOCK();
		if (show)
		{
			m_keyboard.setMode(mode);
			setKeyboardTextArea(textArea);
		}
		else
		{
			setKeyboardTextArea(NULL);
		}
		m_keyboard.setVisible(show);
	}

	void SettingsView::setKeyboardTextArea(LvTextArea* textArea)
	{
		UI_LOCK();
		m_keyboard.setTextArea(textArea);
	}

	bool SettingsView::back()
	{
		UI_LOCK();
		if (!m_keyboard.hasFlag(LV_OBJ_FLAG_HIDDEN))
		{
			showKeyboard(false);
			return true;
		}

		return false;
	}

	void SettingsView::onHide()
	{
		m_currentSubView->hide();
	}

	void SettingsView::onShow()
	{
		showKeyboard(false);
		m_currentSubView->show(true);
	}

	SettingsSubView::SettingsSubView(const std::string& name, LvObj& parent, SettingsView& mainSettingsView)
		: Card(name, parent, layout_t(0, 0, 100, 100))
		, m_mainSettingsView(mainSettingsView)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	}

	std::shared_ptr<SettingsPresenter> SettingsSubView::getMainSettingsPresenter() const
	{
		return m_mainSettingsView.getPresenter();
	}

	void SettingsSubView::onTextAreaEvent(lv_event_t* e)
	{
		UI_LOCK();
		lv_event_code_t code = lv_event_get_code(e);
		if (code != LV_EVENT_FOCUSED && code != LV_EVENT_DEFOCUSED)
		{
			return;
		}

		LvTextArea* ta = static_cast<LvTextArea*>(LvObj::fromPtr(lv_event_get_target_obj(e)));
		SettingsSubView* view = static_cast<SettingsSubView*>(lv_event_get_user_data(e));
		const char* acceptedChars = ta->getAcceptedChars();
		lv_keyboard_mode_t mode = (!acceptedChars || strpbrk(acceptedChars, "abcdefghijklmnopqrstuvwxyz") != nullptr)
									  ? LV_KEYBOARD_MODE_TEXT_LOWER
									  : LV_KEYBOARD_MODE_NUMBER;
		if (code == LV_EVENT_FOCUSED)
		{
			view->getMainSettingsView().showKeyboard(true, mode, ta);
		}

		if (code == LV_EVENT_DEFOCUSED)
		{
			view->getMainSettingsView().showKeyboard(false);
		}
	}

	DuetSettingsView::DuetSettingsView(LvObj& parent, SettingsView& mainSettingsView)
		: SettingsSubView("duet_settings_view", parent, mainSettingsView)
		, m_usbSettings(*this)
		, m_wifiSettings(*this)
		, m_uartSettings(*this)
	{
		UI_LOCK();

		// Connection Method
		std::vector<std::string> options;
		for (const auto& method : Comm::duetCommunicationTypeNames)
		{
			options.push_back(_(method.data()));
		}
		m_connectionMethod.setLabel(_("settings.duet_connection_method"));
		m_connectionMethod.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_connectionMethod.setOptions(options);
		m_connectionMethod.addEventCallback(onConnectionMethodEvent, LV_EVENT_VALUE_CHANGED, this);
		m_connectionMethod.setSelected((uint32_t)Comm::DUET.GetCommunicationType());

		// Poll Interval
		m_pollInterval.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_pollInterval.setLabel(_("settings.duet_poll_interval"));
		m_pollInterval.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_pollInterval.setRange(MIN_PRINTER_POLL_INTERVAL.count(), 2000);
		m_pollInterval.setValue(Comm::DUET.GetPollInterval().count());
		m_pollInterval.setValueChangedCallback(
			[](float value) { Comm::DUET.SetPollInterval(std::chrono::milliseconds(static_cast<int32_t>(value))); });
	}

	DuetSettingsView::UsbSettings::UsbSettings(DuetSettingsView& parent)
		: LvContainer("usb", parent, layout_t(0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT))
	{
		UI_LOCK();
	}

	DuetSettingsView::WifiSettings::WifiSettings(DuetSettingsView& parent)
		: LvContainer("wifi", parent, layout_t(0, 0, 100, LV_SIZE_CONTENT))
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_hostname.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_hostname.setOneLine(true);
		m_hostname.setLabel(_("settings.duet_hostname"));
		m_hostname.setPlaceholderText(_("settings.duet_hostname_prompt"));
		m_hostname.setAcceptedChars("0123456789.");
		m_hostname.setText(Comm::DUET.GetHostname());
		m_hostname.getTextArea().addEventCallback(onTextAreaEvent, LV_EVENT_ALL, &parent);
		m_hostname.addConfirmEventCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				lv_obj_t* ta = (lv_obj_t*)lv_event_get_target(e);
				Comm::DUET.SetHostname(lv_textarea_get_text(ta));
			},
			nullptr);

		m_password.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_password.setOneLine(true);
		m_password.setLabel(_("settings.duet_password"));
		m_password.setPlaceholderText(_("settings.duet_password_prompt"));
		m_password.setPasswordMode(true);
		m_password.setText(Comm::DUET.GetPassword());
		m_password.getTextArea().addEventCallback(onTextAreaEvent, LV_EVENT_ALL, &parent);
		m_password.addConfirmEventCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				lv_obj_t* ta = (lv_obj_t*)lv_event_get_target(e);
				Comm::DUET.SetPassword(lv_textarea_get_text(ta));
			},
			nullptr);
	}

	DuetSettingsView::UartSettings::UartSettings(DuetSettingsView& parent)
		: LvContainer("uart", parent, layout_t(0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT))
	{
		UI_LOCK();
	}

	void DuetSettingsView::onConnectionMethodEvent(lv_event_t* e)
	{
		UI_LOCK();
		DuetSettingsView* view = (DuetSettingsView*)lv_event_get_user_data(e);
		auto comm_type = (Comm::CommunicationType)(view->m_connectionMethod.getSelected());
		Comm::DUET.SetCommunicationType(comm_type);
		switch (comm_type)
		{
		case Comm::CommunicationType::uart:
		case Comm::CommunicationType::usb:
			view->getMainSettingsPresenter()->setUsbMode(UsbMode::Host);
			break;
		case Comm::CommunicationType::network:
			view->getMainSettingsPresenter()->setUsbMode(UsbMode::InternalWiFi);
			break;
		default:
			break;
		}

		view->showConnectionMethodSettings(Comm::DUET.GetCommunicationType());
	}

	void DuetSettingsView::showConnectionMethodSettings(const Comm::CommunicationType method)
	{
		UI_LOCK();

		m_usbSettings.setVisible(method == Comm::CommunicationType::usb);
		m_wifiSettings.setVisible(method == Comm::CommunicationType::network);
		m_uartSettings.setVisible(method == Comm::CommunicationType::uart);
	}

	void DuetSettingsView::onInit()
	{
		m_pollInterval.setNumberPad(&HomeView::instance().getNumberPad());
	}

	void DuetSettingsView::onShow()
	{
		UI_LOCK();
		m_pollInterval.setValue(Comm::DUET.GetPollInterval().count());
		showConnectionMethodSettings(Comm::DUET.GetCommunicationType());
	}

	ScreenSettingsView::ScreenSettingsView(LvObj& parent, SettingsView& mainSettingsView)
		: SettingsSubView("screen", parent, mainSettingsView)
	{
		UI_LOCK();

		m_firmwareVersion.setText(_("settings.firmware_version", FIRMWARE_VERSION));
		m_buildTime.setText(_("settings.build_time", BuildDateText, BuildTimeSuffix));

		m_language.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_language.setLabel(_("settings.language"));
		m_language.setOptions(_("settings.language_en"));

		m_usbMode.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_usbMode.setLabel(_("settings.usb_mode"));
		m_usbMode.setOptions(
			{_("settings.usb_mode_host"), _("settings.usb_mode_device"), _("settings.usb_mode_internal_wifi")});
		m_usbMode.setSelectedCallback([this](uint32_t index, std::string_view /* option */)
									  { getMainSettingsPresenter()->setUsbMode((UsbMode(index))); });
		m_usbMode.setSelected(StorageHelper::getData(ID_USB_MODE, 0));

		/* Brightness */
		m_brightness.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_brightness.setRange(0, 100);
		m_brightness.setLabel(_("settings.brightness"));
		m_brightness.setValue(DisplayHelper::getBrightness());
		m_brightness.setValueChangedCallback([](float value)
											 { DisplayHelper::setBrightness(static_cast<int32_t>(value)); });
		m_brightness.setSendMode(Slider::SendMode::VALUE_CHANGED);

		/* Screensaver Timeout */
		m_screensaverTimeout.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_screensaverTimeout.setLabel(_("settings.screensaver_timeout"));
		m_screensaverTimeout.setRange(0, 5 * 60); // seconds
		m_screensaverTimeout.setValueChangedCallback(
			[](float value) { StorageHelper::setData(ID_SCREENSAVER_TIMEOUT, static_cast<int32_t>(value * 1000)); });
		m_screensaverTimeout.setOutOfRangeMode(Slider::OutOfRange::UPPER);

		/* System Logging */
		m_systemLogging.setText(_("settings.system_logging"));
		m_systemLogging.setChecked(StorageHelper::getData(ID_ENABLE_UI_LOGGING, false));
		m_systemLogging.setCheckedCallback(
			[](bool checked)
			{
				StorageHelper::setData(ID_ENABLE_UI_LOGGING, checked);
				Log::EnableUiLogging(checked);
			});

		/* Display Connected Message */
		m_displayConnectedMessage.setText(_("settings.display_connected_message"));
		m_displayConnectedMessage.setChecked(StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true));
		m_displayConnectedMessage.setCheckedCallback(
			[](bool checked) { StorageHelper::setData(ID_DISPLAY_CONNECTED_MESSAGE, checked); });

		/* Notification Level */
		m_notificationLevel.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_notificationLevel.setLabel(_("settings.notification_level"));
		for (auto& level : RESPONSE_TYPE_STRINGS)
		{
			m_notificationLevel.addOption(_(level));
		}
		m_notificationLevel.setSelectedCallback([](uint32_t index, std::string_view /* option */)
												{ StorageHelper::setData(ID_NOTIFICATION_LEVEL, index); });

		/* Info Timeout */
		m_notificationTimeout.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_notificationTimeout.setLabel(_("settings.notification_timeout"));
		m_notificationTimeout.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_notificationTimeout.setRange(0, 5000);
		m_notificationTimeout.setValue(StorageHelper::getData(ID_NOTIFICATION_TIMEOUT, DEFAULT_NOTIFICATION_TIMEOUT));
		m_notificationTimeout.setValueChangedCallback(
			[](float value) { StorageHelper::setData(ID_NOTIFICATION_TIMEOUT, static_cast<uint32_t>(value)); });

		/* Auto-close Error Notifications */
		m_notificationAutoCloseError.setText(_("settings.notification_auto_close_error"));
		m_notificationAutoCloseError.setCheckedCallback(
			[](bool checked) { StorageHelper::setData(ID_NOTIFICATION_AUTO_CLOSE_ERROR, !checked); });
	}

	void ScreenSettingsView::onInit()
	{
		m_brightness.setNumberPad(&HomeView::instance().getNumberPad());
		m_screensaverTimeout.setNumberPad(&HomeView::instance().getNumberPad());
		m_notificationTimeout.setNumberPad(&HomeView::instance().getNumberPad());
	}

	void ScreenSettingsView::onShow()
	{
		UI_LOCK();
		m_usbMode.setSelected(StorageHelper::getData(ID_USB_MODE, 0));
		m_brightness.setValue(DisplayHelper::getBrightness());
		m_screensaverTimeout.setValue(StorageHelper::getData(ID_SCREENSAVER_TIMEOUT, DEFAULT_SCREEN_TIMEOUT) / 1000);
		m_notificationLevel.setSelected(StorageHelper::getData(ID_NOTIFICATION_LEVEL, DEFAULT_NOTIFICATION_LEVEL));
		m_notificationTimeout.setValue(StorageHelper::getData(ID_NOTIFICATION_TIMEOUT, DEFAULT_NOTIFICATION_TIMEOUT));
		m_notificationAutoCloseError.setChecked(
			!StorageHelper::getData(ID_NOTIFICATION_AUTO_CLOSE_ERROR, DEFAULT_NOTIFICATION_AUTO_CLOSE_ERROR));
	}

	ThemeSettingsView::ThemeSettingsView(LvObj& parent, SettingsView& mainSettingsView)
		: SettingsSubView("screen", parent, mainSettingsView)
		, m_theme("theme", getRoot(), layout_t(0, 0, 100, LV_SIZE_CONTENT))
		, m_themePreview("theme_demo", getRoot())
	{
		UI_LOCK();

		m_theme.setLabel(_("settings.theme"));
		for (auto& theme : Themes::getThemes())
		{
			m_theme.addOption(_(fmt::format("theme.id.{:s}", theme->getName())));
		}
		m_theme.setSelectedCallback(
			[this](uint32_t index, std::string_view /* option */)
			{
				auto theme = Themes::getTheme(index);
				if (theme == nullptr)
				{
					return;
				}
				theme->setThemeActive();
				updateThemePreview();
				StorageHelper::setData(ID_THEME, index);
				// view->getMainSettingsPresenter()->setTheme(selected);
			});
		m_theme.setSelected(StorageHelper::getData(ID_THEME, 0));
		m_themePreview.setSize(LV_PCT(100), LV_SIZE_CONTENT);
	}

	void ThemeSettingsView::updateThemePreview()
	{
		auto theme = Themes::getCurrentTheme();
		if (theme == nullptr)
		{
			return;
		}

		auto customTheme = dynamic_cast<const Themes::CustomTheme*>(theme);
		m_themePreview.showControls(customTheme != nullptr);
		if (customTheme != nullptr)
		{
			m_themePreview.updateSliders(customTheme->getPrimaryHue(),
										 customTheme->getSecondaryHue(),
										 customTheme->getChroma(),
										 customTheme->getDarkMode());
		}
		m_themePreview.updateSwatches();
	}

	void ThemeSettingsView::onInit()
	{
		m_themePreview.setNumberPad(&HomeView::instance().getNumberPad());
	}

	void ThemeSettingsView::onShow()
	{
		updateThemePreview();
	}

	NetworkSettingsView::NetworkSettingsView(LvObj& parent, SettingsView& mainSettingsView)
		: View("network_settings_view", parent, mainSettingsView)
		, m_topBar("top_bar", getRoot())
		, m_ipAddress("ip_address", m_topBar)
		, m_refresh("refresh", m_topBar, _("settings.refresh"), layout_t{0, 0, 0, LV_SIZE_CONTENT})
		, m_networkList(lv_table_create(getRootPtr()))
		, m_passwordWindow("password_msgbox", getRoot(), layout_t{0, 0, 80, LV_SIZE_CONTENT})
		, m_passwordInput("password_input", m_passwordWindow.getBody(), layout_t(0, 0, 80, LV_SIZE_CONTENT))
	{
		UI_LOCK();

		lv_obj_set_flex_flow(getRootPtr(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(getRootPtr(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

		m_topBar.setStylePad(2);
		m_topBar.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_topBar.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		m_topBar.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_ipAddress.setFlexGrow(3);
		m_refresh.setFlexGrow(1);
		m_ipAddress.setHeight(LV_SIZE_CONTENT);
		m_ipAddress.setText(_("settings.network_ip_address", ""));

		// Network List
		lv_obj_set_flex_flow(m_networkList, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_networkList, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		lv_obj_set_size(m_networkList, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_column(m_networkList, 5, 0);
		lv_obj_set_style_pad_row(m_networkList, 5, 0);
		lv_obj_set_style_pad_all(m_networkList, 5, 0);

		// Network Selection
		lv_obj_set_flex_grow(m_networkList, 1);
		lv_obj_set_width(m_networkList, LV_PCT(100));
		lv_table_set_column_count(m_networkList, 5);
		lv_table_set_cell_value(m_networkList, 0, 0, _("settings.network_ssid").c_str());
		lv_table_set_cell_value(m_networkList, 0, 1, _("settings.network_signal").c_str());
		lv_table_set_cell_value(m_networkList, 0, 2, _("settings.network_known").c_str());
		lv_table_set_cell_value(m_networkList, 0, 3, _("settings.network_forget").c_str());
		lv_table_set_cell_value(m_networkList, 0, 4, _("settings.network_connected").c_str());

		// Password Window
		m_passwordWindow.hide();
		m_passwordWindow.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_passwordWindow.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_passwordWindow.setSize(LV_PCT(80), LV_SIZE_CONTENT);
		m_passwordWindow.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_passwordWindow.setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_passwordWindow.setTitle(_("settings.network_password_title"));
		m_passwordWindow.setText("");
		m_passwordWindow.okVisible(true);
		m_passwordWindow.cancelVisible(true);
		m_passwordWindow.setOkCallback([this]() { onPasswordConfirmEvent(); });
		m_passwordWindow.setCloseCallback([this]() { onPasswordCloseEvent(); });
		m_passwordInput.setPlaceholderText(_("settings.network_enter_password"));
		m_passwordInput.setPasswordMode(true);
		m_passwordInput.setOneLine(true);

		// Refresh
		m_refresh.addClickedCallback(onRefreshEvent, this);

		// Callbacks
		lv_obj_add_event_cb(m_networkList, onNetworkSelectionEvent, LV_EVENT_VALUE_CHANGED, this);
	}

	void NetworkSettingsView::setIpAddress(const std::string& ipAddress)
	{
		UI_LOCK();
		m_ipAddress.setText(_("settings.network_ip_address", ipAddress));
	}

	void NetworkSettingsView::setEnabled(bool /* enabled */)
	{
		UI_LOCK();
	}

	void NetworkSettingsView::setNetworkCount(size_t count)
	{
		UI_LOCK();
		lv_table_set_row_count(m_networkList, count + 1);
	}

	void NetworkSettingsView::setNetworkDetails(
		size_t index, const std::string& ssid, int32_t signalLevel, bool known, bool connected)
	{
		UI_LOCK();
		void* knownPtr = lv_malloc(sizeof(bool));
		*(bool*)knownPtr = known;
		lv_table_set_cell_user_data(m_networkList, index + 1, 2, knownPtr);
		lv_table_set_cell_value(m_networkList, index + 1, 0, ssid.c_str());
		lv_table_set_cell_value(m_networkList, index + 1, 1, fmt::format("{:d} dBm", signalLevel).c_str());
		lv_table_set_cell_value(m_networkList, index + 1, 2, known ? LV_SYMBOL_OK : LV_SYMBOL_CLOSE);
		lv_table_set_cell_value(m_networkList, index + 1, 3, known ? LV_SYMBOL_TRASH : "");
		lv_table_set_cell_value(m_networkList, index + 1, 4, connected ? LV_SYMBOL_WIFI : "");
	}

	void NetworkSettingsView::onNetworkSelectionEvent(lv_event_t* e)
	{
		UI_LOCK();

		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		lv_obj_t* table = (lv_obj_t*)lv_event_get_target(e);
		uint32_t row;
		uint32_t col;
		lv_table_get_selected_cell(view->m_networkList, &row, &col);

		view->getMainSettingsView().showKeyboard(false);

		if (row == 0)
		{
			return;
		}

		if (col == 3)
		{
			view->getPresenter()->forgetNetwork(lv_table_get_cell_value(table, row, 0));
			return;
		}

		const char* ssid = lv_table_get_cell_value(table, row, 0);
		bool known = *(bool*)lv_table_get_cell_user_data(table, row, 2);
		if (!known)
		{
			view->m_passwordInput.setText("");
			view->m_passwordInput.showPassword(false);
			view->m_passwordWindow.setText(ssid);
			view->getMainSettingsView().showKeyboard(
				true, LV_KEYBOARD_MODE_TEXT_LOWER, &view->m_passwordInput.getTextArea());
			openModal(&view->m_passwordWindow);
			return;
		}

		view->getPresenter()->connectToNetwork(ssid);
	}

	void NetworkSettingsView::onPasswordCloseEvent()
	{
		UI_LOCK();
		getMainSettingsView().showKeyboard(false);
	}

	void NetworkSettingsView::onPasswordConfirmEvent()
	{
		UI_LOCK();
		getPresenter()->connectToNetwork(m_passwordWindow.getText().getText(), m_passwordInput.getText());
	}

	void NetworkSettingsView::onRefreshEvent(lv_event_t* e)
	{
		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		view->getPresenter()->scanWifi();
	}

	void NetworkSettingsView::onShow()
	{
		getPresenter()->scanWifi();
		m_passwordWindow.close();
	}

	void NetworkSettingsView::onHide()
	{
		m_passwordWindow.close();
	}

	DeveloperSettingsView::DeveloperSettingsView(LvObj& parent, SettingsView& mainSettingsView)
		: SettingsSubView("developer_settings_view", parent, mainSettingsView)
		, m_debugLevelCont(lv_obj_create(getRootPtr()))
		, m_debugLevelLabel(lv_label_create(m_debugLevelCont))
		, m_debugLevel(lv_dropdown_create(m_debugLevelCont))
#if DEBUG_BORDERS
		, m_debugBorders(lv_checkbox_create(getRootPtr()))
#endif
		, m_enableSSH(lv_checkbox_create(getRootPtr()))
		, m_restart("developer_settings_restart", getRoot(), _("settings.restart"))
		, m_eraseAndRestart("developer_settings_erase_and_restart", getRoot(), _("settings.erase_and_restart"))
		, m_reboot("developer_settings_reboot", getRoot(), _("settings.reboot"))
		, m_startHardwareTest("start_hardware_test", getRoot())
	{
		UI_LOCK();

		// Debug Level
		lv_obj_set_flex_flow(m_debugLevelCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_size(m_debugLevelCont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_column(m_debugLevelCont, 5, 0);
		lv_label_set_text(m_debugLevelLabel, _("settings.debug_level").c_str());
		std::string options;
		for (const auto& level : Log::DebugLevelStrings)
		{
			options += _(level);
			options += "\n";
		}
		lv_dropdown_set_options(m_debugLevel, options.c_str());
		lv_dropdown_set_selected(m_debugLevel, static_cast<uint32_t>(Log::GetDebugLevel()));
		lv_dropdown_set_selected_highlight(m_debugLevel, true);
		lv_obj_add_event_cb(m_debugLevel, onDebugLevelEvent, LV_EVENT_VALUE_CHANGED, NULL);

#if DEBUG_BORDERS
		lv_checkbox_set_text(m_debugBorders, _("settings.debug_borders").c_str());
		lv_obj_set_state(m_debugBorders, LV_STATE_CHECKED, Themes::isdebugBorderVisible(lv_screen_active()));
		lv_obj_add_event_cb(m_debugBorders, onDebugBordersEvent, LV_EVENT_VALUE_CHANGED, this);
#endif

		lv_checkbox_set_text(m_enableSSH, _("settings.enable_ssh").c_str());
		lv_obj_set_state(m_enableSSH, LV_STATE_CHECKED, StorageHelper::getData<bool>(ID_SSH_ENABLED, false));
		lv_obj_add_event_cb(m_enableSSH, onEnableSSHEvent, LV_EVENT_VALUE_CHANGED, this);

#if LV_USE_SYSMON
		m_enableSystemMonitor.setText(_("settings.enable_system_monitor"));
		m_enableSystemMonitor.setChecked(StorageHelper::getData<bool>(ID_SYSTEM_MONITOR_ENABLED, true));
		m_enableSystemMonitor.addEventCallback(
			[this](lv_event_t*)
			{
				bool checked = m_enableSystemMonitor.hasState(LV_STATE_CHECKED);
				StorageHelper::setData<bool>(ID_SYSTEM_MONITOR_ENABLED, checked);

#  if LV_USE_PERF_MONITOR
				if (checked)
					lv_sysmon_show_performance(NULL);
				else
					lv_sysmon_hide_performance(NULL);
#  endif
#  if LV_USE_MEM_MONITOR
				if (checked)
					lv_sysmon_show_memory(NULL);
				else
					lv_sysmon_hide_memory(NULL);
#  endif
			},
			LV_EVENT_VALUE_CHANGED);
		m_enableSystemMonitor.sendEvent(LV_EVENT_VALUE_CHANGED);
#endif

		// Power
		m_restart.setHeight(LV_SIZE_CONTENT);
		m_eraseAndRestart.setHeight(LV_SIZE_CONTENT);
		m_reboot.setHeight(LV_SIZE_CONTENT);
		m_restart.addClickedCallback(onRestartEvent, this);
		m_eraseAndRestart.addClickedCallback(onEraseAndRestartEvent, this);
		m_reboot.addClickedCallback(onRebootEvent, this);

		/* Hardware Test */
		m_startHardwareTest.setText(_("settings.start_hardware_test"));
		m_startHardwareTest.addClickedCallback(
			[](lv_event_t* e)
			{
				auto& view = *static_cast<DeveloperSettingsView*>(lv_event_get_user_data(e));
				view.getMainSettingsPresenter()->startHardwareTest();
			},
			this);
	}

	void DeveloperSettingsView::onDebugLevelEvent(lv_event_t* e)
	{
		UI_LOCK();
		lv_obj_t* dropdown = (lv_obj_t*)lv_event_get_target(e);
		size_t lvl = lv_dropdown_get_selected(dropdown);
		Log::SetDebugLevel(static_cast<Log::DebugLevel>(lvl));
	}

#if DEBUG_BORDERS
	void DeveloperSettingsView::onDebugBordersEvent(lv_event_t* e)
	{
		UI_LOCK();
		lv_obj_t* cb = (lv_obj_t*)lv_event_get_target(e);
		bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
		StorageHelper::setData<bool>(ID_DEBUG_BORDERS, checked);
		Themes::showDebugBorders(lv_screen_active(), checked);
	}
#endif

	void DeveloperSettingsView::onEnableSSHEvent(lv_event_t* e)
	{
		UI_LOCK();
		lv_obj_t* cb = (lv_obj_t*)lv_event_get_target(e);
		bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
		LOG_INFO("{:s} SSH", checked ? "Enabling" : "Disabling");
		StorageHelper::setData<bool>(ID_SSH_ENABLED, checked);
		if (checked)
		{
#if !SIMULATION
			if (system("mv /etc/init.d/50dropbear /etc/init.d/S50dropbear;"
					   "/etc/init.d/S50dropbear start") != 0)
			{
				LOG_ERROR("Failed to enable SSH server");
				return;
			}
#endif
		}
		else
		{
#if !SIMULATION
			if (system("/etc/init.d/S50dropbear stop;"
					   "mv /etc/init.d/S50dropbear /etc/init.d/50dropbear") != 0)
			{
				LOG_ERROR("Failed to disable SSH server");
				return;
			}
#endif
		}
	}

	void DeveloperSettingsView::onRestartEvent(lv_event_t* /* e */)
	{
		UI_LOCK();
		Restart();
	}

	void DeveloperSettingsView::onEraseAndRestartEvent(lv_event_t* /* e */)
	{
		UI_LOCK();
		EraseAndRestart();
	}

	void DeveloperSettingsView::onRebootEvent(lv_event_t* /* e */)
	{
		UI_LOCK();
		Reboot();
	}
} // namespace UI
