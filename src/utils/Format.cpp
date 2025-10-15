/*
 * Format.cpp
 *
 *  Created on: 2025-10-14
 *      Author: Andy Everitt
 */

#include "Format.h"
#include "Debug.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"

namespace Format
{
	const std::string& getDistanceUnit()
	{
		auto metric = StorageHelper::getData<bool>(ID_UNITS_METRIC, true);
		return metric ? _("units.mm") : _("units.inches");
	}

	const std::string& getSpeedUnit()
	{
		auto metric = StorageHelper::getData<bool>(ID_UNITS_METRIC, true);
		return metric ? _("units.mm_speed") : _("units.inches_speed");
	}
} // namespace Format
