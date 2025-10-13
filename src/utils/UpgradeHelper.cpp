/*
 * UpgradeHelper.cpp
 *
 *  Created on: 2025-03-04
 *      Author: Andy Everitt
 */

#include "UpgradeHelper.h"
#include "Configuration.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "Hardware/Reset.h"
#include "utils/StorageHelper.h"
#include <sys/stat.h>

#define USB_BASE_DIR "/media/usb"
#define UPGRADE_EXT ".tar.gz"
static constexpr size_t UPGRADE_EXT_SIZE = sizeof(UPGRADE_EXT) - 1;
#define UPGRADE_FILE UPGRADE_FILE_NAME UPGRADE_EXT
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
		if (filePath.rfind(USB_BASE_DIR, 0) != 0)
		{
			LOG_ERROR("File path {:s} is not on USB", filePath.c_str());
			return false;
		}

		// Check if the file has the correct extension
		if (filePath.size() < UPGRADE_EXT_SIZE || filePath.substr(filePath.size() - UPGRADE_EXT_SIZE) != UPGRADE_EXT)
		{
			LOG_ERROR("File {:s} does not have the required " UPGRADE_EXT " extension", filePath.c_str());
			return false;
		}

		struct stat sb;
		if (stat(filePath.c_str(), &sb) == -1)
		{
			LOG_ERROR("Failed to get file stats for {:s}", filePath.c_str());
			return false;
		}

		removeTmpFile(); // Remove any previous upgrade file
		int ret = system(fmt::format("cp \"{:s}\" " TMP_FILEPATH, filePath).c_str());
		if (ret != 0)
		{
			LOG_ERROR("Failed to copy file \"{:s}\" to /tmp, code={:d}", filePath.c_str(), ret);
			return false;
		}
		LOG_DBG("File \"{:s}\" copied to " TMP_FILEPATH ", code={:d}", filePath.c_str(), ret);
		return true;
	}

	static bool moveTmpFileToBoot()
	{
		struct stat sb;
		if (system("rm " BOOT_FILEPATH) != 0)
		{
			LOG_WARN("Failed to remove previous upgrade file at " BOOT_FILEPATH);
		}
		if (stat(TMP_FILEPATH, &sb) == -1)
		{
			LOG_ERROR("Failed to get file stats for " TMP_FILEPATH);
			return false;
		}
		int ret = system("mv " TMP_FILEPATH " " BOOT_FILEPATH);
		if (ret != 0)
		{
			LOG_ERROR("Failed to move file from " TMP_FILEPATH " to " BOOT_FILEPATH ", code={:d}", ret);
			return false;
		}
		LOG_DBG("File moved from " TMP_FILEPATH " to " BOOT_FILEPATH ", code={:d}", ret);
		return true;
	}

	static bool createUpgradeCompleteFile(const std::string& usbPath)
	{
		if (usbPath.rfind(USB_BASE_DIR, 0) != 0)
		{
			LOG_ERROR("File path {:s} is not on USB", usbPath.c_str());
			return false;
		}

		FILE* file = fopen((usbPath + "/upgraded").c_str(), "w");
		if (file == nullptr)
		{
			LOG_ERROR("Failed to create upgrade complete file");
			return false;
		}
		fclose(file);
		return true;
	}

	static bool upgradeFromTmp()
	{
		if (!moveTmpFileToBoot())
		{
			LOG_ERROR("Failed to move file to boot partition");
			return false;
		}

		int ret = system("/etc/init.d/S02setup start");
		return ret == 0;
	}

	bool upgradeFromUSB(const std::string& filePath)
	{
		LOG_INFO("Attempting upgrade from USB file {:s}", filePath.c_str());
		if (!copyFileFromUsb(filePath))
		{
			LOG_ERROR("Failed to copy file from USB");
			return false;
		}

		struct stat file_stat;
		if (stat(filePath.c_str(), &file_stat) != 0)
		{
			LOG_ERROR("Error getting file stats for {:s}", filePath.c_str());
			return false;
		}

		time_t lastModified = file_stat.st_mtime;
		StorageHelper::setData(ID_UPGRADE_FILE_LAST_MODIFIED, lastModified);

		createUpgradeCompleteFile(filePath.substr(0, filePath.find_last_of('/')));
		return upgradeFromTmp();
	}

	bool upgradeFromDuet()
	{
		std::string filePath = "/firmware/" UPGRADE_FILE;

		LOG_INFO("Attempting upgrade from Duet file {:s}", filePath.c_str());
		removeTmpFile(); // Remove any previous upgrade file

		std::string contents;
		if (!Comm::DUET.DownloadFile(filePath.c_str(), contents))
		{
			LOG_ERROR("Failed to download file \"{:s}\" from Duet", filePath.c_str());
			return false;
		}

		std::ofstream file(TMP_FILEPATH, std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to create file \"" TMP_FILEPATH "\"");
			return false;
		}
		file.write(contents.c_str(), contents.size());
		file.close();

		return upgradeFromTmp();
	}
} // namespace UpgradeHelper
