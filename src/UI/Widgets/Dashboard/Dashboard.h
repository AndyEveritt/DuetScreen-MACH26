/*
 * Dashboard.h
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#pragma once

#include "DashboardPresenter.h"
#include "UI/Components/Containers/TabView.h"
#include "UI/Core/View.h"
#include "UI/Screens/File/FileView.h"
#include "UI/Screens/Status/StatusView.h"
#include "UI/Widgets/Temperature/TemperatureGraph.h"
#include "UI/Widgets/ToolList/ToolList.h"

namespace UI
{
	class Dashboard : public View<DashboardPresenter>
	{
	  public:
		Dashboard(const std::string& name, LvObj& parent);

		ToolList& getToolList() { return m_toolList; }
		TemperatureGraph& getGraph() { return m_graph; }
		TabView& getTabs() { return m_tabs; }
		FileView& getFileView() { return m_fileView; }
		StatusView& getStatusView() { return m_statusView; }

		void showJobsTab() { m_tabs.setActiveTab(0); }
		void showStatusTab() { m_tabs.setActiveTab(1); }
		void disableJobsTab(bool disable);

		void clear();

	  private:
		ToolList m_toolList{"tool_list", getRoot(), this};
		TemperatureGraph m_graph{"graph", getRoot()};

		TabView m_tabs{"tabs", getRoot()};
		FileView m_fileView;
		StatusView m_statusView;
	};
} // namespace UI
