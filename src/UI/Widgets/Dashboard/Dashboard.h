/*
 * Dashboard.h
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#pragma once

#include "DashboardPresenter.h"
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
		FileView& getFileView() { return m_fileView; }
		StatusView& getStatusView() { return m_statusView; }

		void clear();

	  private:
		ToolList m_toolList{"tool_list", getRoot(), this};
		TemperatureGraph m_graph{"graph", getRoot()};
		FileView m_fileView{getRoot(), this};
		StatusView m_statusView{getRoot()};
	};
} // namespace UI
