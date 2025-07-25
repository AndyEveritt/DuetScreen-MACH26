/*
 * ToolControlPresenter.h
 *
 *  Created on: 2025-07-14
 *      Author: Andy Everitt
 */

#pragma once

#include "ObjectModel/Tool.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class ToolControl;

	class ToolControlPresenter : public Presenter<ToolControl>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ToolControlPresenter, ToolControl);

        using tool_state_t = OM::ToolStatus;

		// Setters
		void reset();
		void setToolIndex(size_t toolIndex);

		// Getters

		// Actions
		void toggleToolState();

		// Observers
        void newToolData();
		void newToolHeaterData(size_t toolIndex);

	  protected:
		virtual void onInit() override {
            registerEventListener<EventType::ToolData>(this, &ToolControlPresenter::newToolData);
			registerEventListener<EventType::ToolHeaterData>(this, &ToolControlPresenter::newToolHeaterData);
		}
		virtual void onActivate() override;
		virtual void onDeactivate() override;
		virtual void onDisconnect() override { reset(); }

		ssize_t m_toolSlot;
		OM::ToolPtr m_tool;
	};
} // namespace UI
