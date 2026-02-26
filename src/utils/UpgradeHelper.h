/*
 * UpgradeHelper.h
 *
 *  Created on: 2025-03-04
 *      Author: Andy Everitt
 */

#pragma once

#include <string>

namespace UpgradeHelper
{
	enum class UpgradeResult
	{
		Success = 0,
		BuildrootVersionError,
		BuildrootVersionWarning,
	};

	struct UpgradeInfo
	{
		UpgradeResult result;
		std::string currentVersion;
		std::string currentBuildrootVersion;
		std::string updateBuildrootVersion;
	};

	bool upgradeFromUSB(const std::string& filePath);
	bool upgradeFromDuet();
	bool upgradeFromGithubLatest();

	// Start monitoring for upgrade failure/warning files
	void startMonitoringUpgradeStatus();
	std::string_view getBuildrootVersion();
} // namespace UpgradeHelper
