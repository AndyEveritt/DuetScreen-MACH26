/*
 * Sensor.cpp
 *
 *  Created on: 20 Mar 2024
 *      Author: andy
 */

#include "Debug.h"

#include "ListHelpers.h"
#include "Sensor.h"
#include <Duet3D/General/Vector.h>
#include <utils/TimeHelper.h>

typedef Vector<std::shared_ptr<OM::AnalogSensor>, MAX_SENSORS> AnalogSensorList;
typedef Vector<std::shared_ptr<OM::Endstop>, MAX_ENDSTOPS> EndstopList;

static AnalogSensorList s_analogSensors;
static EndstopList s_endstops;

namespace OM
{
	void AnalogSensor::Reset()
	{
		index = 0;
		lastReading = 0.0;
		lastReadingTime = 0;
		name.Clear();
	}

	void Endstop::Reset()
	{
		index = 0;
		triggered = false;
	}

	const size_t GetAnalogSensorCount()
	{
		return s_analogSensors.Size();
	}

	std::shared_ptr<AnalogSensor> GetAnalogSensor(const size_t index, const bool silent)
	{
		return GetOrCreate<AnalogSensorList, AnalogSensor>(s_analogSensors, index, false, silent);
	}

	std::shared_ptr<AnalogSensor> GetOrCreateAnalogSensor(const size_t index)
	{
		return GetOrCreate<AnalogSensorList, AnalogSensor>(s_analogSensors, index, true);
	}

	std::shared_ptr<AnalogSensor> GetAnalogSensorBySlot(const size_t index)
	{
		if (index >= s_analogSensors.Size())
			return nullptr;

		return s_analogSensors[index];
	}

	size_t RemoveAnalogSensor(const size_t index, const bool allFollowing)
	{
		LOG_DBG("Removing analogSensor %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<AnalogSensorList, AnalogSensor>(s_analogSensors, index, allFollowing);
	}

	bool UpdateAnalogSensorReading(const size_t index, const float reading)
	{
		auto const sensor = GetOrCreateAnalogSensor(index);
		if (sensor == nullptr)
		{
			LOG_WARN("Failed to get or create analog sensor %d", index);
			return false;
		}

		sensor->lastReading = reading;
		sensor->lastReadingTime = TimeHelper::getCurrentTime();
		return true;
	}

	bool UpdateAnalogSensorName(const size_t index, const char* name)
	{
		auto const sensor = GetOrCreateAnalogSensor(index);
		if (sensor == nullptr)
		{
			LOG_WARN("Failed to get or create analog sensor %d", index);
			return false;
		}

		sensor->name.copy(name);
		if (sensor->name.IsEmpty())
		{
			sensor->name.copy("Heater ");
			sensor->name.catf("%lu", index);
		}
		return true;
	}

	const size_t GetEndstopCount()
	{
		return s_endstops.Size();
	}

	std::shared_ptr<Endstop> GetEndstop(const size_t index)
	{
		return GetOrCreate<EndstopList, Endstop>(s_endstops, index, false);
	}

	std::shared_ptr<Endstop> GetOrCreateEndstop(const size_t index)
	{
		return GetOrCreate<EndstopList, Endstop>(s_endstops, index, true);
	}

	size_t RemoveEndstop(const size_t index, const bool allFollowing)
	{
		LOG_DBG("Removing endstop %d (allFollowing=%s)", index, allFollowing ? "true" : "false");
		return Remove<EndstopList, Endstop>(s_endstops, index, allFollowing);
	}

	bool UpdateEndstopTriggered(const size_t index, const bool triggered)
	{
		auto const endstop = GetOrCreateEndstop(index);
		if (endstop == nullptr)
		{
			LOG_WARN("Failed to get or create endstop %d", index);
			return false;
		}

		endstop->triggered = triggered;
		return true;
	}
} // namespace OM
