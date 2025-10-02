/*
 * {{file_name_with_ext}}
 *
 *  Created on: {{date}}
 *      Author: {{author}}
 */

#pragma once

#include "UI/Components/LVGL/LvContainer.h"

namespace UI
{
    class {{file_name}} : public LvContainer
    {
      public:
        {{file_name}}(const std::string& name, LvObj& parent);
      private:
    };
} // namespace UI
