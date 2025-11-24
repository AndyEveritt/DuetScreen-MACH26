/*
 * FilamentSelectDropdownPresenter.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/Tool.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class FilamentSelectDropdown;

	class FilamentSelectDropdownPresenter : public Presenter<FilamentSelectDropdown>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FilamentSelectDropdownPresenter, FilamentSelectDropdown);

		// Setters
		void setSelectedToolBySlot(size_t slot);

		// Getters

		// Actions
		void setFilament(const std::string& filamentName);
		void unloadFilament();

		// Observers
		void clear();
		void newToolData();
		void newFilaments(const std::vector<std::string>& filaments);
		void updateFilamentList();

	  protected:
		void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &FilamentSelectDropdownPresenter::newToolData);
			registerEventListener<EventType::Filaments>(this, &FilamentSelectDropdownPresenter::newFilaments);
		}
		void onActivate() override;
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override { clear(); }

		std::vector<std::string> m_filamentOptions;
		OM::ToolPtr m_tool;
	};
} // namespace UI
