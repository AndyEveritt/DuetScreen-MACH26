/*
 * Storage.h
 *
 *  Created on: 16 Apr 2024
 *      Author: andy
 */

#ifndef JNI_STORAGE_H_
#define JNI_STORAGE_H_

#include <sys/types.h>

/* Duet */
constexpr const char* ID_DUET = "duet";
constexpr const char* ID_DUET_HOSTNAME = "hostname";
constexpr const char* ID_DUET_PASSWORD = "password";
constexpr const char* ID_DUET_COMMUNICATION_TYPE = "communication_type";
constexpr const char* ID_DUET_POLL_INTERVAL = "poll_interval";
constexpr const char* ID_DUET_BAUD_RATE = "baud_rate";

/* UI */
constexpr const char* ID_THEME = "ui:theme";

constexpr const char* ID_SCREENSAVER_ENABLE = "ui:screensaver_enable";
constexpr const char* ID_SCREENSAVER_TIMEOUT = "ui:screensaver_timeout";
constexpr const char* ID_NOTIFICATION_TIMEOUT = "ui:info_timeout";
constexpr const char* ID_NOTIFICATION_LEVEL = "ui:notification_level";
constexpr const char* ID_NOTIFICATION_AUTO_CLOSE_ERROR = "ui:notification_auto_close_error";

constexpr const char* ID_UI_CONSOLE_COMMAND_LIST_COLLAPSED = "ui:console_command_list_collapsed";

constexpr const char* ID_BABYSTEP_AMOUNT = "ui:baby_step_amount";
constexpr const char* ID_MOVE_FEEDRATES = "ui:move:feedrates";
constexpr const char* ID_MOVE_SELECTED_FEEDRATE = "ui:move:selected_feedrate";
constexpr const char* ID_EXTRUSION_DISTANCES = "ui:extrusion:distances";
constexpr const char* ID_EXTRUSION_SELECTED_DISTANCE = "ui:extrusion:selected_distance";
constexpr const char* ID_EXTRUSION_FEEDRATES = "ui:extrusion:feedrates";
constexpr const char* ID_EXTRUSION_SELECTED_FEEDRATE = "ui:extrusion:selected_feedrate";

constexpr const char* ID_FILE_SORT_BY = "ui:file:sort_by";
constexpr const char* ID_FILE_SORT_DESCENDING = "ui:file:sort_descending";

constexpr const char* ID_UNIT_SYSTEM = "ui:units";

/* System */
constexpr const char* ID_BUZZER_ENABLED = "sys:buzzer_enabled";
constexpr const char* ID_UPGRADE_FILE_LAST_MODIFIED = "sys:upgrade_file_last_modified";
constexpr const char* ID_SYS_LANG_CODE_KEY = "sys:lang_code";
constexpr const char* ID_SYS_BRIGHTNESS_KEY = "sys:brightness";
constexpr const char* ID_USB_MODE = "sys:usb_mode";
constexpr const char* ID_DISPLAY_CONNECTED_MESSAGE = "sys:display_connected_message";

constexpr const char* ID_WEBCAM_URL = "webcam:url_%d";
constexpr const char* ID_ACTIVE_WEBCAM_INDEX = "webcam:active_index";
constexpr const char* ID_WEBCAM_UPADTE_ITERVAL = "webcam:update_interval";

/* Debug */
constexpr const char* ID_DEBUG_LEVEL = "debug:level";
constexpr const char* ID_LOG_FILE = "debug:file";
constexpr const char* ID_ENABLE_UI_LOGGING = "debug:ui_logging";
constexpr const char* ID_BURNIN_FREQUENCY = "debug:burnin_frequency";

#if DEBUG_BORDERS
constexpr const char* ID_DEBUG_BORDERS = "debug:borders";
#endif

/* Developer */
constexpr const char* ID_SSH_ENABLED = "developer:ssh_enabled";

#endif /* JNI_STORAGE_H_ */
