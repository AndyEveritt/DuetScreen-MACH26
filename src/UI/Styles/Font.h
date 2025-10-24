/*
 * Font.h
 *
 *  Created on: 2025-10-23
 *      Author: Andy Everitt
 */

#pragma once

#include "lvgl/lvgl.h"
#include <string_view>
#include <vector>

namespace UI::Font
{
	void init();
	lv_font_t* createFont(const std::string& name, uint32_t size);

} // namespace UI::Font
