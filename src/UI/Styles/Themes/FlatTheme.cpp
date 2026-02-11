/*
 * FlatTheme.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "CustomTheme.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 18, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.normal = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
		.emphasis = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.subdued = {.size = 12, .style = LV_FREETYPE_FONT_STYLE_ITALIC | LV_FREETYPE_FONT_STYLE_WEIGHT(200)},
	};

	static CustomTheme s_flatTheme("flat",
								   s_fontConfigs,
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
