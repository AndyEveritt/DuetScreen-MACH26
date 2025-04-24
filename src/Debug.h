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
	void SetDebugFile(const char* filename);
	void CloseDebugFile();

	std::shared_ptr<spdlog::logger> GetLogger();
} // namespace Log

#if 1
#  define LOG_VERBOSE(...) spdlog::trace(__VA_ARGS__)
#  define LOG_DBG(...) spdlog::debug(__VA_ARGS__)
#  define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#  define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#  define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#  define LOG_FATAL(...) spdlog::critical(__VA_ARGS__)
#else
#  define LOG_VERBOSE(...)
#  define LOG_DBG(...)
#  define LOG_INFO(...)
#  define LOG_WARN(...)
#  define LOG_ERROR(...)
#  define LOG_FATAL(...)
#endif
