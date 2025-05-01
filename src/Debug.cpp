/*
 * Debug.cpp
 *
 *  Created on: 10 May 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "Configuration.h"
#include "UI/Core/Model.h"
#include "utils/StorageHelper.h"
#include <ctime>
#include <memory>
#include <spdlog/details/os.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

// Use global namespace for std
using std::make_shared;
using std::shared_ptr;
using std::vector;

#define LOG_FORMAT_BOLD_START "\033[1m"
#define LOG_FORMAT_BOLD_END "\033[22m"
#define LOG_FORMAT_ITALIC_START "\033[3m"
#define LOG_FORMAT_ITALIC_END "\033[23m"
#define LOG_FORMAT_UNDERLINE_START "\033[4m"
#define LOG_FORMAT_UNDERLINE_END "\033[24m"

#if LOG_TIMESTAMPS
#  define LOG_TIMESTAMP_FMT "[%Y-%m-%d %H:%M:%S.%e] "
#  if DEBUG
#	define LOG_UI_TIMESTAMP_FMT "[%Y-%m-%d %H:%M:%S.%e] "
#  else
#	define LOG_UI_TIMESTAMP_FMT "[%Y-%m-%d %H:%M:%S] "
#  endif
#else
#  define LOG_TIMESTAMP_FMT ""
#  define LOG_UI_TIMESTAMP_FMT ""
#endif

#define LOG_CONSOLE_PATTERN                                                                                            \
  "%^" LOG_TIMESTAMP_FMT "[%l] [%t] " LOG_FORMAT_UNDERLINE_START LOG_FORMAT_ITALIC_START "%@" LOG_FORMAT_UNDERLINE_END \
  " %!()" LOG_FORMAT_ITALIC_END " %v%$"

#define LOG_FILE_PATTERN LOG_TIMESTAMP_FMT "[%l] [%t] %@ %!() %v"

#define LOG_UI_PATTERN LOG_UI_TIMESTAMP_FMT "[%l] %v"

namespace Log
{
	template <typename Mutex>
	class UiSink : public spdlog::sinks::base_sink<Mutex>
	{
	  protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			// mutex is locked by base_sink
			spdlog::memory_buf_t formatted;
			spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
			std::string str(formatted.data(), formatted.size());
			Model::get().post<EventType::LogMessage>(static_cast<DebugLevel>(msg.level), msg.time, str);
		}

		void flush_() override {}
	};

	using UiSink_mt = UiSink<std::mutex>;

	static DebugLevel s_debugLevel = DebugLevel::Info;
	static shared_ptr<spdlog::logger> s_logger;
	static shared_ptr<UiSink_mt> s_uiSink;

	void Init()
	{
		try
		{
			auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_pattern(LOG_CONSOLE_PATTERN);

			auto file_sink = make_shared<spdlog::sinks::rotating_file_sink_mt>(
				StorageHelper::getData<std::string>(ID_LOG_FILE, DEFAULT_LOG_FILE),
				DEFAULT_LOG_FILE_SIZE,
				DEFAULT_LOG_FILE_COUNT - 1);
			file_sink->set_pattern(LOG_FILE_PATTERN);

			spdlog::sinks_init_list sinks{console_sink, file_sink};
			s_logger = make_shared<spdlog::logger>("duetscreen", sinks);
			s_logger->flush_on(spdlog::level::debug);
			spdlog::set_default_logger(s_logger);
			LOG_INFO("\n\n\n----------------------------------------------------------------------------------\n"
					 "Program started\n"
					 "----------------------------------------------------------------------------------\n\n\n");
			SetDebugLevel(StorageHelper::getData(ID_DEBUG_LEVEL, Log::DebugLevel::Info));
			EnableUiLogging(StorageHelper::getData(ID_ENABLE_UI_LOGGING, false));
			spdlog::flush_every(std::chrono::seconds(1));
			spdlog::enable_backtrace(100);
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

	void CloseDebugFile()
	{
		if (s_logger)
		{
			s_logger->flush();
		}
	}

	void EnableUiLogging(bool enable)
	{
		if ((s_uiSink && enable) || (!s_uiSink && !enable))
			return;

		if (enable)
		{
			s_uiSink = make_shared<UiSink_mt>();
			s_uiSink->set_pattern(LOG_UI_PATTERN);
			s_uiSink->set_level(spdlog::level::warn);
			s_logger->sinks().push_back(s_uiSink);
		}
		else
		{
			auto sinks = s_logger->sinks();
			for (auto it = sinks.begin(); it != sinks.end(); ++it)
			{
				if (*it == s_uiSink)
				{
					sinks.erase(it);
					break;
				}
			}
			s_uiSink.reset();
		}
	}

	bool IsUiLoggingEnabled()
	{
		return s_uiSink != nullptr;
	}

	size_t GetThreadId()
	{
		return spdlog::details::os::thread_id();
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
