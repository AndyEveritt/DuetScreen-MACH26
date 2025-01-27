#include "FilePresenter.h"
#include "Debug.h"
#include "FileView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void FilePresenter::setFolder(const char* folder)
	{
		m_currentFolder = folder;
	}

	void FilePresenter::folderClicked(const size_t index)
	{
		ModelLock lock;
		auto file = OM::FileSystem::GetItem(index);
		if (file != nullptr && file->GetType() == OM::FileSystem::FileSystemItemType::folder)
		{
			setFolder(file->GetPath().c_str());
			requestFiles();
		}
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
					OM::FileSystem::FileSystemItem* file = OM::FileSystem::GetItem(i);
					if (file == nullptr)
					{
						continue;
					}
					item->setLabel(file->GetName().c_str());
					item->setDate(file->GetDate().c_str());
					item->setSize(file->GetReadableSize().c_str());
					item->setType(file->GetType() == OM::FileSystem::FileSystemItemType::folder);
				}
			},
			true);
	}

	bool FilePresenter::back()
	{
		if (m_currentFolder == "0:/gcodes")
		{
			return false; // Already at the root folder
		}

		size_t pos = m_currentFolder.find_last_of('/');
		if (pos != std::string::npos)
		{
			m_currentFolder = m_currentFolder.substr(0, pos);
			requestFiles();
			return true;
		}
		return false;
	}

	void FilePresenter::onActivate()
	{
		m_currentFolder = "0:/gcodes";
		requestFiles();
	}
} // namespace UI
