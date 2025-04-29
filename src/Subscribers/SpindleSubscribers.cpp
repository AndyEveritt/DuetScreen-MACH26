#include "Debug.h"

#include "ObjectModel/Spindle.h"
#include "SpindleSubscribers.h"
#include "UI/Core/Model.h"

bool SpindleSubscribers::nullSpindle(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::RemoveSpindle(indices[0], false);
	Model::get().post<EventType::SpindleData>();
	return true;
}

bool SpindleSubscribers::activeSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::SetSpindleActive(indices[0], data))
	{
		LOG_ERROR("Failed to update spindle {:d} active to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool SpindleSubscribers::canReverse(Comm::JsonDecoder* decoder, const bool& data, const size_t indices[])
{
	if (!OM::SetSpindleCanReverse(indices[0], data))
	{
		LOG_ERROR("Failed to update spindle {:d} canReverse to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool SpindleSubscribers::currentSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::SetSpindleCurrent(indices[0], data))
	{
		LOG_ERROR("Failed to update spindle {:d} current to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool SpindleSubscribers::maxSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::SetSpindleMax(indices[0], data))
	{
		LOG_ERROR("Failed to update spindle {:d} max to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool SpindleSubscribers::minSpeed(Comm::JsonDecoder* decoder, const uint32_t& data, const size_t indices[])
{
	if (!OM::SetSpindleMin(indices[0], data))
	{
		LOG_ERROR("Failed to update spindle {:d} min to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool SpindleSubscribers::state(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	if (!OM::SetSpindleState(indices[0], data))
	{
		LOG_ERROR("Failed to update spindle {:d} state to {:s}", indices[0], data);
		return false;
	}
	return true;
}

bool SpindleSubscribers::spindleArrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveSpindle(indices[0], true))
	{
	}
	Model::get().post<EventType::SpindleData>();
	return true;
}