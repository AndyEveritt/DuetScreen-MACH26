/*
 * SystemHelper.h
 *
 *  Created on: 2026-01-14
 *      Author: Andy Everitt
 */

#pragma once

namespace SystemHelper
{
	enum class Services
	{
		DUETSCREEN,
        UPGRADE,
#if DEVELOPER_MODE
		DUETSCREEN_MONITOR,
		ADB,
		SSH,
		SETUP
#endif
	};

	bool startService(const Services service);
	bool stopService(const Services service);
	bool restartService(const Services service);
	bool enableService(const Services service);
	bool disableService(const Services service);
	bool isServiceRunning(const Services service);
	bool isServiceEnabled(const Services service);
} // namespace SystemHelper
