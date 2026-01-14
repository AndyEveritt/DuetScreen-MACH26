#include "HeaterSliderPresenter.h"
#include "Debug.h"
#include "HeaterSlider.h"
#include "i18n/i18n.h"

namespace UI
{
	void HeaterSliderPresenter::onActivate()
	{
		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			// Do nothing the ToolControlPresenter will handle this
			break;
		}
		case SlotType::Bed:
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				LOG_ERROR("BedOrChamber is null for presenter '{}'", getName());
				reset();
				break;
			}
			auto setter = (m_slotType == SlotType::Bed) ? &HeaterSliderPresenter::setBedIndex
														: &HeaterSliderPresenter::setChamberIndex;
			(this->*setter)(m_slot);
			break;
		}
		default:
			LOG_WARN("HeaterSliderPresenter '{}' has unknown slot type on activate", getName());
			break;
		}

		newHeaterData();
	}

	void HeaterSliderPresenter::setToolHeaterIndex(size_t toolIndex, size_t toolHeaterIndex)
	{
		UI_LOCK();
		reset();
		m_tool = OM::GetTool(toolIndex);
		if (m_tool == nullptr)
		{
			LOG_ERROR("Tool with index {:d} not found", toolIndex);

			reset();
			return;
		}
		m_tHeater = m_tool->GetHeater(toolHeaterIndex);

		if (m_tHeater == nullptr)
		{
			LOG_ERROR("Tool heater with index {:d} not found in tool {:d}", toolHeaterIndex, toolIndex);

			reset();
			return;
		}

		LOG_DBG("Set tool heater index to {} for presenter '{}'", toolHeaterIndex, getName());
		m_slotType = SlotType::Tool;
	}

	void HeaterSliderPresenter::setBedIndex(size_t index)
	{
		UI_LOCK();
		reset();
		m_bedOrChamber = OM::GetBedBySlot(index);
		if (m_bedOrChamber == nullptr)
		{
			LOG_ERROR("Bed with index {:d} not found", index);

			reset();
			return;
		}
		m_slot = index;
		m_slotType = SlotType::Bed;
	}

	void HeaterSliderPresenter::setChamberIndex(size_t index)
	{
		UI_LOCK();
		reset();
		m_bedOrChamber = OM::GetChamberBySlot(index);
		if (m_bedOrChamber == nullptr)
		{
			LOG_ERROR("Chamber with index {:d} not found", index);

			reset();
			return;
		}
		m_slot = index;
		m_slotType = SlotType::Chamber;
	}

	void HeaterSliderPresenter::reset()
	{
		UI_LOCK();

		m_slotType = SlotType::Unknown;
		m_slot = 0;
		m_tool.reset();
		m_tHeater.reset();
		m_bedOrChamber.reset();
	}

	void HeaterSliderPresenter::cycleHeaterState()
	{
		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tool == nullptr || m_tHeater == nullptr)
			{
				LOG_ERROR("Tool or tool heater is not set");
				return;
			}
			m_tool->ToggleHeaterState(m_tHeater->index);
			break;
		}
		case SlotType::Bed:
		{
			if (m_bedOrChamber == nullptr)
			{
				LOG_ERROR("Bed is null");
				return;
			}
			m_bedOrChamber->ToggleBedState();
			break;
		}
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				LOG_ERROR("Chamber is null");
				return;
			}
			m_bedOrChamber->ToggleChamberState();
			break;
		}
		default:
			LOG_WARN("HeaterSliderPresenter '{}' has unknown slot type on cycleHeaterState", getName());
			break;
		}
	}

	void HeaterSliderPresenter::sendTemperature(float value, bool active)
	{
		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tool == nullptr)
			{
				LOG_ERROR("Tool is null");
				return;
			}
			if (m_tHeater == nullptr)
			{
				LOG_ERROR("Tool heater is null");
				return;
			}
			m_tool->SetHeaterTemps(m_tHeater->index, static_cast<int32_t>(value), active);
			break;
		}
		case SlotType::Bed:
		{
			if (m_bedOrChamber == nullptr)
			{
				LOG_ERROR("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->SetBedTemp(static_cast<int32_t>(value), active);
			break;
		}
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				LOG_ERROR("BedOrChamber is null");
				return;
			}
			m_bedOrChamber->SetChamberTemp(static_cast<int32_t>(value), active);
			break;
		}
		default:
			LOG_WARN("HeaterSliderPresenter '{}' has unknown slot type on sendTemperature", getName());
			break;
		}
	}

	void HeaterSliderPresenter::newHeaterData()
	{
		std::string_view name;
		heater_state_t state = heater_state_t::unknown;
		std::string_view state_str;
		int32_t activeTemp = -2000;
		int32_t standbyTemp = -2000;
		float currentTemp = -2000;
		float minTemp = 0.0f;
		float maxTemp = 0.0f;

		OM::Heat::HeaterPtr heater;

		switch (m_slotType)
		{
		case SlotType::Tool:
		{
			if (m_tool == nullptr || m_tHeater == nullptr)
			{
				LOG_ERROR("Tool or ToolHeater is null for presenter '{}'", getName());

				reset();
				return;
			}
			heater = m_tHeater->heater;
			if (heater == nullptr)
			{
				LOG_ERROR("Heater is null for tool {:d} heater {:d}", m_tool->index, m_tHeater->index);

				reset();
				return;
			}

			activeTemp = m_tHeater->activeTemp;
			standbyTemp = m_tHeater->standbyTemp;
			break;
		}
		case SlotType::Bed:
		case SlotType::Chamber:
		{
			if (m_bedOrChamber == nullptr)
			{
				LOG_ERROR(
					"{} is null for presenter '{}'", (m_slotType == SlotType::Bed ? "Bed" : "Chamber"), getName());

				reset();
				return;
			}

			heater = OM::Heat::GetHeater(m_bedOrChamber->heater);
			if (heater == nullptr)
			{
				LOG_ERROR("Heater is null for {} {:d} heater {:d}",
						  (m_slotType == SlotType::Bed ? "Bed" : "Chamber"),
						  m_bedOrChamber->index,
						  m_bedOrChamber->heater);

				reset();
				return;
			}

			activeTemp = m_bedOrChamber->GetActiveTemp();
			standbyTemp = m_bedOrChamber->GetStandbyTemp();
			break;
		}
		default:
			LOG_WARN("HeaterSliderPresenter '{}' has unknown slot type on newHeaterData", getName());
			return;
		}

		if (heater == nullptr)
		{
			return;
		}

		name = heater->GetName();
		state = heater->status;
		state_str = _(fmt::format("temperature.status.{:s}", heater->GetHeaterStatusStr()));
		currentTemp = heater->current;
		minTemp = heater->min;
		maxTemp = heater->max;

		m_view->setHeaterName(name);
		m_view->setHeaterState(state, state_str);
		m_view->setHeaterMinTemperature(minTemp);
		m_view->setHeaterMaxTemperature(maxTemp);
		m_view->setCurrentTemperature(currentTemp);
		m_view->setActiveTemperature(activeTemp);
		m_view->setStandbyTemperature(standbyTemp);
	}
} // namespace UI
