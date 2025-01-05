/*
 * FanObservers.cpp
 *
 *  Created on: 8 Jan 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "FanSubscribers.h"

#include "ObjectModel/Fan.h"

bool FanSubscribers::actualValue(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::UpdateFanActualVal(indices[0], data))
	{
		error("Failed to update fan %d actualValue to %.2f", indices[0], data);
		return false;
	}
	return true;
}

bool FanSubscribers::requestedValue(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::UpdateFanRequestedVal(indices[0], data))
	{
		error("Failed to update fan %d requestedValue to %.2f", indices[0], data);
		return false;
	}
	return true;
}

bool FanSubscribers::rpm(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateFanRpm(indices[0], data))
	{
		error("Failed to update fan %d rpm to %d", indices[0], data);
		return false;
	}
	return true;
}

bool FanSubscribers::arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveFan(indices[0], true))
	{
	}
	return true;
}
