/*
 * tracing.cpp
 *
 *  Created on: 2026-01-03
 *      Author: Andy Everitt
 */

#include "tracing.h"
#include <string_view>
#include <vector>

struct ZoneInfo
{
	std::string_view tag;
	TracyCZoneCtx ctx;
};

static std::vector<ZoneInfo> s_zones;

extern "C" void _lv_tracy_start_zone(const char* tag, const char* func, const char* file, uint32_t line, uint32_t color)
{
	if (tag == NULL)
	{
		return;
	}

	std::string_view key = tag;
	// Same as TracyCZoneNC but with custom parameters so file, func, line can be passed in from caller
	static const struct ___tracy_source_location_data source_info = {NULL, func, file, line, color};
	TracyCZoneCtx ctx = ___tracy_emit_zone_begin_callstack(&source_info, TRACY_CALLSTACK, 1);
	TracyCZoneName(ctx, tag, key.size());

	if (ctx.active)
	{
		s_zones.emplace_back(ZoneInfo{std::string_view(tag), ctx});
	}
}

extern "C" void _lv_tracy_end_zone(const char* tag)
{
	if (tag == NULL)
	{
		return;
	}

	std::string_view key = tag;

	for (auto it = s_zones.rbegin(); it != s_zones.rend(); ++it)
	{
		if (it->tag == key)
		{
			TracyCZoneEnd(it->ctx);
			auto fwd = it.base();
			--fwd;
			s_zones.erase(fwd);
			break;
		}
	}
	return;
}