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
		void SetPath(const std::string& path) { m_path = path; }
		const std::string& GetDate() const { return m_date; }
		void SetDate(const std::string& date) { m_date = date; }
		size_t GetSize() const { return m_size; }
		std::string GetReadableSize() const;
		void SetSize(const size_t size) { m_size = size; }

	  private:
		std::string m_path;
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

	using request_files_cb_t = std::function<void()>;
	using request_file_contents_cb_t = std::function<void(std::string_view contents)>;

	class FileContents
	{
	  public:
		FileContents(std::string_view filename,
					 request_file_contents_cb_t callback = nullptr,
					 bool runEveryTime = false);

		std::string_view GetFilename() const { return m_filename; }
		int AppendData(std::string_view data);
		int GetData(std::string& outData) const;
		int ClearData();

		void SetOffset(uint32_t offset) { m_offset = offset; }
		uint32_t GetOffset() const { return m_offset; }

		void SetNext(uint32_t next) { m_next = next; }
		uint32_t GetNext() const { return m_next; }

		void SetParseError(int16_t parseErr) { m_parseErr = parseErr; }
		int16_t GetParseError() const { return m_parseErr; }

		void RunCallback();

	  private:
		std::string m_filename; // Path to the file
		request_file_contents_cb_t m_callback;
		bool m_runEveryTime = false;
		uint32_t m_offset = 0;
		uint32_t m_next = 0;
		int16_t m_parseErr = 0;
	};

	using FileContentsPtr = std::shared_ptr<FileContents>;

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
					  request_files_cb_t callback,
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

	void RequestFileContents(const OM::Directories::DirectoryType baseFolder,
							 std::string_view path,
							 request_file_contents_cb_t callback,
							 bool runEveryTime = false);
	FileContentsPtr GetCurrentFileRequestContents();

	std::string_view GetFileExtension(std::string_view fileName);
} // namespace OM::FileSystem

#endif /* JNI_OBJECTMODEL_FILES_HPP_ */
