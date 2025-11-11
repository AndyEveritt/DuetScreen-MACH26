/*
 * FlatTheme.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "DefaultTheme.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static const uint16_t s_primaryHue = 245;
	static const uint16_t s_secondaryHue = 50;
	static const float s_chroma = 0.02f;
	static const lv_font_t* s_font = LV_FONT_DEFAULT;
	static bool s_darkMode = true;
	static std::string_view s_iconFolder = "material";

	static ThemeColors s_colors = createThemeColors(s_primaryHue, s_secondaryHue, s_chroma, s_darkMode);

	static DefaultTheme s_flatTheme("flat",
									s_colors,
									s_font,
									s_iconFolder,
									[](Theme* theme)
									{
										auto t = static_cast<DefaultTheme*>(theme);

										auto& lvgl = t->getLvglStyles();
										auto& components = t->getComponentStyles();
										auto& colors = t->getColors();

										lv_style_set_border_width(lvgl.card, 0);

										lv_style_set_bg_color(lvgl.btn, colors.bg_light);
										lv_style_set_bg_grad_dir(lvgl.btn, LV_GRAD_DIR_NONE);
										lv_style_set_text_color(lvgl.btn, colors.primary);

										lv_style_set_bg_color(lvgl.actionBtn, colors.secondary);
										lv_style_set_bg_grad_dir(lvgl.actionBtn, LV_GRAD_DIR_NONE);
										lv_style_set_text_color(lvgl.actionBtn, colors.text);

										lv_style_set_bg_color(lvgl.card, colors.bg);
										lv_style_set_bg_grad_dir(lvgl.card, LV_GRAD_DIR_NONE);

										lv_style_set_recolor(lvgl.icon_recolor, colors.primary);

										lv_style_copy(components.folder, lvgl.btn);
									});
} // namespace UI::Themes
