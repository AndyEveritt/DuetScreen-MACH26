/*
 * Container.h
 *
 *  Created on: 2025-06-11
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvObj.h"

namespace UI
{
    class Container : public LvObj
    {
      public:
        Container(const std::string& name, lv_obj_t* parent);

	  private:
    };
} // namespace UI
