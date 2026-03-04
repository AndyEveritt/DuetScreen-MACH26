#pragma once

#include "utils/utils.h"
#include <chrono>
#include <ctime>
#include <errno.h>
#include <fmt/chrono.h>
#include <string_view>
#include <time.h>

class TimeHelper
{
  public:
	/**
	 * @brief Get the current date and time in milliseconds
	 */
	static auto getCurrentTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
	}

	/**
	 * @brief Get the current running time in milliseconds
	 * @return time since power on in milliseconds
	 */
	static auto getRunningTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now().time_since_epoch());
	}

	/**
	 * @brief Get the difference between current time and the given time in milliseconds
	 * @param ms
	 * @return std::chrono::milliseconds
	 */
	static std::chrono::milliseconds getTimeSince(std::chrono::milliseconds ms) { return getRunningTime() - ms; }

	static bool setDateTime(std::string_view dateTimeStr)
	{
#if T113
		// Accept formats:
		//  - "YYYY-MM-DD HH:MM:SS"
		//  - "YYYY-MM-DDTHH:MM:SS"
		//  - Optional timezone: "Z" or "+/-HH:MM" or "+/-HH"
		// Returns false on parse or system time set failure.

		auto trim = [](std::string_view s) noexcept -> std::string_view
		{
			// Trim leading/trailing spaces
			std::size_t start = 0;
			std::size_t end = s.size();
			while (start < end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\n' || s[start] == '\r'))
				++start;
			while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\n' || s[end - 1] == '\r'))
				--end;
			return s.substr(start, end - start);
		};

		auto parse_uint = [](std::string_view s) noexcept -> int
		{
			int v = 0;
			if (s.empty())
				return -1;
			for (char c : s)
			{
				if (c < '0' || c > '9')
					return -1;
				v = v * 10 + (c - '0');
			}
			return v;
		};

		dateTimeStr = trim(dateTimeStr);
		if (dateTimeStr.size() < 19)
		{
			return false;
		}

		// Locate date/time separator (space or 'T')
		std::size_t sepPos = dateTimeStr.find(' ');
		if (sepPos == std::string_view::npos)
			sepPos = dateTimeStr.find('T');
		if (sepPos == std::string_view::npos)
			return false;

		const std::string_view datePart = dateTimeStr.substr(0, sepPos);
		std::string_view timeAndTz = dateTimeStr.substr(sepPos + 1);
		timeAndTz = trim(timeAndTz);

		// Parse date: YYYY-MM-DD
		std::size_t y1 = 0;
		std::size_t y2 = datePart.find('-');
		if (y2 == std::string_view::npos)
			return false;
		std::size_t m2 = datePart.find('-', y2 + 1);
		if (m2 == std::string_view::npos)
			return false;

		int year = parse_uint(datePart.substr(y1, y2 - y1));
		int month = parse_uint(datePart.substr(y2 + 1, m2 - (y2 + 1)));
		int day = parse_uint(datePart.substr(m2 + 1));
		if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31)
			return false;

		// Parse time: HH:MM:SS then optional timezone
		std::size_t t1 = 0;
		std::size_t t2 = timeAndTz.find(':');
		if (t2 == std::string_view::npos)
			return false;
		std::size_t t3 = timeAndTz.find(':', t2 + 1);
		if (t3 == std::string_view::npos)
			return false;

		int hour = parse_uint(timeAndTz.substr(t1, t2 - t1));
		int minute = parse_uint(timeAndTz.substr(t2 + 1, t3 - (t2 + 1)));
		// Find end of seconds (before timezone suffix)
		std::size_t tzStart = timeAndTz.find_first_of("Zz+-", t3 + 1);
		std::string_view secPart = (tzStart == std::string_view::npos) ? timeAndTz.substr(t3 + 1)
																	   : timeAndTz.substr(t3 + 1, tzStart - (t3 + 1));
		secPart = trim(secPart);
		int second = parse_uint(secPart);
		if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 60)
			return false;

		// Timezone offset (in minutes). Default: no TZ -> treat as local time.
		bool hasTimezone = false;
		int tzOffsetMinutes = 0; // Positive offset means local ahead of UTC.
		if (tzStart != std::string_view::npos)
		{
			hasTimezone = true;
			char tzSign = timeAndTz[tzStart];
			if (tzSign == 'Z' || tzSign == 'z')
			{
				// UTC
				hasTimezone = true;
				tzOffsetMinutes = 0;
			}
			else if (tzSign == '+' || tzSign == '-')
			{
				const std::string_view tz = timeAndTz.substr(tzStart + 1);
				std::size_t colon = tz.find(':');
				int tzh = 0;
				int tzm = 0;
				if (colon == std::string_view::npos)
				{
					// "+HH"
					tzh = parse_uint(tz);
					if (tzh < 0 || tzh > 23)
						return false;
				}
				else
				{
					// "+HH:MM"
					tzh = parse_uint(tz.substr(0, colon));
					tzm = parse_uint(tz.substr(colon + 1));
					if (tzh < 0 || tzh > 23 || tzm < 0 || tzm > 59)
						return false;
				}
				tzOffsetMinutes = tzh * 60 + tzm;
				if (tzSign == '-')
					tzOffsetMinutes = -tzOffsetMinutes;
			}
			else
			{
				return false; // Invalid TZ suffix
			}
		}

		std::tm tm{};
		tm.tm_year = year - 1900;
		tm.tm_mon = month - 1;
		tm.tm_mday = day;
		tm.tm_hour = hour;
		tm.tm_min = minute;
		tm.tm_sec = second;

		// Convert to epoch seconds.
		// If timezone provided, interpret tm as that zone relative to UTC.
		// For UTC/Z or explicit offset: use timegm (UTC) then adjust by offset.
		time_t epoch = 0;
		if (hasTimezone)
		{
			// timegm interprets tm as UTC.
			epoch = timegm(&tm);
			if (epoch == static_cast<time_t>(-1))
			{
				return false;
			}
			// Adjust epoch to real UTC by subtracting the local offset from UTC.
			// Example: "2024-01-01T12:00:00+02:00" means local is UTC+2; UTC is local - 2h.
			epoch -= static_cast<time_t>(tzOffsetMinutes * 60);
		}
		else
		{
			// No timezone info: treat as local time.
			epoch = mktime(&tm);
			if (epoch == static_cast<time_t>(-1))
			{
				return false;
			}
		}

		// Set system time.
		struct timespec ts;
		ts.tv_sec = epoch;
		ts.tv_nsec = 0;
		if (clock_settime(CLOCK_REALTIME, &ts) != 0)
		{
			return false;
		}
#else
		UNUSED(dateTimeStr);
#endif
		return true;
	}
};
