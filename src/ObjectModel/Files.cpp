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
#include <fstream>

namespace OM::FileSystem
{
	static std::string s_currentDirPath;
	static std::vector<ItemPtr> s_items;
	static struct
	{
		request_files_cb_t cb;
		bool runEveryTime;
	} s_callback;
	static bool s_inMacroFolder = false;
	static bool s_usbFolder = false;

	static FileContentsPtr s_fileContents;

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

	static std::string GetLocalFilePath(std::string_view filename)
	{
		filename = filename.substr(filename.find_last_of('/') + 1); // Get the file name only
		return fmt::format("/tmp/files/{}", filename);
	}

	int FileContents::AppendData(std::string_view data)
	{
		if (data.empty())
		{
			return 0;
		}

		const std::string fullPath = GetLocalFilePath(m_filename);
		std::ofstream file(fullPath, std::ios::app | std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open file {} for appending data", fullPath);
			return -1;
		}

		if (!file.write(data.data(), data.size()))
		{
			LOG_ERROR("Failed to write data to file {}", fullPath);
			return -2;
		}

		return 0;
	}

	int FileContents::GetData(std::string& outData) const
	{
		const std::string fullPath = GetLocalFilePath(m_filename);
		std::ifstream file(fullPath, std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open file {} for reading", fullPath);
			return -1;
		}

		outData.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (outData.empty() && !file.eof())
		{
			LOG_ERROR("Failed to read data from file {}", fullPath);
			return -2;
		}
		return 0;
	}

	int FileContents::ClearData()
	{
		const std::string fullPath = GetLocalFilePath(m_filename);
		std::ofstream file(fullPath, std::ios::trunc | std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Failed to open file {} for clearing data", fullPath);
			return -1;
		}
		return 0;
	}

	void FileContents::RunCallback()
	{
		if (m_parseErr != 0)
		{
			LOG_ERROR("File {} has parse error: {}", m_filename, m_parseErr);
			return;
		}

		if (m_next != 0 && !m_runEveryTime)
		{
			LOG_DBG("File {} has next offset: {}, not running callback", m_filename, m_next);
			return;
		}

		if (m_callback)
		{
			std::string contents;
			if (GetData(contents) == 0)
			{
				m_callback(contents);
			}
			else
			{
				LOG_ERROR("Failed to get data for callback from file {}", m_filename);
			}
		}
	}

	FilePtr AddFileAt(const size_t index)
	{
		if (index < s_items.size())
		{
			LOG_DBG("Deleting item[{:d}]", index);
			s_items[index].reset();
		}
		FilePtr file = std::make_shared<File>();
		s_items.insert(s_items.begin() + index, file);
		return file;
	}

	FolderPtr AddFolderAt(const size_t index)
	{
		if (index < s_items.size())
		{
			LOG_DBG("Deleting item[{:d}]", index);
			s_items[index].reset();
		}
		FolderPtr folder = std::make_shared<Folder>();
		s_items.insert(s_items.begin() + index, folder);
		return folder;
	}

	const size_t GetItemCount()
	{
		return s_items.size();
	}

	const std::vector<ItemPtr>& GetItems()
	{
		return s_items;
	}

	ItemPtr GetItem(const size_t index)
	{
		if (index >= GetItemCount())
			return nullptr;
		return s_items[index];
	}

	FilePtr GetFile(const std::string& name)
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

	FolderPtr GetSubFolder(const std::string& name)
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

	void SortFileSystem(const SortBy by, const bool descending)
	{
		if (s_items.empty())
			return;
		SortFilesBy(s_items, by, descending);
	}

	void SortFilesBy(std::vector<ItemPtr>& items, std::function<bool(ItemPtr, ItemPtr)> sortFunc)
	{
		auto first = items.begin();
		auto last = items.end();
		if (first != last)
		{			// Ensure the range is not empty
			--last; // Point to the last valid item
			while (std::distance(first, last) > 0)
			{
				auto temp = last;
				while (temp != first)
				{
					auto prev = std::prev(temp);
					if (*prev == nullptr)
						return;
					if (sortFunc(*temp, *prev))
					{
						std::iter_swap(temp, prev);
					}
					--temp;
				}
				++first;
			}
		}
	}

	void SortFilesBy(std::vector<ItemPtr>& items, const SortBy by, const bool descending)
	{
		switch (by)
		{
		case SortBy::NAME:
			SortFilesBy(items,
						[descending](ItemPtr L, ItemPtr R)
						{
							if (L->GetType() == R->GetType())
								return descending == L->GetName() > R->GetName();
							return L->GetType() < R->GetType();
						});
			break;
		case SortBy::DATE:
			SortFilesBy(items,
						[descending](ItemPtr L, ItemPtr R)
						{
							if (L->GetType() == R->GetType())
								return descending == L->GetDate() > R->GetDate();
							return L->GetType() < R->GetType();
						});
			break;
		case SortBy::SIZE:
			SortFilesBy(items,
						[descending](ItemPtr L, ItemPtr R)
						{
							if (L->GetType() == R->GetType())
								return descending == L->GetSize() > R->GetSize();
							return L->GetType() < R->GetType();
						});
			break;
		default:
			break;
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

	void RequestFiles(OM::Directories::DirectoryType baseFolder,
					  const std::string& path,
					  request_files_cb_t callback,
					  bool runEveryTime)
	{
		s_usbFolder = false;
		s_inMacroFolder = baseFolder == OM::Directories::DirectoryType::MACROS;
		s_callback.cb = callback;
		s_callback.runEveryTime = runEveryTime;
		LOG_INFO("Files: requesting files in {:s}", path.c_str());
		Comm::DUET.RequestFileList(fmt::format("{}{}", OM::Directories::GetDirectory(baseFolder), path));
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
		Comm::DUET.SendGcodef("M23 \"%s\"\nM24\n", OM::GetLastJobName().c_str());
	}

	void ClearFileSystem()
	{
		LOG_INFO("Files: clearing items");
		s_items.clear();
	}

	void RequestFileContents(const OM::Directories::DirectoryType baseFolder,
							 std::string_view path,
							 request_file_contents_cb_t callback,
							 bool runEveryTime)
	{
		LOG_INFO("Requesting file contents of {}", path);
		std::string fullPath = fmt::format("{}{}", OM::Directories::GetDirectory(baseFolder), path);
		if (Comm::DUET.GetCommunicationType() == Comm::CommunicationType::network)
		{
			std::string contents;
			if (!Comm::DUET.DownloadFile(fullPath, contents))
			{
				LOG_ERROR("Failed to download file contents of {}", fullPath);
				return;
			}
			callback(contents);
		}
		else
		{
			s_fileContents = std::make_shared<FileContents>(fullPath, callback, runEveryTime);
			Comm::DUET.SendGcodef("M36.2 P\"%s\" S0\n", fullPath.c_str());
		}
	}

	FileContentsPtr GetCurrentFileRequestContents()
	{
		return s_fileContents;
	}

	std::string_view GetFileExtension(std::string_view filename)
	{
		size_t dot = filename.find_last_of('.');
		if (dot != std::string::npos)
		{
			return filename.substr(dot + 1);
		}
		return "";
	}
} // namespace OM::FileSystem
