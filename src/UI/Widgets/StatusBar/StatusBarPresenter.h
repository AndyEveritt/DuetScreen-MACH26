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

	  protected:
		virtual void onInit() override
		{
			registerEventListener<EventType::NetworkName>(this, &StatusBarPresenter::newDuetName);
			registerEventListener<EventType::Status>(this, &StatusBarPresenter::newStatus);
			registerEventListener<EventType::Time>(this, &StatusBarPresenter::newTime);
		}
		virtual void onActivate() override;
		virtual void onDeactivate() override {}
		virtual void onConnect() override;
		virtual void onDisconnect() override;
	};
} // namespace UI
