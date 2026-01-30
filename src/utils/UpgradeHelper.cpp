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
#include "utils/SystemHelper.h"
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
		ZoneScoped;
		std::error_code ec;
		std::filesystem::remove(TMP_FILEPATH, ec);
		if (ec)
		{
			LOG_ERROR("Failed to remove temporary upgrade file " TMP_FILEPATH ": {:s}", ec.message());
			return false;
		}
		return true;
	}

	static bool copyFileFromUsb(const std::string& filePath)
	{
		ZoneScoped;
		if (filePath.rfind(USB_BASE_DIR, 0) != 0)
		{
			LOG_ERROR("File path {:s} is not on USB", filePath.c_str());
			return false;
		}

		// Check if the file has the correct extension
		if (filePath.size() < UPGRADE_EXT_SIZE || filePath.substr(filePath.size() - UPGRADE_EXT_SIZE) != UPGRADE_EXT)
		{
			LOG_ERROR("File {:s} does not have the required " UPGRADE_EXT " extension", filePath);
			return false;
		}

		struct stat sb;
		if (stat(filePath.c_str(), &sb) == -1)
		{
			LOG_ERROR("Failed to get file stats for {:s}", filePath);
			return false;
		}

		removeTmpFile(); // Remove any previous upgrade file
		if (!std::filesystem::copy_file(filePath, TMP_FILEPATH))
		{
			LOG_ERROR("Failed to copy file \"{:s}\" to /tmp", filePath);
			return false;
		}
		LOG_DBG("File \"{:s}\" copied to " TMP_FILEPATH, filePath);
		return true;
	}

	static bool moveTmpFileToBoot()
	{
		ZoneScoped;
		struct stat sb;
		std::filesystem::remove(BOOT_FILEPATH);
		if (stat(TMP_FILEPATH, &sb) == -1)
		{
			LOG_ERROR("Failed to get file stats for " TMP_FILEPATH);
			return false;
		}
		std::filesystem::copy_file(TMP_FILEPATH, BOOT_FILEPATH);
		std::filesystem::remove(TMP_FILEPATH);
		LOG_DBG("File moved from " TMP_FILEPATH " to " BOOT_FILEPATH);
		return true;
	}

	static bool createUpgradeCompleteFile(const std::string& usbPath)
	{
		ZoneScoped;
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
		ZoneScoped;
		if (!moveTmpFileToBoot())
		{
			LOG_ERROR("Failed to move file to boot partition");
			return false;
		}

		return SystemHelper::restartService(SystemHelper::Services::UPGRADE);
	}

	bool upgradeFromUSB(const std::string& filePath)
	{
		ZoneScoped;
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
		ZoneScoped;
		std::string filePath = "/firmware/" UPGRADE_FILE;

		LOG_INFO("Attempting upgrade from Duet file {:s}", filePath.c_str());
		removeTmpFile(); // Remove any previous upgrade file

		if (!Comm::DUET.DownloadFile(filePath.c_str(),
									 [](const std::string& contents)
									 {
										 std::ofstream file(TMP_FILEPATH, std::ios::binary);
										 if (!file.is_open())
										 {
											 LOG_ERROR("Failed to create file \"" TMP_FILEPATH "\"");
										 }
										 file.write(contents.c_str(), contents.size());
										 file.close();

										 upgradeFromTmp();
									 }))
		{
			LOG_ERROR("Failed to download file \"{:s}\" from Duet", filePath.c_str());
			return false;
		}

		return true;
	}
} // namespace UpgradeHelper
