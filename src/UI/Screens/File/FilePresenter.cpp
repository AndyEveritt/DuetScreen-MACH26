#include "FilePresenter.h"
#include "Comm/Thumbnail.h"
#include "Debug.h"
#include "FileView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Directories.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "UI/Screens/Home/HomeView.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"

namespace UI
{
	static std::string s_emptyStr = "";

	static OM::Directories::DirectoryType getBaseFolderType(FilePresenter::BaseFolder folder)
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

	std::string_view FilePresenter::getBaseFolderPath() const
	{
		std::string_view baseFolder;
		switch (m_baseFolder)
		{
		case BaseFolder::GCODES:
			baseFolder = OM::Directories::GetGcodesDirectory();
			break;
		case BaseFolder::MACROS:
			baseFolder = OM::Directories::GetMacrosDirectory();
			break;
		default:
			baseFolder = "";
		}

		if (baseFolder.back() == '/')
		{
			return baseFolder.substr(0, baseFolder.size() - 1);
		}
		return baseFolder;
	}

	void FilePresenter::setFolder(std::string_view folder)
	{
		if (!folder.empty() && folder.starts_with(getBaseFolderPath()))
		{
			m_currentFolder = folder.substr(getBaseFolderPath().length());
		}
		else
		{
			m_currentFolder = folder;
		}
		if (!m_currentFolder.empty() && m_currentFolder.back() != '/')
		{
			m_currentFolder += '/';
		}

		LOG_DBG("set folder to '{:s}'", m_currentFolder);
		m_view->setFolder(fmt::format("{}{}", getBaseFolderPath(), m_currentFolder));
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
		std::string date = item->GetDate();
		std::replace(date.begin(), date.end(), 'T', ' ');
		m_view->confirmStartPrint(
			item->GetName(), date, item->GetReadableSize(), GetThumbnailPath(item->GetPath().c_str()));
	}

	void FilePresenter::startPrint()
	{
		OM::FileSystem::StartPrint(m_gcodePath);
		HomeView::instance().getDashboard().disableJobsTab(true);
		home();
	}

	void FilePresenter::runMacro()
	{
		OM::FileSystem::RunMacro(m_gcodePath);
	}

	void FilePresenter::displayFiles()
	{
		m_view->setFileCount(m_items.size());
		m_view->showSort(m_sortBy, m_sortOrder);

		auto item_count = m_items.size();
		for (size_t i = 0; i < item_count; i++)
		{
			auto item = m_view->getFileItem(i);
			if (!item)
			{
				continue;
			}
			auto file = m_items.at(i);
			if (file == nullptr)
			{
				continue;
			}

			item->setFileLabel(file->GetName().c_str());
			std::string date = file->GetDate();
			std::replace(date.begin(), date.end(), 'T', ' ');
			item->setFileDate(date.c_str());
#if SHOW_FILE_ITEM_SIZE
			item->setFileSize(file->GetReadableSize().c_str());
#endif
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
		LOG_DBG("Requesting files for folder {:s}{:s}", getBaseFolderPath(), m_currentFolder);
		UI_LOCK();
		OM::FileSystem::RequestFiles(
			getBaseFolderType(m_baseFolder),
			m_currentFolder,
			[this](OM::FileSystem::ItemList files)
			{
				LOG_DBG("Received {:d} files for folder {:s}{:s}",
						files.size(),
						getBaseFolderPath(),
						this->m_currentFolder);
				UI_LOCK();
				m_items = std::move(files);
				this->m_view->setFolder(fmt::format("{}{}", getBaseFolderPath(), this->m_currentFolder));
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
		UI_LOCK();
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
		UI_LOCK();
		for (size_t i = 0; i < this->m_view->getFileCount(); i++)
		{
			if (i >= m_items.size())
			{
				LOG_WARN("File count mismatch");
				break;
			}
			auto file = m_items.at(i);
			if (file == nullptr)
			{
				continue;
			}
			if (file->GetPath() == filename)
			{
				auto item = this->m_view->getFileItem(i);
				if (item)
				{
					item->setThumbnail(GetThumbnailPath(filename.c_str()).c_str());
				}
				break;
			}
		}
	}

	void FilePresenter::onInit()
	{
		registerEventListener<EventType::ThumbnailData>(this, &FilePresenter::newThumbnailData);
		setSort(StorageHelper::getData(ID_FILE_SORT_BY, SortBy::DATE),
				StorageHelper::getData(ID_FILE_SORT_DESCENDING, true));
	}

	void FilePresenter::onActivate()
	{
		// setFolder("");
		requestFiles();
	}

	void FilePresenter::onConnect()
	{
		setFolder("");
	}

	void FilePresenter::onDisconnect()
	{
		LOG_DBG("Clearing files");
		UI_LOCK();
		m_items.clear();
		m_view->setFileCount(0);
	}
} // namespace UI
