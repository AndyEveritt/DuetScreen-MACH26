/*
 * Files.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_OBJECTMODEL_FILES_HPP_
#define JNI_OBJECTMODEL_FILES_HPP_

#include "ObjectModel/Directories.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace OM::FileSystem
{
	enum class FileSystemItemType
	{
		folder,
		file,
	};

	class FileSystemItem
	{
	  public:
		virtual ~FileSystemItem();						// Makes the class polymorphic
		virtual FileSystemItemType GetType() const = 0; // Pure virtual function

		FileSystemItem(const FileSystemItemType type)
			: m_type(type)
			, m_size(0)
		{
		}
		FileSystemItem(const FileSystemItemType type, const std::string& name)
			: m_name(name)
			, m_type(type)
			, m_size(0)
		{
		}

		const std::string& GetName() const { return m_name; }
		void SetName(const std::string name);
		std::string GetPath() const;
		const std::string& GetDate() const { return m_date; }
		void SetDate(const std::string& date) { m_date = date; }
		size_t GetSize() const { return m_size; }
		std::string GetReadableSize() const;
		void SetSize(const size_t size) { m_size = size; }

	  private:
		std::string m_name;
		FileSystemItemType m_type;
		size_t m_size;
		std::string m_date;
	};

	class File : public FileSystemItem
	{
	  public:
		File()
			: FileSystemItem(FileSystemItemType::file)
		{
		}
		File(const std::string& name)
			: FileSystemItem(FileSystemItemType::file, name)
		{
		}
		FileSystemItemType GetType() const override { return FileSystemItemType::file; }
	};

	class Folder : public FileSystemItem
	{
	  public:
		Folder()
			: FileSystemItem(FileSystemItemType::folder)
		{
		}
		Folder(const std::string& name)
			: FileSystemItem(FileSystemItemType::folder, name)
		{
		}
		FileSystemItemType GetType() const override { return FileSystemItemType::folder; }
	};

	using ItemPtr = std::shared_ptr<FileSystemItem>;
	using FilePtr = std::shared_ptr<File>;
	using FolderPtr = std::shared_ptr<Folder>;

	enum class SortBy
	{
		NAME,
		DATE,
		SIZE
	};

	FilePtr AddFileAt(const size_t index);
	FolderPtr AddFolderAt(const size_t index);
	const size_t GetItemCount();
	const std::vector<ItemPtr>& GetItems();
	ItemPtr GetItem(const size_t index);
	FilePtr GetFile(const std::string& name);
	FolderPtr GetSubFolder(const std::string& name);
	void SetCurrentDir(const std::string& path);
	void SortFileSystem(const SortBy by, const bool descending);
	void SortFilesBy(std::vector<ItemPtr>& items, std::function<bool(ItemPtr, ItemPtr)> sortFunc);
	void SortFilesBy(std::vector<ItemPtr>& items, const SortBy sortBy, const bool descending);
	std::string GetParentDirPath();
	std::string GetCurrentDirName();
	std::string& GetCurrentDirPath();
	bool IsInSubFolder();
	void RequestFiles(const OM::Directories::DirectoryType baseFolder,
					  const std::string& path,
					  std::function<void()> callback,
					  bool runEveryTime = false);
	void RunCallback(const size_t next);
	void RequestUsbFiles(const std::string& path);
	bool IsMacroFolder();
	bool IsUsbFolder();
	void RunFile(const File* file);
	void RunMacro(const std::string& path);
	void UploadFile(const File* file);
	void StartPrint(const std::string& path);
	void ResumePrint();
	void PausePrint();
	void StopPrint();
	void PrintAgain();
	void ClearFileSystem();

	std::string GetFileExtension(const std::string& fileName);
} // namespace OM::FileSystem

#endif /* JNI_OBJECTMODEL_FILES_HPP_ */
