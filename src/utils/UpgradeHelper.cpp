/*
 * UpgradeHelper.cpp
 *
 *  Created on: 2025-03-04
 *      Author: Andy Everitt
 */

#include "UpgradeHelper.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "Hardware/Reset.h"
#include "utils/StorageHelper.h"
#include <sys/stat.h>

#define UPGRADE_FILE "DuetScreen.tar.gz"
#define TMP_FILEPATH "/tmp/" UPGRADE_FILE
#define BOOT_FILEPATH "/boot/update.tar.gz"

namespace UpgradeHelper
{
	static bool removeTmpFile()
	{
		return system("rm " TMP_FILEPATH) == 0;
	}

	static bool copyFileFromUsb(const std::string& filePath)
	{
		struct stat sb;
		if (stat((std::string("/mnt/usb1/") + filePath).c_str(), &sb) == -1)
		{
			error("Failed to get file stats for %s", filePath.c_str());
			return false;
		}

		removeTmpFile(); // Remove any previous upgrade file
		int ret = system(utils::format("cd /mnt/usb1 && cp \"%s\" " TMP_FILEPATH, filePath.c_str()).c_str());
		if (ret != 0)
		{
			error("Failed to copy file \"%s\" to /tmp, code=%d", filePath.c_str(), ret);
			return false;
		}
		dbg("File \"%s\" copied to " TMP_FILEPATH ", code=%d", filePath.c_str(), ret);
		return true;
	}

	static bool moveTmpFileToBoot()
	{
		struct stat sb;
		system("rm " BOOT_FILEPATH); // Remove any previous upgrade file
		if (stat(TMP_FILEPATH, &sb) == -1)
		{
			error("Failed to get file stats for " TMP_FILEPATH);
			return false;
		}
		int ret = system("mv " TMP_FILEPATH " " BOOT_FILEPATH);
		if (ret != 0)
		{
			error("Failed to move file from " TMP_FILEPATH " to " BOOT_FILEPATH ", code=%d", ret);
			return false;
		}
		dbg("File moved from " TMP_FILEPATH " to " BOOT_FILEPATH ", code=%d", ret);
		return true;
	}

	static bool upgradeFromTmp()
	{
		if (!moveTmpFileToBoot())
		{
			error("Failed to move file to boot partition");
			return false;
		}

		int ret = system("/etc/init.d/S02setup start");
		return ret == 0;
	}

	bool upgradeFromUSB(const std::string& filePath)
	{
		info("Attempting upgrade from USB file %s", filePath.c_str());
		if (!copyFileFromUsb(filePath))
		{
			error("Failed to copy file from USB");
			return false;
		}
		return upgradeFromTmp();
	}

	bool upgradeFromDuet()
	{
		std::string filePath = "/firmware/" UPGRADE_FILE;

		info("Attempting upgrade from Duet file %s", filePath.c_str());
		removeTmpFile(); // Remove any previous upgrade file

		std::string contents;
		if (!Comm::DUET.DownloadFile(filePath.c_str(), contents))
		{
			error("Failed to download file \"%s\" from Duet", filePath.c_str());
			return false;
		}

		std::ofstream file(TMP_FILEPATH, std::ios::binary);
		if (!file.is_open())
		{
			error("Failed to create file \"" TMP_FILEPATH "\"");
			return false;
		}
		file.write(contents.c_str(), contents.size());
		file.close();

		return upgradeFromTmp();
	}
} // namespace UpgradeHelper
