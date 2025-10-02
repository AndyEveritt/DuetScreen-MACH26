/*
 * DashboardPresenter.cpp
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#include "DashboardPresenter.h"
#include "Dashboard.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"

namespace UI
{
	void DashboardPresenter::newStatus(const OM::PrinterStatus& status)
	{
		switch (status)
		{
		case OM::PrinterStatus::cancelling:
		case OM::PrinterStatus::paused:
		case OM::PrinterStatus::pausing:
		case OM::PrinterStatus::printing:
		case OM::PrinterStatus::resuming:
		case OM::PrinterStatus::simulating:
			addHomeScreen(&getView()->getStatusView());
			break;
		case OM::PrinterStatus::idle:
			removeHomeScreen(&getView()->getStatusView(), true);
			break;
		default:
			break;
		}
	}
} // namespace UI
