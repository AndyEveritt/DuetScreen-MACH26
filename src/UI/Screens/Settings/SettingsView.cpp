#include "SettingsView.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "Hardware/Reset.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/DisplayHelper.h"
#include "utils/StorageHelper.h"

namespace UI
{
	SettingsView::SettingsView(lv_obj_t* parent)
		: View("settings_view", parent, layout_t(0, 0, 100, 100))
		, m_settingsList(lv_list_create(getCont()))
		, m_subWindow(lv_obj_create(getCont()))
		, m_connectivityHeader(lv_list_add_text(m_settingsList, _("settings_connectivity_header")))
		, m_duetSettings(lv_list_add_button(m_settingsList, NULL, _("settings_duet")))
		, m_deviceSettings(lv_list_add_button(m_settingsList, NULL, _("settings_device")))
		, m_networkSettings(lv_list_add_button(m_settingsList, LV_SYMBOL_WIFI, _("settings_network")))
		, m_devHeader(lv_list_add_text(m_settingsList, _("settings_dev_header")))
		, m_developerSettings(lv_list_add_button(m_settingsList, LV_SYMBOL_SETTINGS, _("settings_developer")))
		, m_duetSettingsView(m_subWindow, this)
		, m_deviceSettingsView(m_subWindow, this)
		, m_networkSettingsView(m_subWindow, this)
		, m_developerSettingsView(m_subWindow, this)
		, m_currentSubView(&m_duetSettingsView)
		, m_keyboard(lv_keyboard_create(getCont()))
	{
		Lock lock;
		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_layoutColDsc, m_layoutRowDsc);
		lv_obj_set_grid_cell(m_settingsList, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_subWindow, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_keyboard, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);

		// List
		lv_obj_set_user_data(m_duetSettings, &m_duetSettingsView);
		lv_obj_set_user_data(m_deviceSettings, &m_deviceSettingsView);
		lv_obj_set_user_data(m_networkSettings, &m_networkSettingsView);
		lv_obj_set_user_data(m_developerSettings, &m_developerSettingsView);

		lv_obj_add_event_cb(m_duetSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_deviceSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_networkSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_developerSettings, onWindowSelectEvent, LV_EVENT_CLICKED, this);
	}

	void SettingsView::onWindowSelectEvent(lv_event_t* e)
	{
		Lock lock;
		SettingsView* view = (SettingsView*)lv_event_get_user_data(e);
		BaseView* subView = (BaseView*)lv_obj_get_user_data(lv_event_get_target_obj(e));
		BaseView* currentSubView = view->m_currentSubView;

		if (currentSubView == subView)
		{
			return;
		}

		if (currentSubView != nullptr)
		{
			currentSubView->hide();
		}

		view->showKeyboard(false);
		subView->show();
		view->m_currentSubView = subView;
	}

	void SettingsView::showKeyboard(bool show, lv_keyboard_mode_t mode, lv_obj_t* textArea)
	{
		Lock lock;
		if (show)
		{
			m_layoutRowDsc[1] = LV_GRID_FR(1);
			lv_keyboard_set_mode(m_keyboard, mode);
			lv_obj_remove_flag(m_keyboard, LV_OBJ_FLAG_HIDDEN);
			setKeyboardTextArea(textArea);
		}
		else
		{
			m_layoutRowDsc[1] = 0;
			setKeyboardTextArea(NULL);
			lv_obj_add_flag(m_keyboard, LV_OBJ_FLAG_HIDDEN);
		}
	}

	void SettingsView::setKeyboardTextArea(lv_obj_t* textArea)
	{
		Lock lock;
		lv_keyboard_set_textarea(m_keyboard, textArea);
	}

	bool SettingsView::back()
	{
		Lock lock;
		if (!lv_obj_has_flag(m_keyboard, LV_OBJ_FLAG_HIDDEN))
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
		m_currentSubView->show();
	}

	SettingsSubView::SettingsSubView(const std::string& name, lv_obj_t* parent, SettingsView* mainSettingsView)
		: BaseView(name, parent, layout_t(0, 0, 100, 100))
		, m_mainSettingsView(mainSettingsView)
	{
		Lock lock;
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN_WRAP);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	}

	SettingsPresenter& SettingsSubView::getMainSettingsPresenter() const
	{
		return m_mainSettingsView->getPresenter();
	}

	void SettingsSubView::onTextAreaEvent(lv_event_t* e)
	{
		Lock lock;
		lv_event_code_t code = lv_event_get_code(e);
		lv_obj_t* ta = (lv_obj_t*)lv_event_get_target(e);
		SettingsSubView* view = (SettingsSubView*)lv_event_get_user_data(e);
		lv_keyboard_mode_t mode =
			lv_textarea_get_accepted_chars(ta) == "0123456789" ? LV_KEYBOARD_MODE_NUMBER : LV_KEYBOARD_MODE_TEXT_LOWER;
		if (code == LV_EVENT_FOCUSED)
		{
			view->getMainSettingsView()->showKeyboard(true, mode, ta);
		}

		if (code == LV_EVENT_DEFOCUSED)
		{
			view->getMainSettingsView()->showKeyboard(false);
		}
	}

	DuetSettingsView::DuetSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView)
		: SettingsSubView("duet_settings_view", parent, mainSettingsView)
		, m_connectionMethod(lv_dropdown_create(getCont()))
		, m_hostname(lv_textarea_create(getCont()))
		, m_password(lv_textarea_create(getCont()))
		, m_pollInterval(lv_textarea_create(getCont()))
		, m_infoTimeout(lv_textarea_create(getCont()))
		, m_save("duet_settings_save", getCont(), _("save"))
	{
		Lock lock;

		// Connection Method
		std::string options;
		for (const auto& method : Comm::duetCommunicationTypeNames)
		{
			options += _(method);
			options += "\n";
		}
		lv_dropdown_set_options(m_connectionMethod, options.c_str());
		lv_dropdown_set_selected(m_connectionMethod, (uint32_t)Comm::DUET.GetCommunicationType());

		// Hostname
		lv_textarea_set_one_line(m_hostname, true);
		lv_textarea_set_placeholder_text(m_hostname, _("settings_duet_hostname"));
		lv_textarea_set_text(m_hostname, Comm::DUET.GetHostname().c_str());
		lv_obj_add_event_cb(m_hostname, onTextAreaEvent, LV_EVENT_ALL, this);

		// Password
		lv_textarea_set_one_line(m_password, true);
		lv_textarea_set_placeholder_text(m_password, _("settings_duet_password"));
		lv_textarea_set_password_mode(m_password, true);
		lv_textarea_set_text(m_password, Comm::DUET.GetPassword().c_str());
		lv_obj_add_event_cb(m_password, onTextAreaEvent, LV_EVENT_ALL, this);

		// Poll Interval
		lv_textarea_set_one_line(m_pollInterval, true);
		lv_textarea_set_placeholder_text(m_pollInterval, _("settings_duet_poll_interval"));
		lv_textarea_set_accepted_chars(m_pollInterval, "0123456789");
		lv_textarea_set_text(m_pollInterval, utils::format("%u", Comm::DUET.GetPollInterval()).c_str());
		lv_obj_add_event_cb(m_pollInterval, onTextAreaEvent, LV_EVENT_ALL, this);

		// Info Timeout
		lv_textarea_set_one_line(m_infoTimeout, true);
		lv_textarea_set_placeholder_text(m_infoTimeout, _("settings_duet_info_timeout"));
		lv_textarea_set_accepted_chars(m_infoTimeout, "0123456789");
		lv_textarea_set_text(
			m_infoTimeout, utils::format("%u", StorageHelper::getData(ID_INFO_TIMEOUT, DEFAULT_POPUP_TIMEOUT)).c_str());
		lv_obj_add_event_cb(m_infoTimeout, onTextAreaEvent, LV_EVENT_ALL, this);

		// Save
		lv_obj_set_height(m_save.getCont(), LV_SIZE_CONTENT);
		m_save.setCallback(onSaveEvent, LV_EVENT_CLICKED, this);
	}

	void DuetSettingsView::onSaveEvent(lv_event_t* e)
	{
		Lock lock;
		DuetSettingsView* view = (DuetSettingsView*)lv_event_get_user_data(e);

		Comm::DUET.SetCommunicationType((Comm::CommunicationType)lv_dropdown_get_selected(view->m_connectionMethod));
		Comm::DUET.SetHostname(lv_textarea_get_text(view->m_hostname));
		Comm::DUET.SetPassword(lv_textarea_get_text(view->m_password));
		Comm::DUET.SetPollInterval(atoi(lv_textarea_get_text(view->m_pollInterval)));
		StorageHelper::setData(ID_INFO_TIMEOUT, (uint32_t)atoi(lv_textarea_get_text(view->m_infoTimeout)));
	}

	DeviceSettingsView::DeviceSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView)
		: SettingsSubView("device_settings_view", parent, mainSettingsView)
		, m_brightness("settings_brightness", getCont(), layout_t(0, 0, 100, LV_SIZE_CONTENT))
		, m_screensaverTimeout("settings_screensaver_timeout", getCont(), layout_t(0, 0, 100, LV_SIZE_CONTENT))
	{
		Lock lock;

		// Brightness
		m_brightness.setRange(0, 100);
		m_brightness.setLabel(_("settings_brightness"));
		m_brightness.setValue(DisplayHelper::getBrightness());
		m_brightness.setValueChangedCallback([](uint32_t value) { DisplayHelper::setBrightness(value); });
		m_brightness.setSendMode(Slider::SendMode::VALUE_CHANGED);

		// Screensaver Timeout
		m_screensaverTimeout.setLabel(_("settings_screensaver_timeout"));
		m_screensaverTimeout.setRange(0, 30 * 60); // seconds
		m_screensaverTimeout.setValueChangedCallback([](uint32_t value)
													 { StorageHelper::setData(ID_SCREENSAVER_TIMEOUT, value * 1000); });
		m_screensaverTimeout.setOutOfRangeMode(Slider::OutOfRange::UPPER);
	}

	void DeviceSettingsView::onShow()
	{
		m_brightness.setValue(DisplayHelper::getBrightness());
		m_screensaverTimeout.setValue(StorageHelper::getData(ID_SCREENSAVER_TIMEOUT, DEFAULT_SCREEN_TIMEOUT) / 1000);
	}

	NetworkSettingsView::NetworkSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView)
		: View("network_settings_view", parent)
		, m_topBar(lv_obj_create(getCont()))
		, m_ipAddress(lv_label_create(m_topBar))
		, m_enable(lv_checkbox_create(m_topBar))
		, m_refresh("network_settings_refresh", m_topBar, _("refresh"), layout_t{0, 0, 0, LV_SIZE_CONTENT})
		, m_networkList(lv_table_create(getCont()))
		, m_passwordWindow(lv_msgbox_create(getCont()))
		, m_passwordInput(lv_textarea_create(m_passwordWindow))
		, m_passwordSsid(nullptr)
	{
		Lock lock;
		setMainSettingsView(mainSettingsView);

		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

		lv_obj_set_style_pad_all(m_topBar, 2, 0);
		lv_obj_set_flex_flow(m_topBar, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_topBar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		lv_obj_set_size(m_topBar, LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_flex_grow(m_ipAddress, 3);
		lv_obj_set_flex_grow(m_refresh.getCont(), 1);
		lv_obj_set_height(m_ipAddress, LV_SIZE_CONTENT);
		lv_obj_set_size(m_enable, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_label_set_text(m_ipAddress, utils::format(_("settings_network_ip_address")).c_str());
		lv_checkbox_set_text(m_enable, _("settings_network_enable"));

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
		lv_table_set_cell_value(m_networkList, 0, 0, _("settings_network_ssid"));
		lv_table_set_cell_value(m_networkList, 0, 1, _("settings_network_signal"));
		lv_table_set_cell_value(m_networkList, 0, 2, _("settings_network_known"));
		lv_table_set_cell_value(m_networkList, 0, 3, _("settings_network_forget"));
		lv_table_set_cell_value(m_networkList, 0, 4, _("settings_network_connected"));

		// Password Window
		lv_obj_add_flag(m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(m_passwordWindow, LV_OBJ_FLAG_FLOATING);
		lv_obj_align(m_passwordWindow, LV_ALIGN_CENTER, 0, 0);
		lv_obj_set_size(m_passwordWindow, LV_PCT(80), LV_SIZE_CONTENT);
		lv_obj_set_flex_flow(m_passwordWindow, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_passwordWindow, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_width(m_passwordInput, LV_PCT(80));

		lv_msgbox_add_title(m_passwordWindow, _("settings_network_password_title"));
		m_passwordSsid = lv_msgbox_add_text(m_passwordWindow, "");
		lv_obj_t* closeBtn = lv_msgbox_add_header_button(m_passwordWindow, LV_SYMBOL_CLOSE);
		lv_obj_t* confirmBtn = lv_msgbox_add_footer_button(m_passwordWindow, LV_SYMBOL_OK);
		lv_textarea_set_placeholder_text(m_passwordInput, _("settings_network_enter_password"));
		lv_textarea_set_one_line(m_passwordInput, true);
		lv_textarea_set_password_mode(m_passwordInput, true);

		// Refresh
		m_refresh.setCallback(onRefreshEvent, LV_EVENT_CLICKED, this);

		// Callbacks
		lv_obj_add_event_cb(m_enable, onEnableEvent, LV_EVENT_VALUE_CHANGED, this);
		lv_obj_add_event_cb(m_networkList, onNetworkSelectionEvent, LV_EVENT_VALUE_CHANGED, this);
		lv_obj_add_event_cb(closeBtn, onPasswordCloseEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(confirmBtn, onPasswordConfirmEvent, LV_EVENT_CLICKED, this);
		lv_obj_add_event_cb(m_passwordWindow, onPasswordCloseEvent, LV_EVENT_DEFOCUSED, this);
	}

	void NetworkSettingsView::setIpAddress(const std::string& ipAddress)
	{
		lv_label_set_text(m_ipAddress, utils::format(_("settings_network_ip_address"), ipAddress.c_str()).c_str());
	}

	void NetworkSettingsView::setEnabled(bool enabled)
	{
		lv_obj_set_state(m_enable, LV_STATE_CHECKED, enabled);
	}

	void NetworkSettingsView::setNetworkCount(size_t count)
	{
		lv_table_set_row_count(m_networkList, count + 1);
	}

	void NetworkSettingsView::setNetworkDetails(
		size_t index, const std::string& ssid, int32_t signalLevel, bool known, bool connected)
	{
		void* knownPtr = lv_malloc(sizeof(bool));
		*(bool*)knownPtr = known;
		lv_table_set_cell_user_data(m_networkList, index + 1, 2, knownPtr);
		lv_table_set_cell_value(m_networkList, index + 1, 0, ssid.c_str());
		lv_table_set_cell_value(m_networkList, index + 1, 1, utils::format("%d dBm", signalLevel).c_str());
		lv_table_set_cell_value(m_networkList, index + 1, 2, known ? LV_SYMBOL_OK : LV_SYMBOL_CLOSE);
		lv_table_set_cell_value(m_networkList, index + 1, 3, known ? LV_SYMBOL_TRASH : "");
		lv_table_set_cell_value(m_networkList, index + 1, 4, connected ? LV_SYMBOL_WIFI : "");
	}

	void NetworkSettingsView::onEnableEvent(lv_event_t* e)
	{
		Lock lock;
		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		view->getPresenter().setWifiEnabled(lv_obj_has_state(view->m_enable, LV_STATE_CHECKED));
	}

	void NetworkSettingsView::onNetworkSelectionEvent(lv_event_t* e)
	{
		Lock lock;

		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		lv_obj_t* table = (lv_obj_t*)lv_event_get_target(e);
		uint32_t row;
		uint32_t col;
		lv_table_get_selected_cell(view->m_networkList, &row, &col);

		view->getMainSettingsView()->showKeyboard(false);

		if (row == 0)
		{
			return;
		}

		if (col == 3)
		{
			view->getPresenter().forgetNetwork(lv_table_get_cell_value(table, row, 0));
			return;
		}

		const char* ssid = lv_table_get_cell_value(table, row, 0);
		bool known = *(bool*)lv_table_get_cell_user_data(table, row, 2);
		if (!known)
		{
			lv_textarea_set_text(view->m_passwordInput, "");
			lv_label_set_text(view->m_passwordSsid, ssid);
			view->getMainSettingsView()->showKeyboard(true, LV_KEYBOARD_MODE_TEXT_LOWER, view->m_passwordInput);
			lv_obj_remove_flag(view->m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
			return;
		}

		view->getPresenter().connectToNetwork(ssid);
	}

	void NetworkSettingsView::onPasswordCloseEvent(lv_event_t* e)
	{
		Lock lock;
		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		view->getMainSettingsView()->showKeyboard(false);
		lv_obj_add_flag(view->m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
	}

	void NetworkSettingsView::onPasswordConfirmEvent(lv_event_t* e)
	{
		Lock lock;
		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		view->getMainSettingsView()->showKeyboard(false);
		lv_obj_add_flag(view->m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
		view->getPresenter().connectToNetwork(lv_label_get_text(view->m_passwordSsid),
											  lv_textarea_get_text(view->m_passwordInput));
	}

	void NetworkSettingsView::onRefreshEvent(lv_event_t* e)
	{
		Lock lock;
		NetworkSettingsView* view = (NetworkSettingsView*)lv_event_get_user_data(e);
		lv_obj_add_flag(view->m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
		view->getPresenter().scanWifi();
	}

	void NetworkSettingsView::onShow()
	{
		lv_obj_add_flag(m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
		getPresenter().scanWifi();
	}

	void NetworkSettingsView::onHide()
	{
		lv_obj_add_flag(m_passwordWindow, LV_OBJ_FLAG_HIDDEN);
	}

	DeveloperSettingsView::DeveloperSettingsView(lv_obj_t* parent, SettingsView* mainSettingsView)
		: SettingsSubView("developer_settings_view", parent, mainSettingsView)
		, m_debugLevelCont(lv_obj_create(getCont()))
		, m_debugLevelLabel(lv_label_create(m_debugLevelCont))
		, m_debugLevel(lv_dropdown_create(m_debugLevelCont))
#if DEBUG_BORDERS
		, m_debugBorders(lv_checkbox_create(getCont()))
#endif
		, m_enableSSH(lv_checkbox_create(getCont()))
		, m_restart("developer_settings_restart", getCont(), _("settings_restart"))
		, m_eraseAndRestart("developer_settings_erase_and_restart", getCont(), _("settings_erase_and_restart"))
		, m_reboot("developer_settings_reboot", getCont(), _("settings_reboot"))
	{
		Lock lock;

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

		lv_checkbox_set_text(m_enableSSH, _("settings_enable_ssh"));
		lv_obj_set_state(m_enableSSH, LV_STATE_CHECKED, StorageHelper::getData<bool>(ID_SSH_ENABLED, false));
		lv_obj_add_event_cb(m_enableSSH, onEnableSSHEvent, LV_EVENT_VALUE_CHANGED, this);

		// Power
		lv_obj_set_height(m_restart.getCont(), LV_SIZE_CONTENT);
		lv_obj_set_height(m_eraseAndRestart.getCont(), LV_SIZE_CONTENT);
		lv_obj_set_height(m_reboot.getCont(), LV_SIZE_CONTENT);
		m_restart.setCallback(onRestartEvent, LV_EVENT_CLICKED, this);
		m_eraseAndRestart.setCallback(onEraseAndRestartEvent, LV_EVENT_CLICKED, this);
		m_reboot.setCallback(onRebootEvent, LV_EVENT_CLICKED, this);
	}

	void DeveloperSettingsView::onDebugLevelEvent(lv_event_t* e)
	{
		Lock lock;
		lv_obj_t* dropdown = (lv_obj_t*)lv_event_get_target(e);
		size_t lvl = lv_dropdown_get_selected(dropdown);
		SetDebugLevel(static_cast<DebugLevel>(lvl));
	}

#if DEBUG_BORDERS
	void DeveloperSettingsView::onDebugBordersEvent(lv_event_t* e)
	{
		Lock lock;
		lv_obj_t* cb = (lv_obj_t*)lv_event_get_target(e);
		bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
		StorageHelper::setData<bool>(ID_DEBUG_BORDERS, checked);
		Styles::instance().showDebugBorders(lv_screen_active(), checked);
	}
#endif

	void DeveloperSettingsView::onEnableSSHEvent(lv_event_t* e)
	{
		Lock lock;
		lv_obj_t* cb = (lv_obj_t*)lv_event_get_target(e);
		bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
		info("%s SSH", checked ? "Enabling" : "Disabling");
		StorageHelper::setData<bool>(ID_SSH_ENABLED, checked);
		if (checked)
		{
#if !SIMULATION
			system("mv /etc/init.d/50dropbear /etc/init.d/S50dropbear;"
				   "/etc/init.d/S50dropbear start");
#endif
		}
		else
		{
#if !SIMULATION
			system("/etc/init.d/S50dropbear stop;"
				   "mv /etc/init.d/S50dropbear /etc/init.d/50dropbear");
#endif
		}
	}

	void DeveloperSettingsView::onRestartEvent(lv_event_t* e)
	{
		Lock lock;
		Restart();
	}

	void DeveloperSettingsView::onEraseAndRestartEvent(lv_event_t* e)
	{
		Lock lock;
		EraseAndRestart();
	}

	void DeveloperSettingsView::onRebootEvent(lv_event_t* e)
	{
		Lock lock;
		Reboot();
	}
} // namespace UI
