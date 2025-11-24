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
		m_tool = OM::GetToolBySlot(slot);
		getView()->setFilamentSelected(m_tool->GetFilament().c_str());
	}

	void FilamentSelectDropdownPresenter::setFilament(const std::string& filamentName)
	{
		if (m_tool == nullptr)
		{
			LOG_ERROR("No tool selected, cannot set filament");
			return;
		}
		m_tool->ChangeFilament(filamentName);
	}

	void FilamentSelectDropdownPresenter::unloadFilament()
	{
		if (m_tool == nullptr)
		{
			LOG_ERROR("No tool selected, cannot unload filament");
			return;
		}

		m_tool->UnloadFilament();
	}

	void FilamentSelectDropdownPresenter::clear()
	{
		m_tool.reset();
		m_filamentOptions.clear();
		updateFilamentList();
	}

	void FilamentSelectDropdownPresenter::newToolData()
	{
		MODEL_LOCK();

		if (m_tool)
		{
			getView()->setFilamentSelected(m_tool->GetFilament().c_str());
		}
	}

	void FilamentSelectDropdownPresenter::newFilaments(const std::vector<std::string>& filaments)
	{
		MODEL_LOCK();
		m_filamentOptions = filaments;
		updateFilamentList();
	}

	void FilamentSelectDropdownPresenter::updateFilamentList()
	{
		getView()->setFilamentOptions(m_filamentOptions);
	}

	void FilamentSelectDropdownPresenter::onActivate()
	{
		m_filamentOptions = OM::FileSystem::GetFilamentList();
		updateFilamentList();
	}
} // namespace UI
