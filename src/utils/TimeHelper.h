#pragma once

#include <chrono>
#include <fmt/chrono.h>

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
	 * @brief Get the difference between current time and the given time in milliseconds
	 * @param ms
	 * @return std::chrono::milliseconds
	 */
	static std::chrono::milliseconds getTimeSince(std::chrono::milliseconds ms) { return getCurrentTime() - ms; }

	static bool setDateTime(const char* /* dateTime */) { return true; }
};
