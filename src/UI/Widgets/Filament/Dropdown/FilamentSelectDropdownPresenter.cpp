/*
 * FilamentSelectDropdownPresenter.cpp
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#include "FilamentSelectDropdownPresenter.h"
#include "Debug.h"
#include "FilamentSelectDropdown.h"
#include "ObjectModel/Files.h"
#include "i18n/i18n.h"

namespace UI
{
	void FilamentSelectDropdownPresenter::setSelectedToolBySlot(size_t slot)
	{
		m_selectedTool = OM::GetToolBySlot(slot);
		getView()->setFilamentSelected(m_selectedTool->GetFilament().c_str());
	}

	void FilamentSelectDropdownPresenter::setFilament(const std::string& filamentName)
	{
		if (m_selectedTool == nullptr)
		{
			LOG_ERROR("No tool selected, cannot set filament");
			return;
		}
		m_selectedTool->ChangeFilament(filamentName);
	}

	void FilamentSelectDropdownPresenter::unloadFilament()
	{
		if (m_selectedTool == nullptr)
		{
			LOG_ERROR("No tool selected, cannot unload filament");
			return;
		}

		m_selectedTool->UnloadFilament();
	}

	void FilamentSelectDropdownPresenter::clear()
	{
		m_selectedTool.reset();
		m_filamentOptions.clear();
		updateFilamentList();
	}

	void FilamentSelectDropdownPresenter::newToolData()
	{
		MODEL_LOCK();

		if (OM::GetToolCount() == 0)
		{
			clear();
			return;
		}

		if (m_selectedTool)
		{
			getView()->setFilamentSelected(m_selectedTool->GetFilament().c_str());
		}
	}

	void FilamentSelectDropdownPresenter::updateFilamentList()
	{
		getView()->setFilamentOptions(m_filamentOptions);
	}

	void FilamentSelectDropdownPresenter::onActivate()
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
