/*
 * Storage.h
 *
 *  Created on: 16 Apr 2024
 *      Author: andy
 */

#ifndef JNI_STORAGE_H_
#define JNI_STORAGE_H_

#include <string_view>

/* Duet */
constexpr std::string_view ID_DUET = "duet";
constexpr std::string_view ID_DUET_HOSTNAME = "hostname";
constexpr std::string_view ID_DUET_PASSWORD = "password";
constexpr std::string_view ID_DUET_COMMUNICATION_TYPE = "communication_type";
constexpr std::string_view ID_DUET_POLL_INTERVAL = "poll_interval";
constexpr std::string_view ID_DUET_BAUD_RATE = "baud_rate";

/* UI */
constexpr std::string_view ID_THEME = "ui:theme";
constexpr std::string_view ID_FONT = "ui:font";
constexpr std::string_view ID_ICON_FOLDER = "ui:icon_folder";

constexpr std::string_view ID_SCREENSAVER_ENABLE = "ui:screensaver_enable";
constexpr std::string_view ID_SCREENSAVER_TIMEOUT = "ui:screensaver_timeout";
constexpr std::string_view ID_NOTIFICATION_TIMEOUT = "ui:info_timeout";
constexpr std::string_view ID_NOTIFICATION_LEVEL = "ui:notification_level";
constexpr std::string_view ID_NOTIFICATION_AUTO_CLOSE_ERROR = "ui:notification_auto_close_error";

constexpr std::string_view ID_UI_CONSOLE_COMMAND_LIST_COLLAPSED = "ui:console_command_list_collapsed";

constexpr std::string_view ID_BABYSTEP_AMOUNT = "ui:baby_step_amount";
constexpr std::string_view ID_MOVE_DISTANCES = "ui:move:distances";
constexpr std::string_view ID_MOVE_FEEDRATES = "ui:move:feedrates";
constexpr std::string_view ID_MOVE_SELECTED_FEEDRATE = "ui:move:selected_feedrate";

/* Multi value selectors */
// these will have the following sub keys {"values", "selected"}
constexpr std::string_view ID_MVS_EXTRUSION_FEEDRATES = "ui:extrusion:feedrates";
constexpr std::string_view ID_MVS_EXTRUSION_DISTANCES = "ui:extrusion:distances";

constexpr std::string_view ID_FILE_SORT_BY = "ui:file:sort_by";
constexpr std::string_view ID_FILE_SORT_DESCENDING = "ui:file:sort_descending";

constexpr std::string_view ID_UNIT_SYSTEM = "ui:units";

/* System */
constexpr std::string_view ID_BUZZER_ENABLED = "sys:buzzer_enabled";
constexpr std::string_view ID_UPGRADE_FILE_LAST_MODIFIED = "sys:upgrade_file_last_modified";
constexpr std::string_view ID_SYS_LANG_CODE_KEY = "sys:lang_code";
constexpr std::string_view ID_SYS_BRIGHTNESS_KEY = "sys:brightness";
constexpr std::string_view ID_USB_MODE = "sys:usb_mode";
constexpr std::string_view ID_DISPLAY_CONNECTED_MESSAGE = "sys:display_connected_message";
constexpr std::string_view ID_ENABLE_ADVANCED_SETTINGS = "sys:enable_advanced_settings";

constexpr std::string_view ID_WEBCAM_URL = "webcam:url_%d";
constexpr std::string_view ID_ACTIVE_WEBCAM_INDEX = "webcam:active_index";
constexpr std::string_view ID_WEBCAM_UPADTE_ITERVAL = "webcam:update_interval";

/* Debug */
constexpr std::string_view ID_DEBUG_LEVEL = "debug:level";
constexpr std::string_view ID_LOG_FILE = "debug:file";
constexpr std::string_view ID_ENABLE_UI_LOGGING = "debug:ui_logging";
constexpr std::string_view ID_BURNIN_FREQUENCY = "debug:burnin_frequency";
constexpr std::string_view ID_SYSTEM_MONITOR_ENABLED = "debug:system_monitor_enabled";

#if DEBUG_BORDERS
constexpr std::string_view ID_DEBUG_BORDERS = "debug:borders";
#endif

#if DEVELOPER_MODE
/* Developer */
constexpr std::string_view ID_SSH_ENABLED = "developer:ssh_enabled";
constexpr std::string_view ID_ADB_ENABLED = "developer:adb_enabled";
#endif

#endif /* JNI_STORAGE_H_ */
