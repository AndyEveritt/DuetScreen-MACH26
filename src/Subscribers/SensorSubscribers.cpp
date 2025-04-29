#include "Debug.h"

#include "ObjectModel/Sensor.h"
#include "SensorSubscribers.h"
#include "UI/Core/Model.h"

bool SensorSubscribers::nullAnalogSensor(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::RemoveAnalogSensor(indices[0], false);
	Model::get().post<EventType::AnalogSensorData>();
	return true;
}

bool SensorSubscribers::analogSensorReading(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::UpdateAnalogSensorReading(indices[0], data))
	{
		LOG_ERROR("Failed to update analog sensor {:d} reading to {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool SensorSubscribers::analogSensorName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	if (!OM::UpdateAnalogSensorName(indices[0], data))
	{
		LOG_ERROR("Failed to update analog sensor {:d} name to {:s}", indices[0], data);
		return false;
	}
	return true;
}

bool SensorSubscribers::nullEndstop(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::RemoveEndstop(indices[0], false);
	Model::get().post<EventType::EndstopData>();
	return true;
}

bool SensorSubscribers::endstopTriggered(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	if (!OM::UpdateEndstopTriggered(indices[0], data))
	{
		LOG_ERROR("Failed to update endstop {:d} triggered to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool SensorSubscribers::analogSensorArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveAnalogSensor(indices[0], true))
	{
	}
	Model::get().post<EventType::AnalogSensorData>();
	return true;
}

bool SensorSubscribers::endstopArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveEndstop(indices[0], true))
	{
	}
	Model::get().post<EventType::EndstopData>();
	return true;
}
