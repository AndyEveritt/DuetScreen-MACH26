/*
 * FilamentSelectMultiPresenter.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/Tool.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class FilamentSelectMulti;

	class FilamentSelectMultiPresenter : public Presenter<FilamentSelectMulti>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FilamentSelectMultiPresenter, FilamentSelectMulti);

		// Setters
		void setSelectedToolBySlot(size_t slot);

		// Getters

		// Actions
		void setFilament(std::string_view filamentName);
		void unloadFilament();

		// Observers
		void clear();
		void newToolData();
		void updateFilamentList();

	  protected:
		void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &FilamentSelectMultiPresenter::newToolData);
		}
		void onActivate() override;
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override { clear(); }

		std::vector<OM::ToolPtr> m_tools;
		std::vector<std::string> m_filamentOptions;
		OM::ToolPtr m_selectedTool;
	};
} // namespace UI
