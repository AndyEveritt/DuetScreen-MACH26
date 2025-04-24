/*
 * Files.cpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "Files.h"

#include "Comm/Communication.h"
#include "Hardware/Duet.h"
#include "Hardware/Usb.h"
#include "ObjectModel/Job.h"
#include <algorithm>

namespace OM::FileSystem
{
	static std::string s_currentDirPath;
	static std::vector<std::shared_ptr<FileSystemItem>> s_items;
	static struct
	{
		std::function<void()> cb;
		bool runEveryTime;
	} s_callback;
	static bool s_inMacroFolder = false;
	static bool s_usbFolder = false;

	std::string FileSystemItem::GetPath() const
	{
		if (s_currentDirPath.empty())
			return m_name;
		return s_currentDirPath + "/" + m_name;
	}

	std::string FileSystemItem::GetReadableSize() const
	{
		const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
		int order = 0;

		if (m_size == 0)
			return "0 B";

		double len = double(m_size);
		while (len >= 1024 && order < 4)
		{
			order++;
			len /= 1024;
		}

		return utils::format("%.2f %s", len, sizes[order]);
	}

	void FileSystemItem::SetName(const std::string name)
	{
		m_name = name.c_str();
		switch (m_type)
		{
		case FileSystemItemType::file:
			LOG_VERBOSE("Files: set file name to {:s}", m_name.c_str());
			break;
		case FileSystemItemType::folder:
			LOG_VERBOSE("Files: set folder name to {:s}", m_name.c_str());
			break;
		}
	}

	FileSystemItem::~FileSystemItem()
	{
		LOG_DBG("Files: destructing item {:s}", GetPath().c_str());
	}

	std::shared_ptr<File> AddFileAt(const size_t index)
	{
		if (index < s_items.size())
		{
			LOG_DBG("Deleting item[{:d}]", index);
			s_items[index].reset();
		}
		std::shared_ptr<File> file = std::make_shared<File>();
		s_items.insert(s_items.begin() + index, file);
		return file;
	}

	std::shared_ptr<Folder> AddFolderAt(const size_t index)
	{
		if (index < s_items.size())
		{
			LOG_DBG("Deleting item[{:d}]", index);
			s_items[index].reset();
		}
		std::shared_ptr<Folder> folder = std::make_shared<Folder>();
		s_items.insert(s_items.begin() + index, folder);
		return folder;
	}

	const size_t GetItemCount()
	{
		return s_items.size();
	}

	const std::vector<std::shared_ptr<FileSystemItem>>& GetItems()
	{
		return s_items;
	}

	std::shared_ptr<FileSystemItem> GetItem(const size_t index)
	{
		if (index >= GetItemCount())
			return nullptr;
		return s_items[index];
	}

	std::shared_ptr<File> GetFile(const std::string& name)
	{
		for (const auto& item : s_items)
		{
			if (!item)
				continue;
			if (item->GetName() != name)
				continue;
			if (item->GetType() != FileSystemItemType::file)
				continue;
			return std::static_pointer_cast<File>(item);
		}
		return nullptr;
	}

	std::shared_ptr<Folder> GetSubFolder(const std::string& name)
	{
		for (const auto& item : s_items)
		{
			if (!item)
				continue;
			if (item->GetName() != name)
				continue;
			if (item->GetType() != FileSystemItemType::folder)
				continue;
			return std::static_pointer_cast<Folder>(item);
		}
		return nullptr;
	}

	void SetCurrentDir(const std::string& path)
	{
		s_currentDirPath = path;
		LOG_INFO("Files: current directory = {:s}", s_currentDirPath.c_str());
	}

	struct
	{
		bool operator()(std::shared_ptr<FileSystemItem> L, std::shared_ptr<FileSystemItem> R)
		{
			if (L->GetType() == R->GetType())
				return L->GetDate() > R->GetDate();
			return L->GetType() < R->GetType();
		}
	} SortItem;

	void SortFileSystem()
	{
		auto first = s_items.begin();
		auto last = s_items.end();
		if (first != last)
		{			// Ensure the range is not empty
			--last; // Point to the last valid item
			while (std::distance(first, last) > 0)
			{
				auto temp = last;
				while (temp != first)
				{
					auto prev = std::prev(temp);
					if (SortItem(*temp, *prev))
					{
						std::iter_swap(temp, prev);
					}
					--temp;
				}
				++first;
			}
		}
	}

	std::string GetParentDirPath()
	{
		if (!IsInSubFolder())
			return s_currentDirPath;

		std::string path;
		size_t i = s_currentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return "";
		return s_currentDirPath.substr(0, i);
	}

	std::string GetCurrentDirName()
	{
		std::string path;
		size_t i = s_currentDirPath.find_last_of('/');

		if (i == std::string::npos)
			return s_currentDirPath;
		return s_currentDirPath.substr(i, s_currentDirPath.size() - i);
	}

	std::string& GetCurrentDirPath()
	{
		return s_currentDirPath;
	}

	bool IsInSubFolder()
	{
		if (IsUsbFolder())
		{
			return s_currentDirPath.empty() ? false : true;
		}

		size_t count = 0;
		for (auto c : s_currentDirPath)
		{
			if (c == '/')
				count++;
		}
		LOG_DBG("Files: {:d}", count);

		return count > 1;
	}

	void RequestFiles(const std::string& path, std::function<void()> callback, bool runEveryTime)
	{
		s_usbFolder = false;
		s_inMacroFolder = path.find("macro") != std::string::npos;
		s_callback.cb = callback;
		s_callback.runEveryTime = runEveryTime;
		LOG_INFO("Files: requesting files in {:s}", path.c_str());
		Comm::DUET.RequestFileList(path.c_str());
	}

	void RunCallback(const size_t next)
	{
		if (next > 0 && !s_callback.runEveryTime)
		{
			return;
		}
		if (s_callback.cb)
		{
			s_callback.cb();
			if (next == 0)
			{
				s_callback.cb = nullptr;
			}
		}
	}

	void RequestUsbFiles(const std::string& path)
	{
#if 0
		ClearFileSystem();
		s_usbFolder = true;
		s_currentDirPath = path;
		std::vector<USB::FileInfo> files = USB::ListEntriesInDirectory(std::string("/mnt/usb1/") + path);
		size_t index = 0;
		for (auto& fileInfo : files)
		{
			if (fileInfo.d_type == DT_DIR)
			{
				Folder* folder = AddFolderAt(index);
				folder->SetName(fileInfo.d_name);
			}
			else if (fileInfo.d_type == DT_REG)
			{
				File* file = AddFileAt(index);
				file->SetName(fileInfo.d_name);
				file->SetSize(fileInfo.st_size);
				file->SetDate(utils::format("%d", fileInfo.st_mtim));
			}
			index++;
		}
#endif
	}

	bool IsMacroFolder()
	{
		return s_inMacroFolder;
	}

	bool IsUsbFolder()
	{
		return s_usbFolder;
	}

	void RunFile(const File* file)
	{
		if (s_inMacroFolder)
			RunMacro(file->GetPath());
		else
			StartPrint(file->GetPath());
	}

	void RunMacro(const std::string& path)
	{
		Comm::DUET.SendGcodef("M98 P\"%s\"\n", path.c_str());
	}

	void UploadFile(const File* file)
	{
		// TODO upload file
#if 0
		std::string contents;
		if (!USB::ReadUsbFileContents(file->GetPath(), contents))
			return;
		Comm::DUET.UploadFile(utils::format("/gcodes/%s", file->GetName().c_str()).c_str(), contents);
#endif
	}

	void StartPrint(const std::string& path)
	{
		Comm::DUET.SendGcodef("M32 \"%s\"\n", path.c_str());
	}

	void ResumePrint()
	{
		Comm::DUET.SendGcode("M24\n");
	}

	void PausePrint()
	{
		Comm::DUET.SendGcode("M25\n");
	}

	void StopPrint()
	{
		Comm::DUET.SendGcode("M0\n");
	}

	void PrintAgain()
	{
		Comm::DUET.SendGcodef("M23 \"%s\"\nM24", OM::GetLastJobName().c_str());
	}

	void ClearFileSystem()
	{
		LOG_INFO("Files: clearing items");
		s_items.clear();
	}

	std::string GetFileExtension(const std::string& filename)
	{
		size_t dot = filename.find_last_of('.');
		if (dot != std::string::npos)
		{
			return filename.substr(dot + 1);
		}
		return "";
	}
} // namespace OM::FileSystem
