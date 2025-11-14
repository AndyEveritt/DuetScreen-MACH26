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
#include "i18n/i18n.h"

namespace UI
{
	void FilamentSelectPresenter::setSelectedToolBySlot(size_t slot)
	{
		if (slot >= m_tools.size())
		{
			LOG_ERROR("Invalid tool slot: {}", slot);
			return;
		}
		m_selectedTool = m_tools[slot];
		// getView()->showToolSelect(m_tools.size() > 1);
		// getView()->setSelectedFilament(m_selectedTool->GetFilament().c_str());
		getView()->showSelection(m_selectedTool->GetName(), m_selectedTool->GetFilament().c_str());
	}

	void FilamentSelectPresenter::setFilament(std::string_view filamentName)
	{
		if (m_selectedTool == nullptr)
		{
			LOG_ERROR("No tool selected, cannot set filament");
			return;
		}
		m_selectedTool->ChangeFilament(filamentName.data());
	}

	void FilamentSelectPresenter::unloadFilament()
	{
		if (m_selectedTool == nullptr)
		{
			LOG_ERROR("No tool selected, cannot unload filament");
			return;
		}

		m_selectedTool->UnloadFilament();
	}

	void FilamentSelectPresenter::clear()
	{
		m_selectedTool.reset();
		getView()->setToolCount(0);
		m_filamentOptions.clear();
		updateFilamentList();
	}

	void FilamentSelectPresenter::newToolData()
	{
		MODEL_LOCK();

		// This resizing mess is to prevent unnecessary reallocations since the capacity of m_tools will stay at the
		// largest resize so resizing to OM::GetToolCount() should not cause any reallocations unless the actual tool
		// count in the OM increases.
		m_tools.resize(OM::GetToolCount());
		size_t toolCount = 0;
		OM::IterateToolsWhile(
			[this, &toolCount](OM::ToolPtr tool, size_t /* index */)
			{
				if (tool->filamentExtruder >= 0)
				{
					m_tools[toolCount] = tool;
					toolCount++;
				}
				return true;
			});
		m_tools.resize(toolCount);

		if (m_tools.empty())
		{
			m_selectedTool.reset();
		}
		else if (m_tools.size() == 1)
		{
			m_selectedTool = m_tools[0];
		}
		// getView()->showToolSelect(m_tools.size() > 1);

		getView()->setToolCount(m_tools.size());
		for (size_t i = 0; i < m_tools.size(); i++)
		{
			auto& tool = m_tools[i];
			getView()->setToolData(i, tool->GetName(), tool->GetFilament().c_str());
		}

		if (m_selectedTool)
		{
			getView()->setSelectedFilament(m_selectedTool->GetFilament().c_str());
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
									 [this](OM::FileSystem::ItemList files)
									 {
										 {
											 MODEL_LOCK();
											 this->m_filamentOptions.clear();
											 this->m_filamentOptions.reserve(files.size());
											 for (const auto& item : files)
											 {
												 if (!item)
												 {
													 continue;
												 }
												 this->m_filamentOptions.emplace_back(item->GetName());
											 }
										 }
										 this->updateFilamentList();
									 });
	}
} // namespace UI
