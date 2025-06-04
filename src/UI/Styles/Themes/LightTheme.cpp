/*
 * LightTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "DefaultTheme.h"
#include "UI/Styles/Styles.h"

#define COLOR_SCR lv_palette_lighten(LV_PALETTE_GREY, 4)
#define COLOR_CARD lv_color_white()
#define COLOR_TEXT lv_palette_darken(LV_PALETTE_GREY, 4)
#define COLOR_BORDER lv_palette_lighten(LV_PALETTE_ORANGE, 2)

namespace UI::Themes
{
	static DefaultTheme s_lightTheme("theme_light",
									 lv_palette_main(LV_PALETTE_GREEN),
									 lv_palette_main(LV_PALETTE_YELLOW),
									 lv_color_white(),
									 COLOR_TEXT,
									 COLOR_BORDER,
									 LV_FONT_DEFAULT,
									 false,
									 [](Theme* theme)
									 {
										 lv_style_set_bg_color(theme->lvgl.screen, COLOR_SCR);
										 lv_style_set_text_color(theme->lvgl.screen, COLOR_TEXT);
										 lv_style_set_text_color(theme->lvgl.card, COLOR_TEXT);
										 lv_style_set_bg_color(theme->lvgl.card, COLOR_CARD);
										 lv_style_set_border_color(theme->lvgl.card, COLOR_BORDER);

										 //  lv_style_set_bg_color(theme->lvgl.btn, lv_palette_main(LV_PALETTE_CYAN));

										 lv_style_set_bg_color(theme->lvgl.bg_color_list_item,
															   lv_palette_main(LV_PALETTE_PINK));

										 lv_style_set_bg_color(theme->components.estop,
															   lv_palette_main(LV_PALETTE_RED));
										 // lv_style_set_text_color(m_estop, COLOR_TEXT);
									 });
} // namespace UI::Themes
