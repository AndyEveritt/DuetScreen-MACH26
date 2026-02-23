/*
 * Heat.h
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#pragma once

#include "Configuration.h"
#include "Sensor.h"
#include <Duet3D/General/Bitmap.h>
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringRef.h>
#include <Duet3D/General/function_ref.h>
#include <memory>
#include <sys/types.h>

namespace OM
{
	namespace Heat
	{
		// Status that a tool may report to us. Must be in alphabetical order.
		enum class HeaterStatus
		{
			active = 0,
			fault,
			off,
			offline,
			standby,
			tuning,
			unknown,
		};

		struct HeaterStatusMapEntry
		{
			const char* key;
			HeaterStatus val;
		};

		// This table must be kept in case-insensitive alphabetical order of the search string.
		const HeaterStatusMapEntry heaterStatusMap[] = {
			{"active", HeaterStatus::active},
			{"fault", HeaterStatus::fault},
			{"off", HeaterStatus::off},
			{"offline", HeaterStatus::offline},
			{"standby", HeaterStatus::standby},
			{"tuning", HeaterStatus::tuning},
			{"unknown", HeaterStatus::unknown},
		};

		struct Heater
		{
			void* operator new(size_t) noexcept { return FreelistManager::Allocate<Heater>(); }
			void operator delete(void* p) noexcept { FreelistManager::Release<Heater>(p); }

			size_t index; // This is the heater number
			int32_t activeTemp;
			int32_t standbyTemp;
			float current;
			float avgPwm;
			float min;
			float max;
			HeaterStatus status;
			std::shared_ptr<AnalogSensor> sensor;

			void Reset();
			std::string_view GetName() const;
			const char* GetHeaterStatusStr() const;
			void UpdateTarget(const int32_t temp, const bool active);
			void UpdateTemp(const float value) { current = value; }
			void UpdatePwm(const float value) { avgPwm = value; }
			void UpdateMin(const float value) { this->min = value; }
			void UpdateMax(const float value) { this->max = value; }

			int32_t GetTargetTemp() const
			{
				return (status == HeaterStatus::active)	   ? activeTemp
					   : (status == HeaterStatus::standby) ? standbyTemp
														   : 0;
			}
		};

		using HeaterPtr = std::shared_ptr<Heater>;

		HeaterPtr GetHeater(const size_t heaterIndex);
		HeaterPtr GetOrCreateHeater(const size_t heaterIndex);
		HeaterPtr GetHeaterBySlot(const size_t heaterSlot);
		size_t GetHeaterCount() noexcept;
		bool IterateHeatersWhile(function_ref<bool(HeaterPtr, size_t)> func, const size_t startAt = 0);
		bool UpdateHeaterTarget(const size_t heaterIndex, const int32_t temp, const bool active);
		bool UpdateHeaterTemp(const size_t heaterIndex, const float temp);
		bool UpdateHeaterPwm(const size_t heaterIndex, const float pwm);
		bool UpdateHeaterMin(const size_t heaterIndex, const float min);
		bool UpdateHeaterMax(const size_t heaterIndex, const float max);
		bool UpdateHeaterStatus(const size_t heaterIndex, HeaterStatus status);
		bool UpdateHeaterStatus(const size_t heaterIndex, const char* status);
		bool UpdateHeaterSensor(const size_t heaterIndex, const size_t sensorIndex);
		size_t RemoveHeater(const size_t index, const bool allFollowing);

		void SetColdExtrudeTemperature(float temp);
		float GetColdExtrudeTemperature();
		void SetColdRetractTemperature(float temp);
		float GetColdRetractTemperature();
	} // namespace Heat
} // namespace OM
