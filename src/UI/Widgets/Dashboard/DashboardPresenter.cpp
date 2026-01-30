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
		ZoneScoped;
		switch (status)
		{
		case OM::PrinterStatus::cancelling:
		case OM::PrinterStatus::paused:
		case OM::PrinterStatus::pausing:
		case OM::PrinterStatus::processing:
		case OM::PrinterStatus::resuming:
		case OM::PrinterStatus::simulating:
			getView()->disableJobsTab(true);
			break;
		case OM::PrinterStatus::idle:
			getView()->disableJobsTab(false);
			break;
		default:
			break;
		}
	}
} // namespace UI
