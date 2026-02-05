/*
 * Storage.h
 *
 *  Created on: 16 Apr 2024
 *      Author: andy
 */

#ifndef JNI_STORAGE_H_
#define JNI_STORAGE_H_

#include <chrono>
#include <concepts>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <termios.h>
#include <type_traits>
#include <vector>

template <typename T>
concept StorageKeyValue = std::is_convertible_v<T, nlohmann::json> || std::is_enum_v<T>;

namespace detail
{
	template <typename K, StorageKeyValue T>
	requires std::is_convertible_v<K, std::string_view> && std::is_constructible_v<K, std::string_view>
	struct BaseStorageKey
	{
		using storage_key_marker = void;
		using value_type = T;

		constexpr BaseStorageKey(std::string_view id_, const T& default_value_)
			: id(id_)
			, default_value(default_value_)
		{
		}

		const K id;
		const T default_value;
	};
} // namespace detail

template <StorageKeyValue T>
using StorageKey = detail::BaseStorageKey<std::string_view, T>;

template <StorageKeyValue T>
using StorageKeyRunTime = detail::BaseStorageKey<std::string, T>;

/* Forward declare types */

namespace Comm
{
	enum class UsbMode;
	enum class CommunicationType;
} // namespace Comm
namespace OM
{
	namespace FileSystem
	{
		enum class SortBy;
	}
} // namespace OM
namespace Log
{
	enum class DebugLevel;
}
namespace Units
{
	enum class UnitSystem;
}
enum class ResponseType;

/* Convertors */

namespace std::chrono
{
	void to_json(nlohmann::json& j, const std::chrono::seconds& c);
	void from_json(const nlohmann::json& j, std::chrono::seconds& c);
	void to_json(nlohmann::json& j, const std::chrono::milliseconds& c);
	void from_json(const nlohmann::json& j, std::chrono::milliseconds& c);
} // namespace std::chrono

/* Duet */
extern const StorageKey<std::string_view> ID_DUET_IP_ADDRESS;
extern const StorageKey<std::string_view> ID_DUET_PASSWORD;
extern const StorageKey<Comm::CommunicationType> ID_DUET_COMMUNICATION_TYPE;
extern const StorageKey<std::chrono::milliseconds> ID_DUET_POLL_INTERVAL;
extern const StorageKey<speed_t> ID_DUET_BAUD_RATE;

/* UI */
extern const StorageKey<size_t> ID_THEME;
extern const StorageKey<std::string_view> ID_FONT;
extern const StorageKey<std::string_view> ID_ICON_FOLDER;

extern const StorageKey<bool> ID_SCREENSAVER_ENABLE;
extern const StorageKey<std::chrono::seconds> ID_SCREENSAVER_TIMEOUT;
extern const StorageKey<std::chrono::milliseconds> ID_NOTIFICATION_TIMEOUT;
extern const StorageKey<ResponseType> ID_NOTIFICATION_LEVEL;
extern const StorageKey<bool> ID_NOTIFICATION_AUTO_CLOSE_ERROR;

extern const StorageKey<bool> ID_UI_CONSOLE_COMMAND_LIST_COLLAPSED;

extern const StorageKey<std::vector<float>> ID_BABYSTEP_AMOUNT;
extern const StorageKey<std::vector<float>> ID_MOVE_DISTANCES;
extern const StorageKey<std::vector<uint32_t>> ID_MOVE_FEEDRATES;

extern const StorageKey<bool> ID_SHOW_CONFIRMATION_DIALOGS;

/* Multi value selectors */
// these will have the following sub keys {"values", "selected"}
extern const std::string_view ID_MVS_EXTRUSION_FEEDRATES;
extern const std::string_view ID_MVS_EXTRUSION_DISTANCES;

extern const StorageKey<OM::FileSystem::SortBy> ID_FILE_SORT_BY;
extern const StorageKey<bool> ID_FILE_SORT_DESCENDING;

extern const StorageKey<Units::UnitSystem> ID_UNIT_SYSTEM;

/* System */
extern const StorageKey<time_t> ID_UPGRADE_FILE_LAST_MODIFIED;
extern const StorageKey<std::string_view> ID_SYS_LANG_CODE_KEY;
extern const StorageKey<unsigned int> ID_SYS_BRIGHTNESS_KEY;
extern const StorageKey<Comm::UsbMode> ID_USB_MODE;
extern const StorageKey<bool> ID_DISPLAY_CONNECTED_MESSAGE;
extern const StorageKey<bool> ID_ENABLE_ADVANCED_SETTINGS;

/* Debug */
extern const StorageKey<Log::DebugLevel> ID_DEBUG_LEVEL;
extern const StorageKey<std::string_view> ID_LOG_FILE;
extern const StorageKey<bool> ID_ENABLE_UI_LOGGING;
extern const StorageKey<std::chrono::milliseconds> ID_BURNIN_FREQUENCY;
extern const StorageKey<bool> ID_SYSTEM_MONITOR_ENABLED;

#if DEBUG_BORDERS
extern const StorageKey<bool> ID_DEBUG_BORDERS;
#endif

#if DEVELOPER_MODE
/* Developer */
extern const StorageKey<bool> ID_SSH_ENABLED;
extern const StorageKey<bool> ID_ADB_ENABLED;
#endif

#endif /* JNI_STORAGE_H_ */
