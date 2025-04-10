/*
 * Debug.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef JNI_DEBUG_HPP_
#define JNI_DEBUG_HPP_

#include "utils/utils.h"
#include <cstdarg>

constexpr const char* DebugLevelStrings[] = {
	"[Verbose]",
	"[Debug]",
	"[Info]",
	"[Warn]",
	"[Error]",
	"[Fatal]",
};

enum class DebugLevel
{
	Verbose = 0,
	Debug,
	Info,
	Warn,
	Error,
	Fatal,
	COUNT
};

#define LOG_FORMAT_START "\033["
#define LOG_FORMAT_END "\033[0m"
#define LOG_FORMAT_BOLD "1"
#define LOG_FORMAT_UNDERLINE "4"
#define LOG_FORMAT_ITALIC "3"
#define LOG_COLOR_BLACK "30m"
#define LOG_COLOR_RED "31m"
#define LOG_COLOR_GREEN "32m"
#define LOG_COLOR_YELLOW "33m"
#define LOG_COLOR_BLUE "34m"
#define LOG_COLOR_MAGENTA "35m"
#define LOG_COLOR_CYAN "36m"
#define LOG_COLOR_WHITE "37m"

static_assert(ARRAY_SIZE(DebugLevelStrings) == (int)DebugLevel::COUNT);

void SetDebugLevel(DebugLevel level);
const DebugLevel& GetDebugLevel();

void verbose_inner(const char* fmt, ...);
void dbg_inner(const char* fmt, ...);
void info_inner(const char* fmt, ...);
void warn_inner(const char* fmt, ...);
void error_inner(const char* fmt, ...);
void fatal_inner(const char* fmt, ...);

#if LOG_TIMESTAMPS
#  include "utils/TimeHelper.h"
#  define LOG_TIMESTAMP_FMT "%lld "
#  define LOG_TIMESTAMP_ARGS TimeHelper::getCurrentTime(),
#else
#  define LOG_TIMESTAMP_FMT ""
#  define LOG_TIMESTAMP_ARGS
#endif

#if LOG_THREAD
#  define LOG_THREAD_FMT "[%u] "
#  define LOG_THREAD_ARGS pthread_self(),
#else
#  define LOG_THREAD_FMT ""
#  define LOG_THREAD_ARGS
#endif

#define LOG_LEVEL_FMT(color) LOG_FORMAT_START LOG_FORMAT_BOLD ";" color "%s" LOG_FORMAT_END
#define LOG_FILE_FMT(color) LOG_FORMAT_START LOG_FORMAT_ITALIC ";" LOG_FORMAT_UNDERLINE ";" color "%s:%d" LOG_FORMAT_END
#define LOG_FUNCTION_FMT(color) LOG_FORMAT_START LOG_FORMAT_ITALIC ";" color " %s(): " LOG_FORMAT_END
#define LOG_MESSAGE_FMT(color, fmt) LOG_FORMAT_START color fmt LOG_FORMAT_END

#define LOG_FUNCTION(name, color, level, fmt, args...)                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		name##_inner(LOG_TIMESTAMP_FMT LOG_THREAD_FMT LOG_LEVEL_FMT(color) " " LOG_FILE_FMT(color)                     \
						 LOG_FUNCTION_FMT(color) LOG_MESSAGE_FMT(color, fmt) "\n",                                     \
					 LOG_TIMESTAMP_ARGS LOG_THREAD_ARGS DebugLevelStrings[(int)level],                                 \
					 __FILE_RELPATH__,                                                                                 \
					 __LINE__,                                                                                         \
					 __FUNCTION__,                                                                                     \
					 ##args);                                                                                          \
	} while (0)

#ifdef DEBUG
#  define verbose(fmt, args...) LOG_FUNCTION(verbose, LOG_COLOR_WHITE, DebugLevel::Verbose, fmt, ##args)
#  define dbg(fmt, args...) LOG_FUNCTION(dbg, LOG_COLOR_BLUE, DebugLevel::Debug, fmt, ##args)
#else
#  define verbose(fmt, args...)
#  define dbg(fmt, args...)
#endif

#define info(fmt, args...) LOG_FUNCTION(info, LOG_COLOR_GREEN, DebugLevel::Info, fmt, ##args)
#define warn(fmt, args...) LOG_FUNCTION(warn, LOG_COLOR_YELLOW, DebugLevel::Warn, fmt, ##args)
#define error(fmt, args...) LOG_FUNCTION(error, LOG_COLOR_RED, DebugLevel::Error, fmt, ##args)
#define fatal(fmt, args...) LOG_FUNCTION(fatal, LOG_COLOR_MAGENTA, DebugLevel::Fatal, fmt, ##args)

#endif /* JNI_DEBUG_HPP_ */
