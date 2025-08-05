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
	static const lv_color_t s_primaryColor = lv_palette_main(LV_PALETTE_BLUE);
	static const lv_color_t s_secondaryColor = lv_palette_darken(LV_PALETTE_RED, 3);
	static const lv_color_t s_cardColor = lv_color_hex(0x282b30);
	static const lv_color_t s_textColor = lv_palette_lighten(LV_PALETTE_GREY, 5);
	static const lv_color_t s_highlightColor = lv_color_hex(0x2f3237);
	static const lv_font_t* s_font = LV_FONT_DEFAULT;

	static DefaultTheme s_darkTheme("theme_dark",
									s_primaryColor,
									s_secondaryColor,
									s_cardColor,
									s_textColor,
									s_highlightColor,
									s_font,
									true,
									[](Theme* theme) {
										lv_style_set_bg_color(theme->components.estop, lv_palette_main(LV_PALETTE_RED));
									});
} // namespace UI::Themes
