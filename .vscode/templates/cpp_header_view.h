/*
 * {{file_name_with_ext}}
 *
 *  Created on: {{date}}
 *      Author: {{author}}
 */

#pragma once

#include "UI/Core/View.h"
#include "{{file_name}}Presenter.h"

namespace UI
{
    class {{file_name}} : public View<{{file_name}}Presenter>
    {
      public:
        {{file_name}}(const std::string& name, LvObj& parent);
      private:
    };
} // namespace UI
