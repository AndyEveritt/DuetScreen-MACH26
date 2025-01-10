#include "Debug.h"

#include "ObjectModel/Tool.h"
#include "ToolSubscribers.h"

bool ToolSubscribers::nullTool(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::RemoveTool(indices[0], false);
	return true;
}

bool ToolSubscribers::toolHeater(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolHeater(indices[0], indices[1], (uint8_t)data))
	{
		error("Failed to update tool %d heater %d", indices[0], indices[1]);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolExtruder(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolExtruder(indices[0], indices[1], (uint8_t)data))
	{
		error("Failed to update tool %d extruder %d", indices[0], indices[1]);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolFan(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolFan(indices[0], indices[1], (uint8_t)data))
	{
		error("Failed to update tool %d fan %d", indices[0], indices[1]);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolFilamentExtruder(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolFilamentExtruder(indices[0], (int8_t)data))
	{
		error("Failed to update tool %d filamentExtruder to %d", indices[0], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolActiveTemp(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolTemp(indices[0], indices[1], data, true))
	{
		error("Failed to update tool %d active temperature[%d] to %d", indices[0], indices[1], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolStandbyTemp(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolTemp(indices[0], indices[1], data, false))
	{
		error("Failed to update tool %d standby temperature[%d] to %d", indices[0], indices[1], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolSpindle(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolSpindle(indices[0], data))
	{
		error("Failed to update tool %d spindle to %d", indices[0], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolSpindleRpm(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateToolSpindleRpm(indices[0], data))
	{
		error("Failed to update tool %d spindleRpm to %d", indices[0], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolName(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	if (!OM::UpdateToolName(indices[0], data))
	{
		error("Failed to update tool %d name to %s", indices[0], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolMix(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::UpdateToolMix(indices[0], indices[1], data))
	{
		error("Failed to update tool %d mix %d to %.2f", indices[0], indices[1], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolState(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	if (!OM::UpdateToolStatus(indices[0], data))
	{
		error("Failed to update tool %d name to %s", indices[0], data);
		return false;
	}
	return true;
}

bool ToolSubscribers::toolArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveTool(indices[0], true))
	{
	}
	return true;
}

bool ToolSubscribers::toolHeaterArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveToolHeaters(indices[0], indices[1]))
	{
	}
	return true;
}

bool ToolSubscribers::toolExtruderArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveToolExtruders(indices[0], indices[1]))
	{
	}
	return true;
}

bool ToolSubscribers::toolFanArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveToolFans(indices[0], indices[1]))
	{
	}
	return true;
}
