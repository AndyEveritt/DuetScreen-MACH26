/*
 * ToolListPresenter.cpp
 *
 *  Created on: 2025-01-16
 *      Author: Andy Everitt
 */

#include "ToolListPresenter.h"
#include "Configuration.h"
#include "Debug.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Tool.h"
#include "ToolList.h"
#include "UI/Core/Model.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void ToolListItemPresenter::setSlotIndex(int8_t index)
	{
		if (index >= MAX_SLOTS)
		{
			error("Invalid tool index %u", index);
			return;
		}
		m_slotIndex = index;
	}

	void ToolListItemPresenter::newToolData()
	{
		if (m_slotIndex < 0)
		{
			warn("Tool index not set");
			return;
		}

		uint8_t count = 0;
		OM::Tool* tool = nullptr;
		uint8_t tHeaterIndex = 0;
		OM::ToolHeater* tHeater = nullptr;
		OM::Spindle* spindle = nullptr;

		OM::IterateToolsWhile(
			[&](OM::Tool*& toolIter, size_t)
			{
				const bool hasHeater = toolIter->GetHeaterCount() > 0;
				const bool hasSpindle = toolIter->spindle != nullptr;

				if (m_slotIndex < count + toolIter->GetHeaterCount())
				{
					tool = toolIter;
					tHeaterIndex = m_slotIndex - count;
					tHeater = tool->GetHeater(tHeaterIndex);
					return false;
				}
				count += toolIter->GetHeaterCount();

				// Check if it is a spindle
				if (m_slotIndex < count + (hasSpindle ? 1u : 0u))
				{
					tool = toolIter;
					spindle = toolIter->spindle;
					return false;
				}
				count += (hasSpindle ? 1u : 0u);

				// Also want to display empty tools
				count += (!hasHeater && !hasSpindle) ? 1u : 0u;
				if (m_slotIndex < count)
				{
					tool = toolIter;
					return false;
				}
				return true;
			});

		if (tool != nullptr)
		{
			updateView(tool, tHeater, tHeaterIndex, spindle);
			return;
		}

		int8_t bedOrChamberIndex = m_slotIndex - count;
		OM::BedOrChamber* bedOrChamber = OM::GetBedBySlot(bedOrChamberIndex);
		OM::Heat::Heater* heater;
		if (bedOrChamber != nullptr)
		{
			heater = OM::Heat::GetHeater(bedOrChamber->heater);
			if (heater == nullptr)
			{
				warn("List index %d: Bed %d heater %d is null", m_slotIndex, bedOrChamber->index, bedOrChamber->heater);
				return;
			}
			updateView(bedOrChamber, heater, true);
			return;
		}

		bedOrChamberIndex -= OM::GetBedCount();
		bedOrChamber = OM::GetChamberBySlot(bedOrChamberIndex);
		if (bedOrChamber != nullptr)
		{
			heater = OM::Heat::GetHeater(bedOrChamber->heater);
			if (heater == nullptr)
			{
				warn("List index %d: Bed %d heater %d is null", m_slotIndex, bedOrChamber->index, bedOrChamber->heater);
				return;
			}
			updateView(bedOrChamber, heater, false);
			return;
		}
		warn("Unknown index");
	}

	bool ToolListItemPresenter::updateView(const OM::Tool* tool,
										   const OM::ToolHeater* tHeater,
										   const uint8_t tHeaterIndex,
										   const OM::Spindle* spindle)
	{
		if (tool == nullptr)
		{
			return false;
		}

		// Set tool name
		std::string toolName = tool->name.IsEmpty()
								   ? utils::format("%s %d", _("default_tool_name"), tool->index).c_str()
								   : tool->name.c_str();

		if (tHeater != nullptr)
		{
			if (tool->GetHeaterCount() > 1)
			{
				m_view->setLabel(utils::format("%s (%d)", toolName.c_str(), tHeaterIndex).c_str());
			}
			else
			{
				m_view->setLabel(toolName.c_str());
			}
			m_view->setStatus(tHeater->heater->GetHeaterStatusStr());
			m_view->setCurrentTemp(tHeater->heater->current);
			m_view->setActiveTemp(tHeater->activeTemp);
			m_view->setStandbyTemp(tHeater->standbyTemp);
			m_view->showTemps(true);
		}

		else if (spindle != nullptr)
		{
			m_view->setLabel((toolName + " (spindle)").c_str());
			m_view->setActiveTemp(tool->spindleRpm);
			m_view->setStandbyTempText("RPM");
			m_view->setCurrentTemp(spindle->current);
			m_view->setStatus(_(spindle->GetStateStr()));
			m_view->showTemps(true);
		}

		else
		{
			m_view->setLabel(toolName.c_str());
			m_view->showTemps(false);
		}

		return true;
	}
	bool ToolListItemPresenter::updateView(const OM::BedOrChamber* bedOrChamber,
										   const OM::Heat::Heater* heater,
										   const bool bed)
	{
		if (bedOrChamber == nullptr)
		{
			error("BedOrChamber is null");
			return false;
		}

		if (heater == nullptr)
		{
			error("Heater is null");
			return false;
		}

		const char* bedOrChamberName = bed ? _("bed") : _("chamber");

		if (OM::GetBedCount() > 1)
		{
			m_view->setLabel(utils::format("%s %d", bedOrChamberName, bedOrChamber->index).c_str());
		}
		else
		{
			m_view->setLabel(bedOrChamberName);
		}

		m_view->setStatus(_(heater->GetHeaterStatusStr()));
		m_view->setCurrentTemp(heater->current);
		m_view->setActiveTemp(heater->activeTemp);
		m_view->setStandbyTemp(heater->standbyTemp);
		m_view->showTemps(true);
		return true;
	}

	void ToolListItemPresenter::setActiveTemp(int32_t value)
	{
		if (m_slotIndex < 0)
		{
			warn("Tool index not set");
			return;
		}
		OM::Tool* tool = OM::GetTool(m_slotIndex);
		if (tool == nullptr)
		{
			error("Tool %u not found", m_slotIndex);
			return;
		}

		// TODO need to set the tool heater index
		tool->SetHeaterTemps(0, value, true);
	}

	void ToolListPresenter::newToolData()
	{
		const size_t toolCount = getTotalHeaterCount();
		m_view->setItemCnt(toolCount);
		for (size_t i = 0; i < m_view->getItemCnt(); ++i)
		{
			auto item = m_view->getToolListItem(i);
			if (item != nullptr)
			{
				item->setSlotIndex(i);
			}
		}
	}

	size_t ToolListPresenter::getTotalHeaterCount(const bool addTools, const bool addBeds, const bool addChambers)
	{

		size_t totalHeaterCount = 0;
		if (addTools)
		{
			size_t count = 0;
			OM::IterateToolsWhile(
				[&count](OM::Tool*& tool, size_t index)
				{
					const bool hasHeater = tool->GetHeaterCount() > 0;
					const bool hasSpindle = tool->spindle != nullptr;

					// Heater takes precendence (like in DWC)
					if (hasHeater)
					{
						count += tool->GetHeaterCount();
					}
					if (hasSpindle || !hasHeater)
					{
						count++;
					}
					return true;
				});
			verbose("Tool count: %u", count);
			totalHeaterCount += count;
		}
		if (addBeds)
		{
			size_t bedCount = OM::GetBedCount();
			verbose("Bed count: %u", bedCount);
			totalHeaterCount += bedCount;
		}
		if (addChambers)
		{
			size_t chamberCount = OM::GetChamberCount();
			verbose("Chamber count: %u", chamberCount);
			totalHeaterCount += chamberCount;
		}
		return totalHeaterCount;
	}
} // namespace UI
