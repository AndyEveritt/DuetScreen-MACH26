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
	bool upgradeFromUSB(const std::string& filePath);
	bool upgradeFromDuet();
} // namespace UpgradeHelper
