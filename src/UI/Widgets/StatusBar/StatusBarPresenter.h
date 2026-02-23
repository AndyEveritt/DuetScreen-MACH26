#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class StatusBar;

	class StatusBarPresenter : public Presenter<StatusBar>
	{
	  public:
		PRESENTER_CONSTRUCTOR(StatusBarPresenter, StatusBar)

		// Actions

		// Observers
		void newDuetName();
		void newStatus(const OM::PrinterStatus& status);
		void newTime();
		void newHeaterData();

	  protected:
		void onInit() override
		{
			registerEventListener<EventType::NetworkName>(this, &StatusBarPresenter::newDuetName);
			registerEventListener<EventType::Status>(this, &StatusBarPresenter::newStatus);
			registerEventListener<EventType::Time>(this, &StatusBarPresenter::newTime);
			registerEventListener<EventType::HeaterData>(this, &StatusBarPresenter::newHeaterData);
		}
		void onActivate() override;
		void onDeactivate() override {}
		void onConnect() override;
		void onDisconnect() override;
	};
} // namespace UI
