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

enum class LogColors
{
	Black = 30,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
};

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
  #include "utils/TimeHelper.h"
  #define LOG_FUNCTION(name, color, level, fmt, args...)                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
	  const int colorCode = static_cast<int>(color);                                                                   \
	  name##_inner("%lld \033[1;%dm%s\033[0m \033[3;4;%dm%s:%d\033[0m \033[3;%dm%s():\033[0m\033[%dm " fmt             \
				   "\033[0m\n",                                                                                        \
				   TimeHelper::getCurrentTime(),                                                                       \
				   colorCode,                                                                                          \
				   DebugLevelStrings[(int)level],                                                                      \
				   colorCode,                                                                                          \
				   __FILE_RELPATH__,                                                                                   \
				   __LINE__,                                                                                           \
				   colorCode,                                                                                          \
				   __FUNCTION__,                                                                                       \
				   colorCode,                                                                                          \
				   ##args);                                                                                            \
	} while (0)
#else
  #define LOG_FUNCTION(name, color, level, fmt, args...)                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
	  const int colorCode = static_cast<int>(color);                                                                   \
	  name##_inner("\033[1;%dm%s\033[0m \033[3;4;%dm%s:%d\033[0m \033[3;%dm%s():\033[0m\033[%dm " fmt "\033[0m\n",     \
				   colorCode,                                                                                          \
				   DebugLevelStrings[(int)level],                                                                      \
				   colorCode,                                                                                          \
				   __FILE_RELPATH__,                                                                                   \
				   __LINE__,                                                                                           \
				   colorCode,                                                                                          \
				   __FUNCTION__,                                                                                       \
				   colorCode,                                                                                          \
				   ##args);                                                                                            \
	} while (0)
#endif

#ifdef DEBUG
  #define verbose(fmt, args...) LOG_FUNCTION(verbose, LogColors::White, DebugLevel::Verbose, fmt, ##args)
  #define dbg(fmt, args...) LOG_FUNCTION(dbg, LogColors::Blue, DebugLevel::Debug, fmt, ##args)
#else
  #define verbose(fmt, args...)
  #define dbg(fmt, args...)
#endif

#define info(fmt, args...) LOG_FUNCTION(info, LogColors::Green, DebugLevel::Info, fmt, ##args)
#define warn(fmt, args...) LOG_FUNCTION(warn, LogColors::Yellow, DebugLevel::Warn, fmt, ##args)
#define error(fmt, args...) LOG_FUNCTION(error, LogColors::Red, DebugLevel::Error, fmt, ##args)
#define fatal(fmt, args...) LOG_FUNCTION(fatal, LogColors::Magenta, DebugLevel::Fatal, fmt, ##args)

#endif /* JNI_DEBUG_HPP_ */
