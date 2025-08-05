/*
 * AppDrawer.h
 *
 *  Created on: 2025-08-05
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/LvContainer.h"
#include "App.h"
#include "UI/Components/List/List.h"

namespace UI
{
    class AppDrawer : public LvContainer
    {
      public:
        AppDrawer(const std::string& name, lv_obj_t* parent);
        void init();

      private:
        List<App> m_apps; // List of apps in the drawer
    };
} // namespace UI
