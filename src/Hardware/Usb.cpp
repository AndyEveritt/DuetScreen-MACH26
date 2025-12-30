/*
 * Usb.cpp
 *
 *  Created on: 6 Feb 2024
 *      Author: andy
 */

#include "Usb.h"
#include "sys/stat.h"
#include "tracy/Tracy.hpp"
#include "utils/utils.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <sstream>

namespace USB
{
#if 1
	std::vector<FileInfo> ListEntriesInDirectory(const std::string& directoryPath)
	{
		ZoneScoped;
		std::vector<FileInfo> files;

		// Open the directory
		DIR* dir = opendir(directoryPath.c_str());
		if (dir == nullptr)
		{
			LOG_ERROR("Error opening directory {:s}", directoryPath.c_str());
			return files;
		}

		// Read the directory entries
		dirent* entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			std::string name = entry->d_name;
			if (name == "." || name == ".." || name == "System Volume Information")
			{
				continue;
			}

			// Add files to the vector
			if (entry->d_type == DT_REG || entry->d_type == DT_DIR)
			{ // Regular file or folder
				FileInfo info;
				struct stat sb;
				if (stat((directoryPath + "/" + entry->d_name).c_str(), &sb) == -1)
				{
					LOG_ERROR("Failed to get file stats for {:s}", (directoryPath + entry->d_name).c_str());
				}
				strncpy(info.d_name, entry->d_name, 256);
				info.d_type = entry->d_type;
				info.st_size = sb.st_size;
				info.st_blksize = sb.st_blksize;
				info.st_blocks = sb.st_blocks;
#  ifndef __APPLE__
				info.st_atim = sb.st_atim;
				info.st_ctim = sb.st_ctim;
				info.st_mtim = sb.st_mtim;
#  endif
				files.push_back(info);
			}
		}

		// Close the directory
		closedir(dir);

		return files;
	}

	bool ReadUsbFileContents(const std::string& filePath, std::string& contents)
	{
		ZoneScoped;
		std::string fullPath;
		if (filePath.rfind("/mnt/usb") == 0)
		{
			fullPath = filePath;
		}
		else
		{
			fullPath = std::string("/mnt/usb0/") + filePath;
		}

		return ReadFileContents(fullPath, contents);
	}

	bool ReadFileContents(const std::string& filePath, std::string& contents)
	{
		ZoneScoped;
		LOG_INFO("Reading file {:s}", filePath.c_str());
		std::ifstream file(filePath.c_str(), std::ios::in | std::ios::ate);

		if (!file.is_open())
		{
			LOG_ERROR("Unable to open file {:s}", filePath.c_str());
			return false;
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		contents.resize(size);
		LOG_DBG("Reading {:d} bytes", size);
		if (!file.read(&contents[0], size))
		{
			LOG_ERROR("Failed to read file {:s}", filePath.c_str());
			return false;
		}
		LOG_DBG("Read {:d} bytes", contents.size());
		return true;
	}
#endif

	UsbMonitor::~UsbMonitor()
	{
		ZoneScoped;
		stopMonitoring();
	}

	void UsbMonitor::startMonitoring()
	{
		ZoneScoped;
		if (!running)
		{
			running = true;
			monitor_thread = std::thread(&UsbMonitor::monitorThread, this);
		}
	}

	void UsbMonitor::stopMonitoring()
	{
		ZoneScoped;
		if (running)
		{
			running = false;
			if (monitor_thread.joinable())
			{
				monitor_thread.join();
			}
		}
	}

	void UsbMonitor::registerCallback(UsbDriveCallback callback, bool initialNotify)
	{
		ZoneScoped;
		std::lock_guard<std::mutex> lock(callback_mutex);
		callbacks.push_back(callback);

		if (running && initialNotify)
		{
			const auto mounts = getMountedDrives();
			for (const auto& mount : mounts)
			{
				callback(mount, true);
			}
		}
	}

	std::vector<std::string> UsbMonitor::getMountedDrives() const
	{
		ZoneScoped;
		return current_mounts;
	}

	std::vector<std::string> UsbMonitor::getUsbMounts()
	{
		ZoneScoped;
		std::vector<std::string> mounts;
		FILE* fp = popen("grep \"/media/usb\" /proc/mounts | awk '{print $2}'", "r");
		if (fp == nullptr)
		{
			LOG_ERROR("Failed to run mount point detection command");
			return mounts;
		}

		char path[256];
		while (fgets(path, sizeof(path), fp) != nullptr)
		{
			// Remove newline if present
			size_t len = strlen(path);
			if (len > 0 && path[len - 1] == '\n')
			{
				path[len - 1] = '\0';
			}
			mounts.push_back(path);
		}

		pclose(fp);
		return mounts;
	}

	void UsbMonitor::notifyCallbacks(const std::string& path, bool connected)
	{
		ZoneScoped;
		std::lock_guard<std::mutex> lock(callback_mutex);
		for (const auto& callback : callbacks)
		{
			callback(path, connected);
		}
	}

	void UsbMonitor::monitorThread()
	{
		tracy::SetThreadName("USB Monitor Thread");
		LOG_VERBOSE("USB monitor thread started");

		while (running)
		{
			{
				ZoneScoped;
				auto new_mounts = getUsbMounts();

				const auto old_mounts = getMountedDrives();
				{
					std::lock_guard<std::mutex> lock(callback_mutex);
					current_mounts = new_mounts;
				}

				// Find new mounts
				for (const auto& mount : new_mounts)
				{
					if (std::find(old_mounts.begin(), old_mounts.end(), mount) == old_mounts.end())
					{
						LOG_INFO("USB drive mounted at: {:s}", mount.c_str());
						notifyCallbacks(mount, true);
					}
				}

				// Find removed mounts
				for (const auto& mount : old_mounts)
				{
					if (std::find(new_mounts.begin(), new_mounts.end(), mount) == new_mounts.end())
					{
						LOG_INFO("USB drive unmounted from: {:s}", mount.c_str());
						notifyCallbacks(mount, false);
					}
				}
			}

			// Sleep for a bit to avoid excessive CPU usage
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		LOG_VERBOSE("USB monitor thread stopped");
	}
} // namespace USB
