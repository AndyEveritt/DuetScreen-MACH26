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
		void updateFilamentList();

	  protected:
		virtual void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &FilamentSelectDropdownPresenter::newToolData);
		}
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override { clear(); }

		std::vector<std::string> m_filamentOptions;
		OM::ToolPtr m_selectedTool;
	};
} // namespace UI
