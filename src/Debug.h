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

static_assert(ARRAY_SIZE(DebugLevelStrings) == (int)DebugLevel::COUNT);

void SetDebugLevel(DebugLevel level);
const DebugLevel& GetDebugLevel();

void verbose_inner(const char* fmt, ...);
void dbg_inner(const char* fmt, ...);
void info_inner(const char* fmt, ...);
void warn_inner(const char* fmt, ...);
void error_inner(const char* fmt, ...);
void fatal_inner(const char* fmt, ...);

#define LOG_FUNCTION(name, color, level, fmt, args...)                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		name##_inner("\033[1;" #color "m%s\033[0m \033[3;4;" #color "m%s:%d %s:\033[0m\033[" #color "m " fmt           \
					 "\033[0m\n",                                                                                      \
					 DebugLevelStrings[(int)level],                                                                    \
					 __FILE_RELPATH__,                                                                                 \
					 __LINE__,                                                                                         \
					 __FUNCTION__,                                                                                     \
					 ##args);                                                                                          \
	} while (0)

#ifdef DEBUG
#define verbose(fmt, args...) LOG_FUNCTION(verbose, 37, DebugLevel::Verbose, fmt, ##args)
#define dbg(fmt, args...) LOG_FUNCTION(dbg, 34, DebugLevel::Debug, fmt, ##args)
#else
#define verbose(fmt, args...)
#define dbg(fmt, args...)
#endif

#define info(fmt, args...) LOG_FUNCTION(info, 32, DebugLevel::Info, fmt, ##args)
#define warn(fmt, args...) LOG_FUNCTION(warn, 33, DebugLevel::Warn, fmt, ##args)
#define error(fmt, args...) LOG_FUNCTION(error, 31, DebugLevel::Error, fmt, ##args)
#define fatal(fmt, args...) LOG_FUNCTION(fatal, 35, DebugLevel::Fatal, fmt, ##args)

#endif /* JNI_DEBUG_HPP_ */
