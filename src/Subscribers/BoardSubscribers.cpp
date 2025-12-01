/*
 * BoardSubscribers.cpp
 *
 *  Created on: 2025-12-01
 *      Author: Andy Everitt
 */

#include "BoardSubscribers.h"
#include "Debug.h"

#include "ObjectModel/Directories.h"
#include "UI/Core/Model.h"

bool BoardSubscribers::uniqueId(Comm::JsonDecoder* decoder, const char* data, const size_t indices[])
{
	UNUSED(decoder);

	/* We only care about the uniqueId for the main board */
	if (indices[0] != 0)
	{
		return false;
	}

	OM::SetPrinterUniqueId(data);
	return true;
}
