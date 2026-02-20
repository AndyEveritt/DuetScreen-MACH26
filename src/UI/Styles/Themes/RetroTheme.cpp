/*
 * RetroTheme.cpp
 *
 *  Created on: 2026-02-12
 *      Author: Andy Everitt
 *
 *  Warm, nostalgic feel with squared-off shapes, heavy outlines, chunky
 *  padding, and warm earth tones. Inspired by classic control-panel UIs.
 */

#include "CustomTheme.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static ColorCtx s_colors = {.primaryHue = 65, .secondaryHue = 115, .chroma = 0.03f, .darkMode = false};
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 20, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(800)},
		.normal = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(500)},
		.emphasis = {.size = 15, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.subdued = {.size = 12, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
	};

	static CustomTheme s_theme("retro",
							   s_colors,
							   s_fontConfigs,
							   [](Theme* theme)
							   {
								   auto t = static_cast<DefaultTheme*>(theme);
								   auto& lvgl = t->getLvglStyles();
								   auto& components = t->getComponentStyles();
								   auto& colors = t->getColors();

								   /* Squared off – no rounding */
								   lv_style_set_radius(lvgl.card, 0);
								   lv_style_set_radius(lvgl.btn, 0);
								   lv_style_set_radius(lvgl.knob, 0);

								   /* Heavy double-width borders */
								   lv_style_set_border_width(lvgl.card, 3);
								   lv_style_set_border_color(lvgl.card, colors.text);
								   lv_style_set_border_opa(lvgl.card, LV_OPA_COVER);

								   /* Extra chunky padding */
								   lv_style_set_pad_all(lvgl.card, 10);
								   lv_style_set_pad_row(lvgl.card, 6);
								   lv_style_set_pad_column(lvgl.card, 6);

								   /* Hard offset shadow for depth illusion */
								   lv_style_set_shadow_width(lvgl.card, 0);
								   lv_style_set_shadow_offset_x(lvgl.card, 4);
								   lv_style_set_shadow_offset_y(lvgl.card, 4);
								   lv_style_set_shadow_color(lvgl.card, colors.border);
								   lv_style_set_shadow_opa(lvgl.card, LV_OPA_COVER);

								   /* Buttons: raised with hard shadow */
								   lv_style_set_border_width(lvgl.btn, 3);
								   lv_style_set_border_color(lvgl.btn, colors.text);
								   lv_style_set_shadow_width(lvgl.btn, 0);
								   lv_style_set_shadow_offset_x(lvgl.btn, 3);
								   lv_style_set_shadow_offset_y(lvgl.btn, 3);
								   lv_style_set_shadow_color(lvgl.btn, colors.border);
								   lv_style_set_shadow_opa(lvgl.btn, LV_OPA_COVER);

								   lv_style_set_border_color(lvgl.btn_checked, colors.secondary);
								   lv_style_set_border_width(lvgl.btn_checked, 3);

								   /* Pressed: collapse shadow for "pushed" effect */
								   lv_style_set_recolor(lvgl.pressed, lv_color_black());
								   lv_style_set_recolor_opa(lvgl.pressed, 30);

								   /* Wider scrollbar for chunky look */
								   lv_style_set_radius(lvgl.scrollbar, 0);
								   lv_style_set_width(lvgl.scrollbar, 8);
								   lv_style_set_bg_opa(lvgl.scrollbar, LV_OPA_60);
								   lv_style_set_border_width(lvgl.scrollbar, 1);
								   lv_style_set_border_color(lvgl.scrollbar, colors.text);

#if LV_USE_BUTTONMATRIX
								   lv_style_set_radius(lvgl.btnm_btn, 0);
								   lv_style_set_border_width(lvgl.btnm_btn, 2);
								   lv_style_set_border_color(lvgl.btnm_btn, colors.text);
								   lv_style_set_shadow_width(lvgl.btnm_btn, 0);
								   lv_style_set_shadow_offset_x(lvgl.btnm_btn, 2);
								   lv_style_set_shadow_offset_y(lvgl.btnm_btn, 2);
								   lv_style_set_shadow_color(lvgl.btnm_btn, colors.border);
								   lv_style_set_shadow_opa(lvgl.btnm_btn, LV_OPA_COVER);
#endif

#if LV_USE_BAR
								   lv_style_set_radius(lvgl.bar, 0);
								   lv_style_set_radius(lvgl.bar_indic, 0);
#endif

#if LV_USE_SLIDER
								   lv_style_set_radius(lvgl.slider, 0);
								   lv_style_set_radius(lvgl.slider_indic, 0);
								   lv_style_set_radius(lvgl.slider_knob, 0);
								   lv_style_set_height(lvgl.slider, 24);
								   lv_style_set_border_width(lvgl.slider_knob, 3);
								   lv_style_set_border_color(lvgl.slider_knob, colors.text);
#endif

#if LV_USE_CHECKBOX
								   lv_style_set_radius(lvgl.cb_marker, 0);
								   lv_style_set_border_width(lvgl.cb_marker, 3);
								   lv_style_set_border_color(lvgl.cb_marker, colors.text);
#endif

#if LV_USE_SWITCH
								   lv_style_set_radius(lvgl.bg_switch, 0);
								   lv_style_set_radius(lvgl.switch_knob, 0);
#endif

#if LV_USE_KEYBOARD
								   lv_style_set_radius(lvgl.keyboard_button, 0);
								   lv_style_set_border_width(lvgl.keyboard_button, 2);
								   lv_style_set_border_color(lvgl.keyboard_button, colors.text);
#endif

#if LV_USE_TABVIEW
								   lv_style_set_border_width(lvgl.tab_btn, 3);
								   lv_style_set_border_color(lvgl.tab_btn, colors.text);
								   lv_style_set_border_side(lvgl.tab_btn, LV_BORDER_SIDE_BOTTOM);
#endif

#if LV_USE_LIST
								   lv_style_set_border_width(lvgl.list_btn, 2);
								   lv_style_set_border_color(lvgl.list_btn, colors.border);
								   lv_style_set_pad_all(lvgl.list_btn, 6);
#endif

								   lv_style_set_radius(components.bar_label_bg, 0);
								   lv_style_set_radius(components.estop, 0);

								   lv_style_copy(components.folder, lvgl.btn);
								   lv_style_merge(components.file, lvgl.btn);
								   lv_style_merge(components.file, lvgl.bg_light);
							   });
} // namespace UI::Themes
