#include "FilePresenter.h"
#include "Comm/Thumbnail.h"
#include "Debug.h"
#include "FileView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	const char* FilePresenter::getBaseFolderPath() const
	{
		switch (m_baseFolder)
		{
		case BaseFolder::GCODES:
			return DEFAULT_GCODES_PATH;
		case BaseFolder::MACROS:
			return DEFAULT_MACROS_PATH;
		default:
			return nullptr;
		}
	}

	void FilePresenter::setFolder(const char* folder)
	{
		m_currentFolder = folder;
		m_view->setFolder(folder);
		requestFiles();
	}

	void FilePresenter::itemClicked(const size_t index)
	{
		ModelLock lock;
		auto item = OM::FileSystem::GetItem(index);

		m_view->cancelStartPrint();
		if (item == nullptr)
		{
			warn("item %u is null", index);
			return;
		}

		if (item->GetType() == OM::FileSystem::FileSystemItemType::folder)
		{
			setFolder(item->GetPath().c_str());
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
		for (size_t i = 0; i < m_view->getFileCount(); i++)
		{
			if (i > m_items.size())
			{
				warn("File count mismatch");
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
			m_currentFolder.c_str(),
			[this]()
			{
				ModelLock lock;
				m_items = OM::FileSystem::GetItems();
				this->m_view->setFolder(this->m_currentFolder.c_str());
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
		auto first = m_items.begin();
		auto last = m_items.end();
		if (first != last)
		{			// Ensure the range is not empty
			--last; // Point to the last valid item
			while (std::distance(first, last) > 0)
			{
				auto temp = last;
				while (temp != first)
				{
					auto prev = std::prev(temp);
					if (
						[this](std::shared_ptr<OM::FileSystem::FileSystemItem> L,
							   std::shared_ptr<OM::FileSystem::FileSystemItem> R) -> bool
						{
							if (L->GetType() == R->GetType())
							{
								switch (m_sortBy)
								{
								case SortBy::NAME:
									return L->GetName() > R->GetName();
								case SortBy::DATE:
									return L->GetDate() > R->GetDate();
								case SortBy::SIZE:
									return L->GetSize() > R->GetSize();
								default:
									return false;
								}
							}
							return L->GetType() < R->GetType();
						}(*temp, *prev))
					{
						std::iter_swap(temp, prev);
					}
					--temp;
				}
				++first;
			}
		}
	}

	void FilePresenter::setSortOrder(SortBy by, bool forward)
	{
		m_sortBy = by;
		m_sortOrder = forward;
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

		size_t pos = m_currentFolder.find_last_of('/');
		if (pos != std::string::npos)
		{
			setFolder(m_currentFolder.substr(0, pos).c_str());
			return true;
		}
		return false;
	}

	void FilePresenter::newThumbnailData(const char* filename)
	{
		ModelLock lock;
		for (size_t i = 0; i < this->m_view->getFileCount(); i++)
		{
			auto file = OM::FileSystem::GetItem(i);
			if (file == nullptr)
			{
				continue;
			}
			if (file->GetPath() == filename)
			{
				auto item = this->m_view->getFileItem(i);
				if (item != nullptr)
				{
					item->setThumbnail(GetThumbnailPath(filename).c_str());
				}
				break;
			}
		}
	}

	void FilePresenter::onActivate()
	{
		setFolder(getBaseFolderPath());
	}
} // namespace UI
