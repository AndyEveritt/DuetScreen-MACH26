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
	int8_t ToolListItemPresenter::getSlotIndex() const
	{
		return m_slotIndex;
	}

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
		update();
	}

	void ToolListItemPresenter::newHeaterData()
	{
		update();
	}

	void ToolListItemPresenter::update()
	{
		if (m_slotIndex < 0)
		{
			warn("Tool index not set");
			return;
		}

		uint8_t count = 0;
		uint8_t tHeaterIndex = 0;
		m_tool = nullptr;
		m_tHeater = nullptr;
		m_spindle = nullptr;

		OM::IterateToolsWhile(
			[&](OM::Tool*& toolIter, size_t)
			{
				const bool hasHeater = toolIter->GetHeaterCount() > 0;
				const bool hasSpindle = toolIter->spindle != nullptr;

				if (m_slotIndex < count + toolIter->GetHeaterCount())
				{
					m_tool = toolIter;
					tHeaterIndex = m_slotIndex - count;
					m_tHeater = m_tool->GetHeater(tHeaterIndex);
					return false;
				}
				count += toolIter->GetHeaterCount();

				// Check if it is a spindle
				if (m_slotIndex < count + (hasSpindle ? 1u : 0u))
				{
					m_tool = toolIter;
					m_spindle = toolIter->spindle;
					return false;
				}
				count += (hasSpindle ? 1u : 0u);

				// Also want to display empty tools
				count += (!hasHeater && !hasSpindle) ? 1u : 0u;
				if (m_slotIndex < count)
				{
					m_tool = toolIter;
					return false;
				}
				return true;
			});

		if (m_tool != nullptr)
		{
			m_slotType = SlotType::Tool;
			updateView(m_tool, m_tHeater, tHeaterIndex, m_spindle);
			return;
		}

		int8_t bedOrChamberIndex = m_slotIndex - count;
		m_bedOrChamber = OM::GetBedBySlot(bedOrChamberIndex);
		OM::Heat::Heater* heater;
		if (m_bedOrChamber != nullptr)
		{
			m_slotType = SlotType::Bed;
			heater = OM::Heat::GetHeater(m_bedOrChamber->heater);
			if (heater == nullptr)
			{
				warn("List index %d: Bed %d heater %d is null",
					 m_slotIndex,
					 m_bedOrChamber->index,
					 m_bedOrChamber->heater);
				return;
			}
			updateView(m_bedOrChamber, heater, true);
			return;
		}

		bedOrChamberIndex -= OM::GetBedCount();
		m_bedOrChamber = OM::GetChamberBySlot(bedOrChamberIndex);
		if (m_bedOrChamber != nullptr)
		{
			m_slotType = SlotType::Chamber;
			heater = OM::Heat::GetHeater(m_bedOrChamber->heater);
			if (heater == nullptr)
			{
				warn("List index %d: Bed %d heater %d is null",
					 m_slotIndex,
					 m_bedOrChamber->index,
					 m_bedOrChamber->heater);
				return;
			}
			updateView(m_bedOrChamber, heater, false);
			return;
		}
		warn("Unknown index");
		m_slotType = SlotType::Unknown;
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

	void ToolListItemPresenter::setTemp(int32_t value)
	{
		if (m_slotIndex < 0)
		{
			warn("Tool index not set");
			return;
		}
		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tool == nullptr)
			{
				error("Tool is null");
				return;
			}
			if (m_tHeater == nullptr)
			{
				error("Tool heater is null");
				return;
			}

			m_tool->SetHeaterTemps(m_tHeater->index, value, m_setActiveTemp);
			break;
		}
		case SlotType::Bed:
		{
			if (m_bedOrChamber == nullptr)
			{
				error("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->SetBedTemp(value, m_setActiveTemp);
			break;
		}
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				error("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->SetChamberTemp(value, m_setActiveTemp);
			break;
		}
		}
	}

	bool ToolListItemPresenter::configureNumberPad(const bool active)
	{
		ToolListNumPad& np = m_view->getToolList().m_numberPad;
		OM::Heat::Heater* heater = nullptr;
		std::string header;

		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tHeater != nullptr)
			{
				heater = m_tHeater->heater;
				header = utils::format(_("tool_list_numpad_header_tool_heater"),
									   m_tool->index,
									   m_tHeater->index,
									   active ? _("active") : _("standby"));
				break;
			}

			if (m_spindle != nullptr)
			{
				header = utils::format(_("tool_list_numpad_header_tool_heater"),
									   m_tool->index,
									   m_tHeater->index,
									   active ? _("active") : _("standby"));
			}
			warn("Tool heater is null");
			return false;
		}
		case SlotType::Bed:
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				warn("BedOrChamber is null");
				return false;
			}
			header = utils::format(_("tool_list_numpad_header_bed_chamber"),
								   m_slotType == SlotType::Bed ? _("bed") : _("chamber"),
								   m_bedOrChamber->index,
								   active ? _("active") : _("standby"));

			heater = OM::Heat::GetHeater(m_bedOrChamber->heater);
			break;
		}
		}

		np.setHeader(header.c_str());
		if (heater != nullptr)
		{
			m_setActiveTemp = active;
			np.setMinValue(heater->min);
			np.setMaxValue(heater->max);
			np.setConfirmCallback(numberPadConfirmCallback, this);
			return true;
		}
		if (m_spindle != nullptr)
		{
			np.setMinValue(m_spindle->min);
			np.setMaxValue(m_spindle->max);
			// TODO Set confirm callback
			return true;
		}

		warn("Heater is null");
		return false;
	}

	void ToolListItemPresenter::toggleState()
	{
		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tool == nullptr)
			{
				error("Tool is null");
				return;
			}
			m_tool->ToggleState();
			break;
		}
		case SlotType::Bed:
		{
			if (m_bedOrChamber == nullptr)
			{
				error("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->ToggleBedState();
			break;
		}
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				error("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->ToggleChamberState();
			break;
		}
		}
	}

	void ToolListItemPresenter::toggleSubState()
	{
		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tool == nullptr)
			{
				error("Tool is null");
				return;
			}
			if (m_tHeater != nullptr)
			{
				m_tool->ToggleHeaterState(m_tHeater->index);
				break;
			}
			if (m_spindle != nullptr)
			{
				m_tool->ToggleSpindleState();
				break;
			}
			warn("No heater or spindle");
			break;
		}
		case SlotType::Bed:
		{
			if (m_bedOrChamber == nullptr)
			{
				error("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->ToggleBedState();
			break;
		}
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				error("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->ToggleBedState();
			break;
		}
		}
	}

	void ToolListItemPresenter::numberPadConfirmCallback(lv_event_t* e)
	{
		ToolListItemPresenter* presenter = (ToolListItemPresenter*)lv_event_get_user_data(e);
		NumberPad* np = (NumberPad*)lv_event_get_param(e);
		if (presenter == nullptr)
		{
			error("Presenter is null");
			return;
		}
		presenter->setTemp(np->getValue());
	}

	void ToolListPresenter::newToolData()
	{
		update();
	}

	void ToolListPresenter::newHeaterData()
	{
		update();
	}

	void ToolListPresenter::update()
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
