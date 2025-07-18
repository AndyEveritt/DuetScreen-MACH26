/*
 * FilamentSelectPresenter.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/Tool.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class FilamentSelect;

	class FilamentSelectPresenter : public Presenter<FilamentSelect>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FilamentSelectPresenter, FilamentSelect);

		// Setters
		void setSelectedToolBySlot(size_t slot);

		// Getters

		// Actions
		void setFilament(std::string_view filamentName);

		// Observers
		void clear();
		void newToolData();
		void updateFilamentList();

	  protected:
		virtual void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &FilamentSelectPresenter::newToolData);
		}
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() { clear(); }

		std::vector<OM::ToolPtr> m_tools;
		std::vector<std::string> m_filamentOptions;
		OM::ToolPtr m_selectedTool;
	};
} // namespace UI
