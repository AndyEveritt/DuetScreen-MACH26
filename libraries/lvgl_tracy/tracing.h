/*
 * tracing.h
 *
 *  Created on: 2026-01-03
 *      Author: Andy Everitt
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <tracy/TracyC.h>

#if defined TRACY_ENABLE && TRACY_ENABLE

#  define LvZoneStart TracyCZone(tracy_ctx, 1)
#  define LvZoneEnd TracyCZoneEnd(tracy_ctx)
#  define LvZoneStartTag(tag) _lv_tracy_start_zone(tag, __func__, __FILE__, __LINE__, 0)
#  define LvZoneEndTag(tag) _lv_tracy_end_zone(tag)

#  ifdef __cplusplus
extern "C"
{
#  endif

	void _lv_tracy_start_zone(const char* tag, const char* func, const char* file, uint32_t line, uint32_t color);
	void _lv_tracy_end_zone(const char* tag);

#  ifdef __cplusplus
} // extern "C"
#  endif

#else

#  define LvZoneStart
#  define LvZoneEnd
#  define LvZoneStartTag(tag)
#  define LvZoneEndTag(tag)

#endif
