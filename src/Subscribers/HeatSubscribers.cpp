/*
 * HeatObservers.cpp
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "HeatSubscribers.h"

#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Heat.h"

#include "UI/Core/Model.h"

// TODO: this isn't thread safe if there are multiple decoders running simultaneously
static ssize_t s_lastBedIndex = -1;
static ssize_t s_lastChamberIndex = -1;

bool HeatSubscribers::coldExtrudeTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Heat::SetColdExtrudeTemperature(data);
	return true;
}

bool HeatSubscribers::coldRetractTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	OM::Heat::SetColdRetractTemperature(data);
	return true;
}

bool HeatSubscribers::nullHeater(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(data);
	if (OM::Heat::RemoveHeater(indices[0], false) > 0)
	{
		Model::get().post<EventType::HeaterData>();
	}
	return true;
}

bool HeatSubscribers::bedHeater(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (indices[0] == 0)
	{
		s_lastBedIndex = -1;
	}

	if (data > -1)
	{
		OM::SetBedHeater(indices[0], static_cast<int8_t>(data));
		for (size_t i = s_lastBedIndex + 1; i < indices[0]; ++i)
		{
			OM::RemoveBed(i, false);
		}
		s_lastBedIndex = static_cast<int8_t>(indices[0]);
		LOG_DBG("s_lastBedIndex={:d}", s_lastBedIndex);
	}
	return true;
}

bool HeatSubscribers::chamberHeater(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (indices[0] == 0)
	{
		s_lastChamberIndex = -1;
	}

	if (data > -1)
	{
		OM::SetChamberHeater(indices[0], static_cast<int8_t>(data));
		for (size_t i = s_lastChamberIndex + 1; i < indices[0]; ++i)
		{
			OM::RemoveChamber(i, false);
		}
		s_lastChamberIndex = static_cast<int8_t>(indices[0]);
		LOG_DBG("s_lastChamberIndex={:d}", s_lastChamberIndex);
	}
	return true;
}

bool HeatSubscribers::heaterCurrentTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterTemp(indices[0], data))
	{
		LOG_ERROR("Failed to update heater temperature; heater {:d} = {:g}C", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterActiveTemperature(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterTarget(indices[0], data, true))
	{
		LOG_ERROR("Failed to update heater {:d} active temperature to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterStandbyTemperature(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterTarget(indices[0], data, false))
	{
		LOG_ERROR("Failed to update heater {:d} standby temperature to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterAvgPwm(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterPwm(indices[0], data))
	{
		LOG_ERROR("Failed to update heater {:d} avgPwm to {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterMinTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterMin(indices[0], data))
	{
		LOG_ERROR("Failed to update heater {:d} min to {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterMaxTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterMax(indices[0], data))
	{
		LOG_ERROR("Failed to update heater {:d} max to {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterSensorNum(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (!OM::Heat::UpdateHeaterSensor(indices[0], data))
	{
		LOG_ERROR("Failed to update heater {:d} sensor to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterState(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	OM::Heat::UpdateHeaterStatus(indices[0], data);
	return true;
}

bool HeatSubscribers::heaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	if (OM::Heat::RemoveHeater(indices[0], true))
	{
	}
	Model::get().post<EventType::HeaterData>();
	return true;
}

bool HeatSubscribers::bedHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	if (OM::RemoveBed(s_lastBedIndex + 1, true))
	{
	}
	Model::get().post<EventType::BedHeaterData>();
	return true;
}

bool HeatSubscribers::chamberHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	ZoneScoped;
	UNUSED(decoder);
	UNUSED(indices);
	if (OM::RemoveChamber(s_lastChamberIndex + 1, true))
	{
	}
	Model::get().post<EventType::ChamberHeaterData>();
	return true;
}
