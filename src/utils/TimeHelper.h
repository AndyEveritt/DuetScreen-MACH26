#pragma once

#include <chrono>

class TimeHelper
{
  public:
	/**
	 * @brief Get the current date and time in milliseconds
	 */
	static long long getCurrentTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
				   std::chrono::system_clock::now().time_since_epoch())
			.count();
	}

	static bool setDateTime(const char* dateTime) { return true; }
};
