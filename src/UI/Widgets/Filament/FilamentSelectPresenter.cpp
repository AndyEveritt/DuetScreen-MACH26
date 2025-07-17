/*
 * FilamentSelectPresenter.cpp
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#include "FilamentSelectPresenter.h"
#include "Debug.h"
#include "FilamentSelect.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void FilamentSelectPresenter::clear()
	{
		getView()->setToolCount(0);
		m_filamentOptions.clear();
		updateFilamentList();
	}

	void FilamentSelectPresenter::newToolData()
	{
		MODEL_LOCK();
		std::vector<OM::ToolPtr> tools;
		OM::IterateToolsWhile(
			[&tools](OM::ToolPtr tool, size_t index)
			{
				if (tool->filamentExtruder >= 0)
				{
					tools.push_back(tool);
				}
				return true;
			});

		getView()->setToolCount(tools.size());
		for (size_t i = 0; i < tools.size(); i++)
		{
			auto& tool = tools[i];
			getView()->setToolData(i,
								   tool->name.IsEmpty() ? fmt::format("{} {}", _("default_tool_name"), i)
														: tool->name.c_str(),
								   tool->GetFilament().c_str());
		}
	}

	void FilamentSelectPresenter::updateFilamentList()
	{
		getView()->setFilamentOptions(m_filamentOptions);
	}

	void FilamentSelectPresenter::onActivate()
	{
		OM::FileSystem::RequestFiles(OM::Directories::DirectoryType::FILAMENTS,
									 "",
									 [this]()
									 {
										 {
											 MODEL_LOCK();
											 this->m_filamentOptions.clear();
											 for (size_t i = 0; i < OM::FileSystem::GetItemCount(); i++)
											 {
												 std::shared_ptr<OM::FileSystem::FileSystemItem> item =
													 OM::FileSystem::GetItem(i);
												 if (item == nullptr)
												 {
													 continue;
												 }
												 this->m_filamentOptions.push_back(item->GetName());
											 }
										 }
										 this->updateFilamentList();
									 });
	}
} // namespace UI
