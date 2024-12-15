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
	"[V]",
	"[D]",
	"[I]",
	"[W]",
	"[E]",
	"[F]",
};

#define __dbg(fmt, ...)                                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		printf(fmt, ##__VA_ARGS__);                                                                                    \
	} while (0)

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
		verbose_inner(                                                                                                 \
			"%s %s %s(%d): " fmt, DebugLevelStrings[(int)DebugLevel::Verbose], __FUNCTION__, __LINE__, ##args);        \
	} while (0)
#define dbg(fmt, args...)                                                                                              \
	do                                                                                                                 \
	{                                                                                                                  \
		dbg_inner("%s %s %s(%d): " fmt, DebugLevelStrings[(int)DebugLevel::Debug], __FUNCTION__, __LINE__, ##args);    \
	} while (0)
#else
#define verbose(fmt, args...)
#define dbg(fmt, args...)
#endif

#define info(fmt, args...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		info_inner("%s %s %s(%d): " fmt, DebugLevelStrings[(int)DebugLevel::Info], __FUNCTION__, __LINE__, ##args);    \
	} while (0)
#define warn(fmt, args...)                                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		warn_inner("%s %s %s(%d): " fmt, DebugLevelStrings[(int)DebugLevel::Warn], __FUNCTION__, __LINE__, ##args);    \
	} while (0)
#define error(fmt, args...)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		error_inner("%s %s %s(%d): " fmt, DebugLevelStrings[(int)DebugLevel::Error], __FUNCTION__, __LINE__, ##args);  \
	} while (0)
#define fatal(fmt, args...)                                                                                            \
	do                                                                                                                 \
	{                                                                                                                  \
		fatal_inner("%s %s %s(%d): " fmt, DebugLevelStrings[(int)DebugLevel::Fatal], __FUNCTION__, __LINE__, ##args);  \
	} while (0)

#endif /* JNI_DEBUG_HPP_ */
