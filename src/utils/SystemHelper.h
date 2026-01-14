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

	bool startService(Services service);
	bool stopService(Services service);
	bool restartService(Services service);
	bool enableService(Services service);
	bool disableService(Services service);
	bool isServiceRunning(Services service);
	bool isServiceEnabled(Services service);
} // namespace SystemHelper
