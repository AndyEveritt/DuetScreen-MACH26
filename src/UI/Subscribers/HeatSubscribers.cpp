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

bool HeatSubscribers::nullHeater(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::Heat::RemoveHeater(indices[0], false);
	return true;
}

bool HeatSubscribers::bedHeater(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (data > -1)
	{
		OM::SetBedHeater(indices[0], data);
		for (size_t i = OM::g_lastBed + 1; i < indices[0]; ++i)
		{
			OM::RemoveBed(i, false);
		}
		OM::g_lastBed = indices[0];
		dbg("g_lastBed=%d", OM::g_lastBed);
	}
	return true;
}

bool HeatSubscribers::chamberHeater(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (data > -1)
	{
		OM::SetChamberHeater(indices[0], data);
		for (size_t i = OM::g_lastChamber + 1; i < indices[0]; ++i)
		{
			OM::RemoveChamber(i, false);
		}
		OM::g_lastChamber = indices[0];
		dbg("g_lastChamber=%d", OM::g_lastChamber);
	}
	return true;
}

bool HeatSubscribers::heaterCurrentTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterTemp(indices[0], data))
	{
		error("Failed to update heater temperature; heater %d = %fC", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterActiveTemperature(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterTarget(indices[0], data, true))
	{
		error("Failed to update heater %d active temperature to %d", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterStandbyTemperature(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterTarget(indices[0], data, false))
	{
		error("Failed to update heater %d standby temperature to %d", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterAvgPwm(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterPwm(indices[0], data))
	{
		error("Failed to update heater %d avgPwm to %.3f", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterMinTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterMin(indices[0], data))
	{
		error("Failed to update heater %d min to %.3f", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterMaxTemperature(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterMax(indices[0], data))
	{
		error("Failed to update heater %d max to %.3f", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterSensorNum(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::Heat::UpdateHeaterSensor(indices[0], data))
	{
		error("Failed to update heater %d sensor to %d", indices[0], data);
		return false;
	}
	return true;
}

bool HeatSubscribers::heaterState(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::Heat::UpdateHeaterStatus(indices[0], data);
	return true;
}

bool HeatSubscribers::heaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::Heat::RemoveHeater(indices[0], true))
	{
	}
	return true;
}

bool HeatSubscribers::bedHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveBed(indices[0], true))
	{
	}
	return true;
}

bool HeatSubscribers::chamberHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveChamber(indices[0], true))
	{
	}
	return true;
}
