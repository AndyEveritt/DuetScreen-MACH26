/*
 * Files.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_OBJECTMODEL_FILES_HPP_
#define JNI_OBJECTMODEL_FILES_HPP_

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

	std::shared_ptr<File> AddFileAt(const size_t index);
	std::shared_ptr<Folder> AddFolderAt(const size_t index);
	const size_t GetItemCount();
	const std::vector<std::shared_ptr<FileSystemItem>>& GetItems();
	std::shared_ptr<FileSystemItem> GetItem(const size_t index);
	std::shared_ptr<File> GetFile(const std::string& name);
	std::shared_ptr<Folder> GetSubFolder(const std::string& name);
	void SetCurrentDir(const std::string& path);
	void SortFileSystem();
	std::string GetParentDirPath();
	std::string GetCurrentDirName();
	std::string& GetCurrentDirPath();
	bool IsInSubFolder();
	void RequestFiles(const std::string& path, std::function<void()> callback, bool runEveryTime = false);
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
