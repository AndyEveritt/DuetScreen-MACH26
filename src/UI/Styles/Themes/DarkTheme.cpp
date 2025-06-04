/*
 * DarkTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "DefaultTheme.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static DefaultTheme s_darkTheme("theme_dark",
									lv_palette_main(LV_PALETTE_BLUE),
									lv_palette_main(LV_PALETTE_RED),
									lv_color_hex(0x282b30),
									lv_palette_lighten(LV_PALETTE_GREY, 5),
									lv_color_hex(0x2f3237),
									LV_FONT_DEFAULT,
									true,
									[](Theme* theme) {
										lv_style_set_bg_color(theme->components.estop, lv_palette_main(LV_PALETTE_RED));
									});
} // namespace UI::Themes
