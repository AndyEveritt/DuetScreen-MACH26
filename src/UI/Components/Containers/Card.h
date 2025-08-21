/*
 * Card.h
 *
 *  Created on: 2025-08-07
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvContainer.h"

namespace UI
{
    class Card : public LvContainer
    {
      public:
		Card(const std::string& name, lv_obj_t* parent);
		Card(const std::string& name, lv_obj_t* parent, layout_t layout);

	  private:
        void init();
    };
} // namespace UI
