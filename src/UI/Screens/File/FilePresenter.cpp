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
		m_view->confirmStartPrint(item->GetName().c_str(), item->GetDate().c_str(), item->GetReadableSize().c_str());
	}

	void FilePresenter::startPrint()
	{
		OM::FileSystem::StartPrint(m_gcodePath);
	}

	void FilePresenter::runMacro()
	{
		OM::FileSystem::RunMacro(m_gcodePath);
	}

	void FilePresenter::requestFiles()
	{
		OM::FileSystem::RequestFiles(
			m_currentFolder.c_str(),
			[this]()
			{
				ModelLock lock;
				this->m_view->setFolder(this->m_currentFolder.c_str());
				this->m_view->setFileCount(OM::FileSystem::GetItemCount());
				for (size_t i = 0; i < this->m_view->getFileCount(); i++)
				{
					auto item = this->m_view->getFileItem(i);
					if (item == nullptr)
					{
						continue;
					}
					auto file = OM::FileSystem::GetItem(i);
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
			},
			true);
	}

	void FilePresenter::refreshFiles()
	{
		FILEINFO_CACHE->ClearCache();
		requestFiles();
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

	void FilePresenter::onActivate()
	{
		setFolder(getBaseFolderPath());
	}
} // namespace UI
