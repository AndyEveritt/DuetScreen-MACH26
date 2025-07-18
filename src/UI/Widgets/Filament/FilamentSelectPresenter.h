/*
 * FilamentSelectPresenter.h
 *
 *  Created on: 2025-07-15
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FilamentSelect;

	class FilamentSelectPresenter : public Presenter<FilamentSelect>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FilamentSelectPresenter, FilamentSelect);

		struct ToolData
		{
			std::string toolName;
			std::string filamentName;
		};

		// Setters

		// Getters

		// Actions

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

		std::vector<ToolData> m_toolData;
		std::vector<std::string> m_filamentOptions;
		int8_t m_selectedTool = -1;
	};
} // namespace UI
