#include "SettingsView.h"
#include "BuildDate.h"
#include "Comm/Usb.h"
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

#define USE_MODAL_NUMBERPAD_FOR_IP_ADDRESS 1

namespace UI
{
	static void onTextareaEvent(lv_event_t* e, TextBox& text_box, lv_keyboard_mode_t mode);

	static void setSliderNumberpadLabel(Slider& slider, std::string_view label)
	{
		slider.setLabel(label);
		slider.getLabel().hide();
	}

	SettingsView::SettingsView(const std::string& name, LvObj& parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);

		m_tabs.setSize(LV_PCT(100), LV_PCT(100));
	}

	void SettingsView::setKeyboard(LvKeyboard* keyboard)
	{
		m_keyboard = keyboard;
		m_connectionSettings.setKeyboard(m_keyboard);
	}

	bool SettingsView::back()
	{
		return false;
	}

	void SettingsView::onShow() {}

	void SettingsView::onHide() {}

	SettingsTab::SettingsTab(const std::string& name, LvObj& parent)
		: LvContainer(name, parent, layout_t(0, 0, 100, 100))
		, m_colDsc({LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST})
	{
		addStyle(Themes::getLvglStyles().card);

		m_rowDsc.at(1) = LV_GRID_TEMPLATE_LAST;
		setGridDsc(m_colDsc, m_rowDsc);
	}

	void SettingsTab::createHeader(std::string_view text)
	{
		if (m_rowCount >= m_maxRowCount)
		{
			LOG_FATAL_THROW("Maximum row count exceeded");
		}

		m_rowDsc.at(m_rowCount) = LV_GRID_CONTENT;
		auto lv_label = std::make_unique<LvLabel>(fmt::format("row_label_{:d}", m_rowCount), getRoot());
		lv_label->setText(text);
		lv_label->addStyle(Themes::getLvglStyles().text_emphasis);
		m_labels.at(m_rowCount) = std::move(lv_label);

		setGridCell(*m_labels.at(m_rowCount),
					LV_GRID_ALIGN_START,
					0,
					1,
					LV_GRID_ALIGN_CENTER,
					static_cast<int32_t>(m_rowCount),
					1);

		m_rowCount++;
		m_rowDsc.at(m_rowCount) = LV_GRID_TEMPLATE_LAST;

		updateLayout();
	}

	void SettingsTab::createRow(std::string_view label, LvObj& obj)
	{
		if (m_rowCount >= m_maxRowCount)
		{
			LOG_FATAL_THROW("Maximum row count exceeded");
		}

		m_rowDsc.at(m_rowCount) = LV_GRID_CONTENT;
		auto lv_label = std::make_unique<LvLabel>(fmt::format("row_label_{:d}", m_rowCount), getRoot());
		lv_label->setText(label);
		m_labels.at(m_rowCount) = std::move(lv_label);

		setGridCell(*m_labels.at(m_rowCount),
					LV_GRID_ALIGN_END,
					0,
					1,
					LV_GRID_ALIGN_CENTER,
					static_cast<int32_t>(m_rowCount),
					1);
		setGridCell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, static_cast<int32_t>(m_rowCount), 1);

		m_rowCount++;
		m_rowDsc.at(m_rowCount) = LV_GRID_TEMPLATE_LAST;

		updateLayout();
	}

	void SettingsTab::createSpanRow(LvObj& obj)
	{
		if (m_rowCount >= m_maxRowCount)
		{
			LOG_FATAL_THROW("Maximum row count exceeded");
		}

		m_rowDsc.at(m_rowCount) = LV_GRID_CONTENT;
		setGridCell(obj, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, static_cast<int32_t>(m_rowCount), 1);

		m_rowCount++;
		m_rowDsc.at(m_rowCount) = LV_GRID_TEMPLATE_LAST;

		updateLayout();
	}

	void SettingsTab::setRowVisibility(LvObj& obj, bool show)
	{
		UI_LOCK();
		int32_t row = lv_obj_get_style_grid_cell_row_pos(obj.getRootPtr(), LV_PART_MAIN);
		if (row < 0 || row >= static_cast<int32_t>(m_rowCount))
		{
			LOG_ERROR("Invalid obj");
			return;
		}

		if (auto& label = m_labels.at(row))
		{
			label->setVisible(show);
		}
		obj.setVisible(show);
	}

	GeneralSettings::GeneralSettings(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		/* Firmware version */
		createRow(_("settings.firmware_version"), m_buildTime);
#if USE_FIXED_TEST_BUILD_TIME
		m_buildTime.setText(_("settings.build_time", "vX.Y.Z", "YYYY-MM-DD", "HH:mm::ss"));
#else
		m_buildTime.setText(_("settings.build_time", FIRMWARE_VERSION, BuildDateText, BuildTimeSuffix));
#endif

		/* Language */
		createRow(_("settings.language"), m_language);
		m_language.setHeight(LV_SIZE_CONTENT);
		{
			std::array languages = {_("settings.language_en")};
			m_language.setOptions(languages);
		}

		/* Brightness */
		createRow(_("settings.brightness"), m_brightness);
		setSliderNumberpadLabel(m_brightness, _("settings.brightness"));
		m_brightness.setHeight(LV_SIZE_CONTENT);
		m_brightness.setRange(0, 100);
		m_brightness.setValueChangedCallback([](float value)
											 { DisplayHelper::setBrightness(static_cast<int32_t>(value)); });
		m_brightness.setSendMode(Slider::SendMode::VALUE_CHANGED);

		/* Screensaver Timeout */
		createRow(_("settings.screensaver_timeout"), m_screensaverTimeout);
		setSliderNumberpadLabel(m_screensaverTimeout, _("settings.screensaver_timeout"));
		m_screensaverTimeout.setHeight(LV_SIZE_CONTENT);
		m_screensaverTimeout.setRange(0, 5 * 60); // seconds
		m_screensaverTimeout.setValueChangedCallback(
			[](float value) { StorageHelper::setData(ID_SCREENSAVER_TIMEOUT, static_cast<int32_t>(value * 1000)); });
		m_screensaverTimeout.setOutOfRangeMode(Slider::OutOfRange::UPPER);

		/* Notifications */
		createHeader(_("settings.headers.notifications"));

		/* Display Connected Message */
		createRow(_("settings.display_connected_message"), m_displayConnectedMessage);
		m_displayConnectedMessage.setAlign(LV_ALIGN_CENTER);
		m_displayConnectedMessage.setCheckedCallback(
			[](bool checked) { StorageHelper::setData(ID_DISPLAY_CONNECTED_MESSAGE, checked); });
		m_displayConnectedMessage.setChecked(StorageHelper::getData(ID_DISPLAY_CONNECTED_MESSAGE, true));

		/* Notification Level */
		createRow(_("settings.notification_level"), m_notificationLevel);
		m_notificationLevel.setHeight(LV_SIZE_CONTENT);
		for (auto& level : RESPONSE_TYPE_STRINGS)
		{
			m_notificationLevel.addOption(_(level));
		}
		m_notificationLevel.setSelectedCallback([](uint32_t index, std::string_view /* option */)
												{ StorageHelper::setData(ID_NOTIFICATION_LEVEL, index); });

		/* Info Timeout */
		createRow(_("settings.notification_timeout"), m_notificationTimeout);
		setSliderNumberpadLabel(m_notificationTimeout, _("settings.notification_timeout"));
		m_notificationTimeout.setHeight(LV_SIZE_CONTENT);
		m_notificationTimeout.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_notificationTimeout.setRange(0, 5000);
		m_notificationTimeout.setValueChangedCallback(
			[](float value) { StorageHelper::setData(ID_NOTIFICATION_TIMEOUT, static_cast<uint32_t>(value)); });

		/* Auto-close Error Notifications */
		createRow(_("settings.notification_auto_close_error"), m_notificationAutoCloseError);
		m_notificationAutoCloseError.setCheckedCallback(
			[](bool checked) { StorageHelper::setData(ID_NOTIFICATION_AUTO_CLOSE_ERROR, !checked); });
	}

	void GeneralSettings::onInit()
	{
		m_brightness.setNumberPad(&HomeView::instance().getNumberPad());
		m_screensaverTimeout.setNumberPad(&HomeView::instance().getNumberPad());
		m_notificationTimeout.setNumberPad(&HomeView::instance().getNumberPad());
	}

	void GeneralSettings::onShow()
	{
		// Update language selection
		m_language.setSelected(std::string(i18n::getCurrentLanguage()));
		m_brightness.setValue(static_cast<float>(DisplayHelper::getBrightness()));
		m_screensaverTimeout.setValue(
			static_cast<float>(StorageHelper::getData(ID_SCREENSAVER_TIMEOUT, DEFAULT_SCREEN_TIMEOUT) / 1000));
		m_notificationLevel.setSelected(
			(StorageHelper::getData<uint32_t>(ID_NOTIFICATION_LEVEL, DEFAULT_NOTIFICATION_LEVEL)));
		m_notificationTimeout.setValue(
			StorageHelper::getData<float>(ID_NOTIFICATION_TIMEOUT, DEFAULT_NOTIFICATION_TIMEOUT));
		m_notificationAutoCloseError.setChecked(
			!StorageHelper::getData(ID_NOTIFICATION_AUTO_CLOSE_ERROR, DEFAULT_NOTIFICATION_AUTO_CLOSE_ERROR));
	}

	ConnectionSettings::ConnectionSettings(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		/* Connection method */
		createRow(_("settings.duet_connection_method"), m_connectionMethod);
		std::vector<std::string> options;
		for (const auto& method : Comm::duetCommunicationTypeNames)
		{
			options.push_back(_(method.data()));
		}
		m_connectionMethod.setHeight(LV_SIZE_CONTENT);
		m_connectionMethod.setOptions(options);
		m_connectionMethod.addEventCallback(
			[this](lv_event_t*)
			{
				auto comm_type = (Comm::CommunicationType)(m_connectionMethod.getSelected());
				Comm::DUET.SetCommunicationType(comm_type);
				switch (comm_type)
				{
				case Comm::CommunicationType::uart:
				case Comm::CommunicationType::usb:
					setUsbMode(Comm::UsbMode::Host);
					break;
				case Comm::CommunicationType::network:
					setUsbMode(Comm::UsbMode::InternalWiFi);
					break;
				default:
					break;
				}
				m_usbMode.setSelected(static_cast<uint32_t>(Comm::getUsbMode()));
				showConnectionMethodSettings(Comm::DUET.GetCommunicationType());
			},
			LV_EVENT_VALUE_CHANGED);

		/* USB mode */
		createRow(_("settings.usb_mode"), m_usbMode);
		m_usbMode.setHeight(LV_SIZE_CONTENT);
		{
			std::array usbModeOptions = {
				_("settings.usb_mode_host"),
				_("settings.usb_mode_device"),
				_("settings.usb_mode_internal_wifi"),
			};
			m_usbMode.setOptions(usbModeOptions);
		}
		m_usbMode.setSelectedCallback([this](uint32_t index, std::string_view /* option */)
									  { setUsbMode(Comm::UsbMode(index)); });

		/* Poll Interval */
		createRow(_("settings.duet_poll_interval"), m_pollInterval);
		setSliderNumberpadLabel(m_pollInterval, _("settings.duet_poll_interval"));
		m_pollInterval.setHeight(LV_SIZE_CONTENT);
		m_pollInterval.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_pollInterval.setRange(MIN_PRINTER_POLL_INTERVAL.count(), 2000);
		m_pollInterval.setValueChangedCallback(
			[](float value) { Comm::DUET.SetPollInterval(std::chrono::milliseconds(static_cast<int32_t>(value))); });

		/* Wifi settings */
		createRow(_("settings.duet_ip_address"), m_duetIpAddress);
		m_duetIpAddress.setHeight(LV_SIZE_CONTENT);
		m_duetIpAddress.setOneLine(true);
		m_duetIpAddress.setPlaceholderText(_("settings.duet_ip_address_prompt"));
		m_duetIpAddress.setAcceptedChars("0123456789.");
#if USE_MODAL_NUMBERPAD_FOR_IP_ADDRESS
		m_duetIpAddress.getTextarea().addEventCallback(
			[this](lv_event_t*)
			{
				if (auto np = m_duetIpAddress.getNumberPad())
				{
					np->setHeader(_("settings.duet_ip_address_prompt"));
				}
			},
			LV_EVENT_CLICKED);
#else
		m_duetIpAddress.getTextarea().addEventCallback(
			[this](lv_event_t* e) { onTextareaEvent(e, m_duetIpAddress, LV_KEYBOARD_MODE_NUMBER); }, LV_EVENT_ALL);
#endif

// A bit gross but the callback is the same and this way it means it can't accidentally do different things if updated
// in the future
#if USE_MODAL_NUMBERPAD_FOR_IP_ADDRESS
		m_duetIpAddress.addEventCallback
#else
		m_duetIpAddress.addConfirmEventCallback
#endif
			([this](lv_event_t*) { Comm::DUET.SetHostname(m_duetIpAddress.getText()); }
#if USE_MODAL_NUMBERPAD_FOR_IP_ADDRESS
			 ,
			 LV_EVENT_VALUE_CHANGED
#endif
			);

		createRow(_("settings.duet_password"), m_duetPassword);
		m_duetPassword.setHeight(LV_SIZE_CONTENT);
		m_duetPassword.setOneLine(true);
		m_duetPassword.setPlaceholderText(_("settings.duet_password_prompt"));
		m_duetPassword.setPasswordMode(true);
		m_duetPassword.getTextarea().addEventCallback(
			[this](lv_event_t* e) { onTextareaEvent(e, m_duetPassword, LV_KEYBOARD_MODE_TEXT_LOWER); }, LV_EVENT_ALL);
		m_duetPassword.addConfirmEventCallback([this](lv_event_t*)
											   { Comm::DUET.SetPassword(m_duetPassword.getText()); });

		createHeader(_("settings.headers.screen_networking"));
		createSpanRow(m_wifiSelector);
		m_wifiSelector.setHeight(LV_SIZE_CONTENT);
	}

	void ConnectionSettings::setKeyboard(LvKeyboard* keyboard)
	{
		m_keyboard = keyboard;
#if !USE_MODAL_NUMBERPAD_FOR_IP_ADDRESS
		m_duetIpAddress.setKeyboard(m_keyboard);
#endif
		m_duetPassword.setKeyboard(m_keyboard);
		m_wifiSelector.setKeyboard(m_keyboard);
	}

	void ConnectionSettings::showConnectionMethodSettings(const Comm::CommunicationType method)
	{
		UI_LOCK();

		setRowVisibility(m_duetIpAddress, method == Comm::CommunicationType::network);
		setRowVisibility(m_duetPassword, method == Comm::CommunicationType::network);
	}

	void ConnectionSettings::onInit()
	{
		m_pollInterval.setNumberPad(&HomeView::instance().getNumberPad());
#if USE_MODAL_NUMBERPAD_FOR_IP_ADDRESS
		m_duetIpAddress.setNumberPad(&HomeView::instance().getNumberPad());
#endif
	}

	void ConnectionSettings::onShow()
	{
		// Update USB mode selection
		auto communicationType = Comm::DUET.GetCommunicationType();
		m_connectionMethod.setSelected(static_cast<uint32_t>(communicationType));
		m_usbMode.setSelected(static_cast<uint32_t>(Comm::getUsbMode()));
		m_pollInterval.setValue(static_cast<float>(Comm::DUET.GetPollInterval().count()));
		m_duetIpAddress.setText(Comm::DUET.GetHostname());
		m_duetPassword.setText(Comm::DUET.GetPassword());
		showConnectionMethodSettings(communicationType);
	}

	DisplaySettings::DisplaySettings(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		/* Theme */
		createRow(_("settings.theme"), m_theme);
		m_theme.setHeight(LV_SIZE_CONTENT);
		for (const auto& theme : Themes::getThemes())
		{
			m_theme.addOption(_(fmt::format("theme.id.{:s}", theme->getName())));
		}
		m_theme.setSelectedCallback(
			[this](uint32_t index, std::string_view /* option */)
			{
				LOG_DBG("Changing theme");
				auto theme = Themes::getTheme(index);
				if (theme == nullptr)
				{
					return;
				}
				theme->setThemeActive();
				updateThemePreview();
				LOG_DBG("Finished changing theme");
				StorageHelper::setData(ID_THEME, index);
				// view->getMainSettingsPresenter()->setTheme(selected);
			});
		m_theme.setSelected(StorageHelper::getData(ID_THEME, 0));

		/* Font */
		createRow(_("settings.font"), m_font);
		m_font.setHeight(LV_SIZE_CONTENT);
		for (const auto& font : FontManager::getLoadedFontNames())
		{
#if 0
			if (font.ends_with("-Bold"))
			{
				continue;
			}
#endif
			m_font.addOption(font);
		}
		m_font.setSelectedCallback([this](uint32_t /* index */, std::string_view option)
								   { FontManager::setActiveTypeface(std::string(option)); });

		/* Icons */
		createRow(_("settings.icons"), m_icons);
		m_icons.setHeight(LV_SIZE_CONTENT);
		for (const auto& iconSet : Themes::getIconSets())
		{
			m_icons.addOption(_(fmt::format("theme.icon_sets.{:s}", iconSet)));
		}
		m_icons.setSelectedCallback(
			[this](uint32_t index, std::string_view)
			{
				LOG_INFO("Changing icon set");
				Themes::setIconFolder(Themes::getIconSets().at(index));
			});

		/* Theme preview */
		createSpanRow(m_themePreview);
		m_themePreview.setHeight(LV_SIZE_CONTENT);
	}

	void DisplaySettings::updateThemePreview()
	{
		auto theme = Themes::getCurrentTheme();
		if (theme == nullptr)
		{
			return;
		}

		LOG_DBG("Updating theme preview");
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

	void DisplaySettings::onInit()
	{
		m_themePreview.setNumberPad(&HomeView::instance().getNumberPad());
	}

	void DisplaySettings::onShow()
	{
		updateThemePreview();
		m_font.setSelected(FontManager::getActiveTypefaceName());
		m_icons.setSelected(_(fmt::format("theme.icon_sets.{:s}", Themes::getIconFolder())));
	}

	DeveloperSettings::DeveloperSettings(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		/* Debug level */
		createRow(_("settings.debug_level"), m_debugLevel);
		m_debugLevel.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		for (const auto& level : Log::DebugLevelStrings)
		{
			m_debugLevel.addOption(_(level));
		}
		m_debugLevel.setSelectedCallback([this](uint32_t index, std::string_view /* option */)
										 { Log::SetDebugLevel(static_cast<Log::DebugLevel>(index)); });

		/* Advanced settings */
		createRow(_("settings.enable_advanced_settings"), m_enableAdvancedSettings);
		m_enableAdvancedSettings.setCheckedCallback([](bool checked)
													{ StorageHelper::setData(ID_ENABLE_ADVANCED_SETTINGS, checked); });

#if DEBUG_BORDERS
		/* Debug borders */
		createRow(_("settings.debug_borders"), m_debugBorders);
		m_debugBorders.setCheckedCallback(
			[](bool checked)
			{
				StorageHelper::setData(ID_DEBUG_BORDERS, checked);
				Themes::showDebugBorders(lv_screen_active(), checked);
			});
#endif

		/* Enable SSH */
		createRow(_("settings.enable_ssh"), m_enableSSH);
		m_enableSSH.setCheckedCallback(
			[](bool checked)
			{
				StorageHelper::setData(ID_SSH_ENABLED, checked);
#if !SIMULATION
				if (checked && !std::filesystem::exists("/etc/init.d/S50dropbear"))
				{
					std::filesystem::rename("/etc/init.d/50dropbear", "/etc/init.d/S50dropbear");
					if (system("/etc/init.d/S50dropbear start") != 0)
					{
						LOG_ERROR("Failed to start SSH server");
					}
				}
				else if (!checked && std::filesystem::exists("/etc/init.d/S50dropbear"))
				{
					if (system("/etc/init.d/S50dropbear stop") != 0)
					{
						LOG_ERROR("Failed to stop SSH server");
					}
					std::filesystem::rename("/etc/init.d/S50dropbear", "/etc/init.d/50dropbear");
				}
#endif
			});

#if LV_USE_SYSMON
		createRow(_("settings.enable_system_monitor"), m_enableSystemMonitor);
		m_enableSystemMonitor.setCheckedCallback(
			[](bool checked)
			{
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
			});
#endif

		/* System Logging */
		createRow(_("settings.system_logging"), m_systemLogging);
		m_systemLogging.setCheckedCallback(
			[](bool checked)
			{
				StorageHelper::setData(ID_ENABLE_UI_LOGGING, checked);
				Log::EnableUiLogging(checked);
			});

		createSpanRow(m_controls);
		m_controls.setSize(LV_PCT(50), LV_PCT(40));
		m_controls.setFlexFlow(LV_FLEX_FLOW_ROW_WRAP);
		m_controls.setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (auto& btn : {&m_restart, &m_eraseAndRestart, &m_reboot, &m_startHardwareTest})
		{
			btn->setSize(150, 100);
		}

		m_restart.setText(_("settings.restart"));
		m_eraseAndRestart.setText(_("settings.erase_and_restart"));
		m_reboot.setText(_("settings.reboot"));
		m_startHardwareTest.setText(_("settings.start_hardware_test"));

		m_restart.addClickedCallback([](lv_event_t*) { Restart(); });
		m_eraseAndRestart.addClickedCallback([](lv_event_t*) { EraseAndRestart(); });
		m_reboot.addClickedCallback([](lv_event_t*) { Reboot(); });
		m_startHardwareTest.addClickedCallback([this](lv_event_t*) { getPresenter()->startHardwareTest(); });

		/* Hardware test */
		m_hardwareTest.hide();
	}

	void DeveloperSettings::onInit() {}

	void DeveloperSettings::onShow()
	{
		m_debugLevel.setSelected(static_cast<uint32_t>(Log::GetDebugLevel()));
		m_enableAdvancedSettings.setChecked(StorageHelper::getData(ID_ENABLE_ADVANCED_SETTINGS, false));
#if DEBUG_BORDERS
		m_debugBorders.setChecked(Themes::isdebugBorderVisible(lv_screen_active()));
#endif
		m_enableSSH.setChecked(
			StorageHelper::getData(ID_SSH_ENABLED, std::filesystem::exists("/etc/init.d/S50dropbear")));
#if LV_USE_SYSMON
		m_enableSystemMonitor.setChecked(StorageHelper::getData(ID_SYSTEM_MONITOR_ENABLED, false));
#endif
		m_systemLogging.setChecked(StorageHelper::getData(ID_ENABLE_UI_LOGGING, false));
	}

	static void onTextareaEvent(lv_event_t* e, TextBox& text_box, lv_keyboard_mode_t mode)
	{
		UI_LOCK();

		lv_event_code_t code = lv_event_get_code(e);
		if (code != LV_EVENT_FOCUSED && code != LV_EVENT_DEFOCUSED && code != LV_EVENT_READY)
		{
			return;
		}

		LvKeyboard* kb = text_box.getKeyboard();
		if (!kb)
		{
			LOG_WARN("Keyboard not set");
			return;
		}

		switch (code)
		{
		case LV_EVENT_FOCUSED:
			kb->setMode(mode);
			kb->show(true);
			break;
		case LV_EVENT_DEFOCUSED:
		case LV_EVENT_READY:
			kb->hide();
			break;
		default:
			break;
		}
	}
} // namespace UI
