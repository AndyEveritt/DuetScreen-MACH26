/*
 * Usb.h
 *
 *  Created on: 6 Feb 2024
 *      Author: andy
 */

#ifndef JNI_HARDWARE_USB_H_
#define JNI_HARDWARE_USB_H_

#include "Debug.h"
#include <atomic>
#include <dirent.h>
#include <functional>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <vector>

namespace USB
{
#if 1
	typedef struct
	{
		unsigned char d_type;	 /* file type */
		char d_name[256];		 /* file name */
		off_t st_size;			 /* total size, in bytes */
		blksize_t st_blksize;	 /* blocksize for file system I/O */
		blkcnt_t st_blocks;		 /* number of 512B blocks allocated */
		struct timespec st_atim; /* time of last access */
		struct timespec st_mtim; /* time of last modification */
		struct timespec st_ctim; /* time of last status change */
	} FileInfo;

	std::vector<FileInfo> ListEntriesInDirectory(const std::string& directoryPath);
	bool ReadUsbFileContents(const std::string& filePath, std::string& contents);
	bool ReadFileContents(const std::string& filePath, std::string& contents);
#endif

	// Callback type for USB drive notifications
	using UsbDriveCallback = std::function<void(const std::string&, bool)>;

	class UsbMonitor
	{
	  public:
		static UsbMonitor& getInstance()
		{
			static UsbMonitor instance;
			return instance;
		}

		// Delete copy constructor and assignment operator
		UsbMonitor(const UsbMonitor&) = delete;
		UsbMonitor& operator=(const UsbMonitor&) = delete;

		// Start monitoring USB drives
		void startMonitoring();

		// Stop monitoring USB drives
		void stopMonitoring();

		// Register a callback for USB drive notifications
		void registerCallback(UsbDriveCallback callback, bool initialNotify = false);

		// Get list of currently mounted USB drives
		std::vector<std::string> getMountedDrives() const;

	  private:
		UsbMonitor()
			: running(false)
		{
		}
		~UsbMonitor();

		void monitorThread();
		void notifyCallbacks(const std::string& path, bool connected);
		std::vector<std::string> getUsbMounts();

		std::thread monitor_thread;
		std::atomic<bool> running;
		mutable std::mutex callback_mutex;
		std::vector<UsbDriveCallback> callbacks;
		std::vector<std::string> current_mounts;
	};

} // namespace USB

#endif /* JNI_HARDWARE_USB_H_ */
