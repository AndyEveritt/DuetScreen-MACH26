/*
 * UnitSystem.h
 *
 *  Created on: 2025-10-14
 *      Author: Andy Everitt
 */

#pragma once
#include <string>

namespace Units
{
	enum class UnitSystem
	{
		Metric,
		Imperial,
	};

	UnitSystem getCurrentDuetUnitSystem();
	UnitSystem getCurrentDisplayedUnitSystem();

	const std::string& getDisplayedDistanceUnit();
	const std::string& getDisplayedSpeedUnit();

	float convertDistanceToCurrentDisplayedUnit(float distance_mm);
	float convertSpeedToCurrentDisplayedUnit(float speed_mm_s);

	float convertDisplayedDistanceToDuetUnits(float distance);
	float convertDisplayedSpeedToDuetUnits(float speed);

	float inchesToMm(float inches);
	float mmToInches(float mm);
} // namespace Units
