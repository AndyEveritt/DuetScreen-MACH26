/*
 * Container.cpp
 *
 *  Created on: 2025-06-11
 *      Author: Andy Everitt
 */

#include "Container.h"
#include "Debug.h"

namespace UI
{
	Container::Container(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_obj_create, name, parent)
	{
	}
} // namespace UI
