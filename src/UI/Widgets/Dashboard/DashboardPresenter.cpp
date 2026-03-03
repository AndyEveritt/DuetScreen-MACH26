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
		getView()->disableJobsTab(OM::IsPrintingStatus(status));
	}

	void DashboardPresenter::newJobFileName(const std::string& filename)
	{
		ZoneScoped;
		if (filename.empty())
			return;
		getView()->showStatusTab();
	}
} // namespace UI
