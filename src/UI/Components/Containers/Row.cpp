/*
 * Row.cpp
 *
 *  Created on: 2025-08-21
 *      Author: Andy Everitt
 */

#include "Row.h"
#include "Debug.h"

namespace UI
{
	Row::Row(const std::string& name, lv_obj_t* parent)
		: LvContainer(name, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_ROW);
	}
} // namespace UI
