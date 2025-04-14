/*
 * Sensor.h
 *
 *  Created on: 20 Mar 2024
 *      Author: Andy Everitt
 */

#pragma once

#include "Configuration.h"
#include "Duet3D/General/String.h"
#include <Duet3D/General/FreelistManager.h>
#include <memory>

namespace OM
{

	struct AnalogSensor
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<AnalogSensor>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<AnalogSensor>(p); }

		uint8_t index;
		float lastReading;
		long long lastReadingTime;
		String<MAX_TOOL_NAME_LENGTH> name;

		void Reset();
	};

	struct Endstop
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<Endstop>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<Endstop>(p); }

		uint8_t index;
		bool triggered;

		void Reset();
	};

	const size_t GetAnalogSensorCount();
	std::shared_ptr<AnalogSensor> GetAnalogSensor(const size_t index, const bool silent = false);
	std::shared_ptr<AnalogSensor> GetOrCreateAnalogSensor(const size_t index);
	std::shared_ptr<AnalogSensor> GetAnalogSensorBySlot(const size_t index);
	size_t RemoveAnalogSensor(const size_t index, const bool allFollowing);

	bool UpdateAnalogSensorReading(const size_t index, const float reading);
	bool UpdateAnalogSensorName(const size_t index, const char* name);

	const size_t GetEndstopCount();
	std::shared_ptr<Endstop> GetEndstop(const size_t index);
	std::shared_ptr<Endstop> GetOrCreateEndstop(const size_t index);
	size_t RemoveEndstop(const size_t index, const bool allFollowing);

	bool UpdateEndstopTriggered(const size_t index, const bool triggered);

} // namespace OM
