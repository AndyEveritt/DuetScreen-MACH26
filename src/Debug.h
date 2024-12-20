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

#ifdef DEBUG
#define verbose(fmt, args...)                                                                                          \
	do                                                                                                                 \
	{                                                                                                                  \
		verbose_inner("\033[1;37m%s\033[0m \033[3;4;37m%s %s(%d):\033[0m\033[37m " fmt "\033[0m\n",                    \
					  DebugLevelStrings[(int)DebugLevel::Verbose],                                                     \
					  __FILE_RELPATH__,                                                                                \
					  __FUNCTION__,                                                                                    \
					  __LINE__,                                                                                        \
					  ##args);                                                                                         \
	} while (0)
#define dbg(fmt, args...)                                                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		dbg_inner("\033[1;34m%s\033[0m \033[3;4;34m%s %s(%d):\033[0m\033[34m " fmt "\033[0m\n",                        \
				  DebugLevelStrings[(int)DebugLevel::Debug],                                                           \
				  __FILE_RELPATH__,                                                                                    \
				  __FUNCTION__,                                                                                        \
				  __LINE__,                                                                                            \
				  ##args);                                                                                             \
	} while (0)
#else
#define verbose(fmt, args...)
#define dbg(fmt, args...)
#endif

#define info(fmt, args...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		info_inner("\033[1;32m%s\033[0m \033[3;4;32m%s %s(%d):\033[0m\033[32m " fmt "\033[0m\n",                       \
				   DebugLevelStrings[(int)DebugLevel::Info],                                                           \
				   __FILE_RELPATH__,                                                                                   \
				   __FUNCTION__,                                                                                       \
				   __LINE__,                                                                                           \
				   ##args);                                                                                            \
	} while (0)
#define warn(fmt, args...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		warn_inner("\033[1;33m%s\033[0m \033[3;4;33m%s %s(%d):\033[0m\033[33m " fmt "\033[0m\n",                       \
				   DebugLevelStrings[(int)DebugLevel::Warn],                                                           \
				   __FILE_RELPATH__,                                                                                   \
				   __FUNCTION__,                                                                                       \
				   __LINE__,                                                                                           \
				   ##args);                                                                                            \
	} while (0)
#define error(fmt, args...)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		error_inner("\033[1;31m%s\033[0m \033[3;4;31m%s %s(%d):\033[0m\033[31m " fmt "\033[0m\n",                      \
					DebugLevelStrings[(int)DebugLevel::Error],                                                         \
					__FILE_RELPATH__,                                                                                  \
					__FUNCTION__,                                                                                      \
					__LINE__,                                                                                          \
					##args);                                                                                           \
	} while (0)
#define fatal(fmt, args...)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		fatal_inner("\033[1;35m%s\033[0m \033[3;4;35m%s %s(%d):\033[0m\033[35m " fmt "\033[0m\n",                      \
					DebugLevelStrings[(int)DebugLevel::Fatal],                                                         \
					__FILE_RELPATH__,                                                                                  \
					__FUNCTION__,                                                                                      \
					__LINE__,                                                                                          \
					##args);                                                                                           \
	} while (0)

#endif /* JNI_DEBUG_HPP_ */
