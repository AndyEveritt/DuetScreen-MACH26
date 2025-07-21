#include "FilePresenter.h"
#include "Comm/Thumbnail.h"
#include "Debug.h"
#include "FileView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Directories.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	static std::string s_emptyStr = "";

	static const OM::Directories::DirectoryType getBaseFolderType(FilePresenter::BaseFolder folder)
	{
		switch (folder)
		{
		case FilePresenter::BaseFolder::GCODES:
			return OM::Directories::DirectoryType::GCODES;
		case FilePresenter::BaseFolder::MACROS:
			return OM::Directories::DirectoryType::MACROS;
		default:
			LOG_ERROR("Invalid base folder type");
			return OM::Directories::DirectoryType::GCODES;
		}
	}

	const std::string& FilePresenter::getBaseFolderPath() const
	{
		switch (m_baseFolder)
		{
		case BaseFolder::GCODES:
			return OM::Directories::GetGcodesDirectory();
		case BaseFolder::MACROS:
			return OM::Directories::GetMacrosDirectory();
		default:
			return s_emptyStr;
		}
	}

	void FilePresenter::setFolder(const std::string& folder)
	{
		if (folder.starts_with(getBaseFolderPath()))
		{
			m_currentFolder = folder.substr(getBaseFolderPath().length() + 1);
		}
		else
		{
			m_currentFolder = folder;
		}
		if (!m_currentFolder.empty() && m_currentFolder.back() != '/')
		{
			m_currentFolder += '/';
		}

		LOG_DBG("set folder to {:s}", m_currentFolder);
		m_view->setFolder(getBaseFolderPath() + m_currentFolder);
		requestFiles();
	}

	void FilePresenter::itemClicked(const size_t index)
	{
		MODEL_LOCK();
		if (index >= m_items.size())
		{
			LOG_ERROR("item {:d} out of range", index);
			return;
		}

		auto item = m_items[index];

		m_view->cancelStartPrint();
		if (item == nullptr)
		{
			LOG_WARN("item {:d} is null", index);
			return;
		}

		if (item->GetType() == OM::FileSystem::FileSystemItemType::folder)
		{
			setFolder(item->GetPath());
			return;
		}

		// File
		m_gcodePath = item->GetPath();
		if (m_gcodePath.starts_with(DEFAULT_MACROS_PATH))
		{
			m_view->confirmRunMacro(item->GetName().c_str());
			return;
		}

		Comm::FileInfoPtr fileInfo = FILEINFO_CACHE->GetFileInfo(item->GetPath());
		FILEINFO_CACHE->QueueLargeThumbnailRequest(item->GetPath());
		m_view->confirmStartPrint(item->GetName().c_str(),
								  item->GetDate().c_str(),
								  item->GetReadableSize().c_str(),
								  GetThumbnailPath(item->GetPath().c_str()).c_str());
	}

	void FilePresenter::startPrint()
	{
		OM::FileSystem::StartPrint(m_gcodePath);
	}

	void FilePresenter::runMacro()
	{
		OM::FileSystem::RunMacro(m_gcodePath);
	}

	void FilePresenter::displayFiles()
	{
		m_view->setFileCount(m_items.size());
		m_view->showSort(m_sortBy, m_sortOrder);
		for (size_t i = 0; i < m_view->getFileCount(); i++)
		{
			if (i >= m_items.size())
			{
				LOG_WARN("File count mismatch");
				break;
			}
			auto item = m_view->getFileItem(i);
			if (item == nullptr)
			{
				continue;
			}
			auto file = m_items[i];
			if (file == nullptr)
			{
				continue;
			}
			item->setLabel(file->GetName().c_str());
			item->setDate(file->GetDate().c_str());
			item->setSize(file->GetReadableSize().c_str());
			item->setType(file->GetType() == OM::FileSystem::FileSystemItemType::folder);

			// Set thumbnail
			if (file->GetType() == OM::FileSystem::FileSystemItemType::file &&
				IsThumbnailCached(file->GetPath().c_str()))
			{
				item->setThumbnail(GetThumbnailPath(file->GetPath().c_str()).c_str());
			}
			else
			{
				item->setThumbnail(nullptr);
			}
		}
	}

	void FilePresenter::requestFiles()
	{
		m_items.clear();
		m_view->setFileCount(0);
		OM::FileSystem::RequestFiles(
			getBaseFolderType(m_baseFolder),
			m_currentFolder,
			[this]()
			{
				{
					MODEL_LOCK();
					m_items = OM::FileSystem::GetItems();
				}
				this->m_view->setFolder(getBaseFolderPath() + this->m_currentFolder);
				this->sortFiles();
				this->displayFiles();
			},
			true);
	}

	void FilePresenter::refreshFiles()
	{
		FILEINFO_CACHE->ClearCache();
		requestFiles();
	}

	void FilePresenter::sortFiles()
	{
		OM::FileSystem::SortFilesBy(m_items, m_sortBy, m_sortOrder);
	}

	void FilePresenter::setSort(SortBy by, bool descending)
	{
		m_sortBy = by;
		m_sortOrder = descending;
		StorageHelper::setData(ID_FILE_SORT_BY, by);
		StorageHelper::setData(ID_FILE_SORT_DESCENDING, descending);
		sortFiles();
		displayFiles();
	}

	bool FilePresenter::back()
	{
		if (m_currentFolder == DEFAULT_GCODES_PATH)
		{
			return false; // Already at the root folder
		}
		if (m_currentFolder == DEFAULT_MACROS_PATH)
		{
			return false; // Already at the root folder
		}

		if (m_currentFolder.empty())
		{
			return false;
		}

		size_t pos = m_currentFolder.substr(0, m_currentFolder.size() - 1).find_last_of('/');
		if (pos != std::string::npos)
		{
			setFolder(m_currentFolder.substr(0, pos));
			return true;
		}
		else
		{
			setFolder("");
			return true;
		}
		return false;
	}

	void FilePresenter::newThumbnailData(const std::string& filename)
	{
		for (size_t i = 0; i < this->m_view->getFileCount(); i++)
		{
			if (i >= m_items.size())
			{
				LOG_WARN("File count mismatch");
				break;
			}
			auto file = m_items[i];
			if (file == nullptr)
			{
				continue;
			}
			if (file->GetPath() == filename)
			{
				auto item = this->m_view->getFileItem(i);
				if (item != nullptr)
				{
					item->setThumbnail(GetThumbnailPath(filename.c_str()).c_str());
				}
				break;
			}
		}
	}

	void FilePresenter::onActivate()
	{
		setSort(StorageHelper::getData(ID_FILE_SORT_BY, SortBy::DATE),
				StorageHelper::getData(ID_FILE_SORT_DESCENDING, true));
		setFolder("");
	}

	void FilePresenter::onConnect()
	{
		requestFiles();
	}

	void FilePresenter::onDisconnect()
	{
		m_items.clear();
		m_view->setFileCount(0);
	}
} // namespace UI
