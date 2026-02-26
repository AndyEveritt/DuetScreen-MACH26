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
#include "UI/Core/Model.h"
#include "utils/StorageHelper.h"
#include "utils/SystemHelper.h"
#include "version.h"
#include <sys/stat.h>

#include <hv/requests.h>
#include <nlohmann/json.hpp>

#include <atomic>
#include <cstring>
#include <fcntl.h>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>

#define USB_BASE_DIR "/media/usb"
#define UPGRADE_EXT ".tar.gz"
static constexpr size_t UPGRADE_EXT_SIZE = sizeof(UPGRADE_EXT) - 1;
#define UPGRADE_FILE UPGRADE_FILE_NAME UPGRADE_EXT
#define TMP_FILEPATH "/tmp/" UPGRADE_FILE
#define BOOT_FILEPATH "/boot/update.tar.gz"

namespace UpgradeHelper
{
	static std::string readFileToString(const char* path);
	static void postUpdateResult(UpgradeResult result);

	namespace
	{
		constexpr const char* kUpgradeSuccessPath = "/tmp/rootfs_upgrade_success";
		constexpr const char* kBuildrootVersionFile = "/etc/buildroot_version";
		constexpr const char* kUpgradeBuildrootVersionFile = "/tmp/update_buildroot_version";
		constexpr const char* kUpgradeFailedPath = "/tmp/rootfs_upgrade_failed";
		constexpr const char* kUpgradePatchWarningPath = "/tmp/rootfs_upgrade_patch_warning";
		std::atomic<bool> g_monitoring{false};
		std::thread g_monitorThread;
	} // namespace

	void startMonitoringUpgradeStatus()
	{
		if (g_monitoring.exchange(true))
		{
			// Already running
			return;
		}

		if (std::filesystem::exists(kUpgradePatchWarningPath))
		{
			LOG_WARN("Upgrade patch warning file exists: {:s}", kUpgradePatchWarningPath);
			postUpdateResult(UpgradeResult::BuildrootVersionWarning);
		}
		else if (std::filesystem::exists(kUpgradeSuccessPath))
		{
			LOG_INFO("Upgrade successful");
			std::filesystem::remove(kUpgradeSuccessPath);
			postUpdateResult(UpgradeResult::Success);
		}

		g_monitorThread = std::thread(
			[]
			{
				tracy::SetThreadName("Upgrade Result Monitor");

				int inotifyFd = inotify_init1(IN_NONBLOCK);
				if (inotifyFd < 0)
				{
					LOG_ERROR("inotify_init1 failed: {:s}", strerror(errno));
					return;
				}
				int wd1 = inotify_add_watch(inotifyFd, kUpgradeFailedPath, IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
				int wd2 =
					inotify_add_watch(inotifyFd, kUpgradePatchWarningPath, IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
				if (wd1 < 0 && wd2 < 0)
				{
					LOG_ERROR("inotify_add_watch failed for both files");
					close(inotifyFd);
					return;
				}
				constexpr size_t bufLen = 1024;
				char buf[bufLen]{};
				while (g_monitoring.load())
				{
					ssize_t len = read(inotifyFd, buf, bufLen);
					if (len > 0)
					{
						ZoneScopedN("Upgrade Result Monitor - inotify event");
						ssize_t i = 0;
						while (i < len)
						{
							struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&buf[i]);
							if (event->wd == wd1)
							{
								LOG_WARN("Upgrade failed: file appeared or changed: {:s}", kUpgradeFailedPath);
								postUpdateResult(UpgradeResult::BuildrootVersionError);
							}
							else if (event->wd == wd2)
							{
								LOG_WARN("Upgrade patch warning: file appeared or changed: {:s}",
										 kUpgradePatchWarningPath);
								postUpdateResult(UpgradeResult::BuildrootVersionWarning);
							}
							i += sizeof(struct inotify_event) + event->len;
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
				}
				if (wd1 >= 0)
					inotify_rm_watch(inotifyFd, wd1);
				if (wd2 >= 0)
					inotify_rm_watch(inotifyFd, wd2);
				close(inotifyFd);
			});
		g_monitorThread.detach();
	}

	std::string_view getBuildrootVersion()
	{
#if SIMULATION
		return "Simulation";
#endif

		static std::string version;
		if (!version.empty())
		{
			return version;
		}

		version = readFileToString(kBuildrootVersionFile);
		return version;
	}

	static std::string readFileToString(const char* path)
	{
		std::ifstream f(path);
		if (!f.is_open())
			return {};
		std::string s;
		std::getline(f, s);
		return s;
	}

	static void postUpdateResult(UpgradeResult result)
	{
		UpgradeInfo info;
		info.result = result;
		info.currentVersion = FIRMWARE_VERSION;
		info.currentBuildrootVersion = getBuildrootVersion();
		info.updateBuildrootVersion = readFileToString(kUpgradeBuildrootVersionFile);
		LOG_DBG("Upgrade result: '{:s}', current version: '{:s}', current buildroot version: '{:s}', update buildroot "
				"version: '{:s}'",
				nameof::nameof_enum(result),
				info.currentVersion,
				info.currentBuildrootVersion,
				info.updateBuildrootVersion);
		Model::get().post<EventType::UpdateResult>(std::move(info));
	}

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
#if SIMULATION
		LOG_INFO("Simulating upgrade...");
		std::this_thread::sleep_for(std::chrono::seconds(2));
		postUpdateResult(UpgradeResult::Success);
		return true;
#endif
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

	bool upgradeFromGithubLatest()
	{
		ZoneScoped;
		LOG_INFO("Attempting upgrade from GitHub latest release");

		removeTmpFile();

		// Query GitHub releases API for releases list and pick the newest prerelease
		HttpRequest req;
		req.method = HTTP_GET;
		req.scheme = "https";
		req.host = "api.github.com";
		req.path = "/repos/Duet3D/DuetScreen/releases"; // returns array, newest first
		req.headers["Accept"] = "application/vnd.github.v3+json";
		req.headers["User-Agent"] = "DuetScreenUpgradeHelper";
		req.timeout = HTTP_TIMEOUT;

		req.DumpUrl();

		hv::HttpClient cli;
		HttpResponse r;
		cli.send(&req, &r);
		if (r.status_code != HTTP_STATUS_OK)
		{
			LOG_ERROR("Failed to fetch releases list: HTTP {:d}", (int)r.status_code);
			return false;
		}

		auto body = nlohmann::json::parse(r.body, nullptr, false);
		if (body.is_discarded() || !body.is_array())
		{
			LOG_ERROR("Failed to parse GitHub releases response or unexpected format");
			return false;
		}

		std::string downloadUrl;
		// Find the first release marked as prerelease that contains the desired asset
		for (const auto& release : body)
		{
			if (!release.is_object())
				continue;
#if 0
			bool isPrerelease = false;
			if (release.contains("prerelease") && release["prerelease"].is_boolean())
				isPrerelease = release["prerelease"].get<bool>();
#endif
			if (!release.contains("assets") || !release["assets"].is_array())
				continue;
			for (const auto& asset : release["assets"])
			{
				if (!asset.is_object() || !asset.contains("name"))
					continue;
				std::string name = asset["name"].get<std::string>();
				if (name == UPGRADE_FILE)
				{
					if (asset.contains("browser_download_url") && asset["browser_download_url"].is_string())
						downloadUrl = asset["browser_download_url"].get<std::string>();
					break;
				}
			}
			if (!downloadUrl.empty())
				break; // found in newest prerelease
		}

		if (downloadUrl.empty())
		{
			LOG_ERROR("No asset named {:s} found in latest release", UPGRADE_FILE);
			return false;
		}

		// Parse download URL (expecting https://host/path)
		const std::string httpsPrefix = "https://";
		if (downloadUrl.rfind(httpsPrefix, 0) == 0)
			downloadUrl = downloadUrl.substr(httpsPrefix.size());
		auto slashPos = downloadUrl.find('/');
		if (slashPos == std::string::npos)
		{
			LOG_ERROR("Invalid download URL: {:s}", downloadUrl);
			return false;
		}
		std::string host = downloadUrl.substr(0, slashPos);
		std::string path = downloadUrl.substr(slashPos);

		HttpRequest req2;
		req2.method = HTTP_GET;
		req2.scheme = "https";
		req2.host = host;
		req2.path = path;
		req2.headers["User-Agent"] = "DuetScreenUpgradeHelper";
		req2.timeout = HTTP_TIMEOUT * 6; // give more time for download

		req2.DumpUrl();

		HttpResponse r2;
		hv::HttpClient cli2;
		cli2.send(&req2, &r2);
		if (r2.status_code != HTTP_STATUS_OK)
		{
			LOG_ERROR("Failed to download asset: HTTP {:d}", (int)r2.status_code);
			return false;
		}

		std::ofstream file(TMP_FILEPATH, std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to create file \"" TMP_FILEPATH "\"");
			return false;
		}
		file.write(r2.body.data(), r2.body.size());
		file.close();
		LOG_DBG("Downloaded asset to " TMP_FILEPATH);

		return upgradeFromTmp();
	}
} // namespace UpgradeHelper
