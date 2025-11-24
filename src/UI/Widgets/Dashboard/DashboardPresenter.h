/*
 * DashboardPresenter.h
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class Dashboard;

	class DashboardPresenter : public Presenter<Dashboard>
	{
	  public:
		PRESENTER_CONSTRUCTOR(DashboardPresenter, Dashboard);

		// Setters

		// Getters

		// Actions

		// Observers
		void newStatus(const OM::PrinterStatus& status);

	  protected:
		void onInit() override { registerEventListener<EventType::Status>(this, &DashboardPresenter::newStatus); }
		void onActivate() override {}
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}

		OM::PrinterStatus m_lastStatus = OM::PrinterStatus::unknown;
	};
} // namespace UI
