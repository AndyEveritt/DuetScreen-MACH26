/*
 * Debug.cpp
 *
 *  Created on: 10 May 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "Storage.h"

static DebugLevel s_debugLevel = DebugLevel::Info;

void SetDebugLevel(DebugLevel level)
{
	// TODO save debug level
	s_debugLevel = level;
}

const DebugLevel& GetDebugLevel()
{
	return s_debugLevel;
}

void verbose_inner(const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Verbose)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(fmt, args);
	va_end(args);
}

void dbg_inner(const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Debug)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(fmt, args);
	va_end(args);
}

void info_inner(const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Info)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(fmt, args);
	va_end(args);
}

void warn_inner(const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Warn)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(fmt, args);
	va_end(args);
}

void error_inner(const char* fmt, ...)
{
	if (GetDebugLevel() > DebugLevel::Error)
		return;
	va_list args;
	va_start(args, fmt);
	__dbg(fmt, args);
	va_end(args);
}

void fatal_inner(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	__dbg(fmt, args);
	va_end(args);
}