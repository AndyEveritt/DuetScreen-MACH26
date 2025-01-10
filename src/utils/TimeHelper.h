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
#if SIMULATION
		return std::chrono::duration_cast<std::chrono::milliseconds>(
				   std::chrono::system_clock::now().time_since_epoch())
			.count();
#else
#warning "TimeHelper::getCurrentTime() not implemented for this platform"
		return 0;
#endif
	}

	static bool setDateTime(const char* dateTime) {}
};
