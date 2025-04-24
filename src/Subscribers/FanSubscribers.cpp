/*
 * FanSubscribers.cpp
 *
 *  Created on: 4 Jan 2024
 *      Author: Andy Everitt
 */
#include "Debug.h"

#include "Configuration.h"
#include "FanSubscribers.h"

#include "ObjectModel/Fan.h"
#include "UI/Core/Model.h"

bool FanSubscribers::nullFan(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	OM::RemoveFan(indices[0], false);
	return true;
}

bool FanSubscribers::actualValue(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::UpdateFanActualVal(indices[0], data))
	{
		LOG_ERROR("Failed to update fan {:d} actualValue to {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool FanSubscribers::requestedValue(Comm::JsonDecoder* decoder, const float& data, const size_t indices[])
{
	if (!OM::UpdateFanRequestedVal(indices[0], data))
	{
		LOG_ERROR("Failed to update fan {:d} requestedValue to {:g}", indices[0], data);
		return false;
	}
	return true;
}

bool FanSubscribers::rpm(Comm::JsonDecoder* decoder, const int32_t& data, const size_t indices[])
{
	if (!OM::UpdateFanRpm(indices[0], data))
	{
		LOG_ERROR("Failed to update fan {:d} rpm to {:d}", indices[0], data);
		return false;
	}
	return true;
}

bool FanSubscribers::arrayEnd(Comm::JsonDecoder* decoder, const size_t indices[])
{
	if (OM::RemoveFan(indices[0], true))
	{
	}
	Model::get().newFanData();
	return true;
}
