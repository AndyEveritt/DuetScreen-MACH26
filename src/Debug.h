/*
 * Debug.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Andy Everitt
 */

#pragma once

#include "utils/utils.h"
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace Log
{
	using log_time_t = std::chrono::_V2::system_clock::time_point;

	constexpr const char* DebugLevelStrings[] = {
		"Verbose",
		"Debug",
		"Info",
		"Warn",
		"Error",
		"Fatal",
	};

	enum class DebugLevel
	{
		Verbose = 0,
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

	void Init();
	void SetDebugLevel(DebugLevel level);
	const DebugLevel& GetDebugLevel();
	void CloseDebugFile();

	void EnableUiLogging(bool enable);
	bool IsUiLoggingEnabled();
	bool GetNextUiLogMessage(DebugLevel& level, log_time_t& time, std::string& message);

	size_t GetThreadId();

	std::shared_ptr<spdlog::logger> GetLogger();
} // namespace Log

#define CUSTOM_SPDLOG_LOGGER_CALL(logger, level, ...)                                                                  \
	(logger)->log(spdlog::source_loc{__FILE_RELPATH__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)

#if DEBUG
#  define LOG_VERBOSE(...) CUSTOM_SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::trace, __VA_ARGS__)
#else
#  define LOG_VERBOSE(...)
#endif

#define LOG_DBG(...) CUSTOM_SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) CUSTOM_SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) CUSTOM_SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) CUSTOM_SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::err, __VA_ARGS__);
#define LOG_FATAL(...)                                                                                                 \
	CUSTOM_SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::critical, __VA_ARGS__);                     \
	spdlog::dump_backtrace();
