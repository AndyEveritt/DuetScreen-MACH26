#pragma once

#include <chrono>

class TimeHelper
{
  public:
	/**
	 * @brief Get the current date and time in milliseconds
	 */
	static int64_t getCurrentTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
				   std::chrono::system_clock::now().time_since_epoch())
			.count();
	}

	/**
	 * @brief Get the difference between current time and the given time in milliseconds
	 * @param ms
	 * @return int64_t
	 */
	static int64_t getTimeSince(int64_t ms) { return getCurrentTime() - ms; }

	static bool setDateTime(const char* dateTime) { return true; }
};
