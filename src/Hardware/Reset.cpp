/*
 * Reset.cpp
 *
 * Created: 19/02/2023
 * Author: Loïc G.
 *
 */

#include "Reset.h"
#include "Debug.h"

#include <cstdlib>
#include <sys/reboot.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * @brief Restart the program
	 */
	void Restart() noexcept
	{
#if !SIMULATION
		system("/etc/init.d/S20DuetScreen restart");
#endif
	}

	/**
	 * @brief Reboot the hardware
	 */
	void Reboot() noexcept
	{
#if !SIMULATION
		reboot(RB_AUTOBOOT);
#endif
	}

	/**
	 * @brief Reset any user customizations and reset the hardware
	 */
	void EraseAndRestart() noexcept
	{
#if SIMULATION
		if (system("rm -rf config.json") != 0)
		{
			LOG_ERROR("Failed to remove config.json file");
		}
#else
	if (system("rm -rf /etc/duetscreen.json") != 0)
	{
		LOG_ERROR("Failed to remove /etc/duetscreen.json file");
	}
#endif

		Restart();
	}

#ifdef __cplusplus
}
#endif
