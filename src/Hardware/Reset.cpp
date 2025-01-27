/*
 * Reset.cpp
 *
 * Created: 19/02/2023
 * Author: Loïc G.
 *
 */

#include "Reset.h"

#include <sys/reboot.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * @brief Restart the hardware
	 */
	void Reset() noexcept {}

	/**
	 * @brief Reset any user customizations and reset the hardware
	 */
	void EraseAndReset() noexcept {}

#ifdef __cplusplus
}
#endif
