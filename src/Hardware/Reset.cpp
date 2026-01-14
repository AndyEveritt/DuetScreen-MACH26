/*
 * Reset.cpp
 *
 * Created: 19/02/2023
 * Author: Loïc G.
 *
 */

#include "Reset.h"
#include "Debug.h"
#include "utils/SystemHelper.h"

#include <cstdlib>
#include <filesystem>
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
#if SIMULATION
		exit(EXIT_SUCCESS);
#else
	SystemHelper::restartService(SystemHelper::Services::DUETSCREEN);
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
		if (!std::filesystem::remove("config.json"))
		{
			LOG_ERROR("Failed to remove config.json file");
		}
#else
	if (!std::filesystem::remove("/etc/duetscreen.json"))
	{
		LOG_ERROR("Failed to remove /etc/duetscreen.json file");
	}
#endif

		Restart();
	}

#ifdef __cplusplus
}
#endif
