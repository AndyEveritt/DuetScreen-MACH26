/*
 * ToolListPresenter.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "ToolListPresenter.h"
#include "Configuration.h"
#include "Debug.h"
#include "ObjectModel/Tool.h"
#include "ToolList.h"
#include "UI/Core/Model.h"

namespace UI
{
	void ToolListItemPresenter::setToolIndex(int8_t index)
	{
		if (index >= MAX_SLOTS)
		{
			error("Invalid tool index %u", index);
			return;
		}
		m_toolIndex = index;
	}

	void ToolListItemPresenter::newToolData()
	{
		if (m_toolIndex < 0)
		{
			warn("Tool index not set");
			return;
		}
		OM::Tool* tool = OM::GetTool(m_toolIndex);

		if (tool == nullptr)
		{
			error("Tool %u not found", m_toolIndex);
			return;
		}
		m_view->setLabel(tool->name.c_str());

		OM::ToolHeater* tHeater = tool->GetHeater(0);
		if (tHeater == nullptr)
		{
			error("Tool %u heater not found", m_toolIndex);
			return;
		}

		m_view->setStatus(tHeater->heater->GetHeaterStatusStr());
		m_view->setCurrentTemp(tHeater->heater->current);
		m_view->setActiveTemp(tHeater->activeTemp);
		m_view->setStandbyTemp(tHeater->standbyTemp);
	}

	void ToolListPresenter::newToolData()
	{
		const size_t toolCount = OM::GetToolCount();
		m_view->setItemCnt(toolCount);
		for (size_t i = 0; i < m_view->getItemCnt(); ++i)
		{
			auto item = m_view->getToolListItem(i);
			if (item != nullptr)
			{
				item->setToolIndex(i);
			}
		}
	}
} // namespace UI
