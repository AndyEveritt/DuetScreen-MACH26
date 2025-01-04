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

/*
 * These functions are run when the end of an array has been received from the OM
 * The function needs to take in an array containing the indices of the OM key
 */
// static UI::Observer<UI::ui_array_end_update_cb>
// 	FanObserversArrayEnd[] = {
// 		OBSERVER_ARRAY_END("fans^",
// 						   [](OBSERVER_ARRAY_END_ARGS)
// 						   {
// 							   if (OM::RemoveFan(indices[0], true))
// 							   {
// 								   //    UI::GetUIControl<ZKListView>(ID_MAIN_PrintFanList)->refreshListView();
// 							   }
// 						   }),
// };
