/*
 * Debug.cpp
 *
 *  Created on: 10 May 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "utils/StorageHelper.h"
#include <ctime>
#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

// Use global namespace for std
using std::make_shared;
using std::shared_ptr;
using std::vector;

namespace Log
{
	static DebugLevel s_debugLevel = DebugLevel::Info;
	static shared_ptr<spdlog::logger> s_logger;

	void Init()
	{
		try
		{
			auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

			auto file_sink = make_shared<spdlog::sinks::rotating_file_sink_mt>("DuetScreen.log", 1024 * 1024 * 5, 3);
			file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

			spdlog::sinks_init_list sinks{console_sink, file_sink};
			s_logger = make_shared<spdlog::logger>("duetscreen", sinks);
			s_logger->set_level(spdlog::level::trace);
			s_logger->flush_on(spdlog::level::debug);
			spdlog::set_default_logger(s_logger);
			spdlog::enable_backtrace(32);
			spdlog::trace("Logger initialized {}, {}, {}", 1u, (uint32_t)s_debugLevel, "Hello World!");
			spdlog::debug("Logger initialized {}, {}", 1u, 1.0f);
			spdlog::info("Logger initialized {}", 1u, 1.0f);
			spdlog::warn("Logger initialized {}, {}", 1u, 1.0f);
			spdlog::error("Logger initialized {}, {}", 1u, 1.0f);
			spdlog::critical("Logger initialized {}, {}", 1u, 1.0f);
			spdlog::dump_backtrace();
			LOG_INFO("Logger initialized");
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			fprintf(stderr, "Log initialization failed: %s\n", ex.what());
		}
	}

	void SetDebugLevel(DebugLevel level)
	{
		s_debugLevel = level;
		StorageHelper::setData(ID_DEBUG_LEVEL, level);

		if (!s_logger)
			return;

		switch (level)
		{
		case DebugLevel::Verbose:
			s_logger->set_level(spdlog::level::trace);
			break;
		case DebugLevel::Debug:
			s_logger->set_level(spdlog::level::debug);
			break;
		case DebugLevel::Info:
			s_logger->set_level(spdlog::level::info);
			break;
		case DebugLevel::Warn:
			s_logger->set_level(spdlog::level::warn);
			break;
		case DebugLevel::Error:
			s_logger->set_level(spdlog::level::err);
			break;
		case DebugLevel::Fatal:
			s_logger->set_level(spdlog::level::critical);
			break;
		default:
			s_logger->set_level(spdlog::level::info);
		}
	}

	void SetDebugFile(const char* filename)
	{
		try
		{
			auto file_sink = make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 1024 * 1024 * 5, 3);
			file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

			if (s_logger)
			{
				auto console_sink = s_logger->sinks()[0];
				vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
				s_logger = make_shared<spdlog::logger>("duetscreen", sinks.begin(), sinks.end());
				s_logger->set_level(spdlog::level::trace);
				s_logger->flush_on(spdlog::level::debug);
			}
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			fprintf(stderr, "Failed to set debug file: %s\n", ex.what());
		}
	}

	void CloseDebugFile()
	{
		if (s_logger)
		{
			s_logger->flush();
		}
	}

	const DebugLevel& GetDebugLevel()
	{
		return s_debugLevel;
	}

	shared_ptr<spdlog::logger> GetLogger()
	{
		return s_logger;
	}
} // namespace Log
