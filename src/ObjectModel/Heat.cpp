/*
 * Heat.cpp
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#include "Heat.h"
#include "Configuration.h"
#include "ListHelpers.h"
#include "ObjectModel/Utils.h"
#include "utils/utils.h"
#include <Duet3D/General/Vector.h>

#include "Debug.h"

typedef Vector<std::shared_ptr<OM::Heat::Heater>, MAX_HEATERS> HeaterList;
static HeaterList heaters;

namespace OM
{
	namespace Heat
	{
		void Heater::Reset()
		{
			index = 0;
			activeTemp = 0;
			standbyTemp = 0;
			current = 0;
			avgPwm = 0;
			status = HeaterStatus::off;
			sensor = nullptr;
		}

		const char* Heater::GetName() const
		{
			if (sensor == nullptr)
			{
				return "";
			}

			return sensor->name.c_str();
		}

		const char* Heater::GetHeaterStatusStr() const
		{
			const HeaterStatusMapEntry key = {"unknown", status};
			const HeaterStatusMapEntry* statusFromMap =
				(HeaterStatusMapEntry*)bsearch(&key,
											   heaterStatusMap,
											   ARRAY_SIZE(heaterStatusMap),
											   sizeof(HeaterStatusMapEntry),
											   compareValue<HeaterStatusMapEntry>);

			return (statusFromMap != nullptr) ? statusFromMap->key : "unknown";
		}

		void Heater::UpdateTarget(const int32_t temp, const bool active)
		{
			if (active)
			{
				activeTemp = temp;
				return;
			}
			standbyTemp = temp;
		}

		std::shared_ptr<Heater> GetHeater(const size_t index)
		{
			return GetOrCreate<HeaterList, Heater>(heaters, index, false);
		}

		std::shared_ptr<Heater> GetOrCreateHeater(const size_t index)
		{
			return GetOrCreate<HeaterList, Heater>(heaters, index, true);
		}

		bool IterateHeatersWhile(function_ref<bool(std::shared_ptr<Heater>, size_t)> func, const size_t startAt)
		{
			return heaters.IterateWhile(func, startAt);
		}

		bool UpdateHeaterTarget(const size_t heaterIndex, const int32_t temp, const bool active)
		{
			auto heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdateTarget(temp, active);
			return true;
		}

		bool UpdateHeaterTemp(const size_t heaterIndex, const float temp)
		{
			auto heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdateTemp(temp);
			return true;
		}

		bool UpdateHeaterPwm(const size_t heaterIndex, const float pwm)
		{
			auto heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdatePwm(pwm);
			return true;
		}

		bool UpdateHeaterMin(const size_t heaterIndex, const float min)
		{
			auto heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdateMin(min);
			return true;
		}

		bool UpdateHeaterMax(const size_t heaterIndex, const float max)
		{
			auto heater = GetOrCreateHeater(heaterIndex);

			// If we do not handle this heater back off
			if (heater == nullptr)
			{
				return false;
			}

			heater->UpdateMax(max);
			return true;
		}

		bool UpdateHeaterStatus(const size_t heaterIndex, const char* statusStr)
		{
			const HeaterStatusMapEntry key = {statusStr, HeaterStatus::off};
			const HeaterStatusMapEntry* statusFromMap =
				(HeaterStatusMapEntry*)bsearch(&key,
											   heaterStatusMap,
											   ARRAY_SIZE(heaterStatusMap),
											   sizeof(HeaterStatusMapEntry),
											   compareKey<HeaterStatusMapEntry>);
			HeaterStatus status = (statusFromMap != nullptr) ? statusFromMap->val : HeaterStatus::off;
			return UpdateHeaterStatus(heaterIndex, status);
		}

		bool UpdateHeaterStatus(const size_t heaterIndex, HeaterStatus status)
		{
			auto heater = GetOrCreateHeater(heaterIndex);

			if (heater == nullptr)
				return false;

			heater->status = status;
			LOG_DBG("Heater {:d} state={:d} {:s}", heaterIndex, (int32_t)heater->status, heater->GetHeaterStatusStr());
			return true;
		}

		bool UpdateHeaterSensor(const size_t heaterIndex, const size_t sensorIndex)
		{
			auto heater = GetOrCreateHeater(heaterIndex);
			if (heater == nullptr)
			{
				return false;
			}

			heater->sensor = OM::GetOrCreateAnalogSensor(sensorIndex);
			return true;
		}

		size_t RemoveHeater(const size_t index, const bool allFollowing)
		{
			LOG_DBG("Removing heater {:d} (allFollowing={:s})", index, allFollowing ? "true" : "false");
			return Remove<HeaterList, Heater>(heaters, index, allFollowing);
		}
	} // namespace Heat
} // namespace OM
