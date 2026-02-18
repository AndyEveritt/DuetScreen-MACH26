/*
 * SoftTheme.cpp
 *
 *  Created on: 2026-02-12
 *      Author: Andy Everitt
 *
 *  Light mode, pill-shaped buttons, generous padding, subtle shadows,
 *  pastel tones. Emphasises rounded organic shapes and breathing room.
 */

#include "CustomTheme.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static ColorCtx s_colors = {.primaryHue = 250, .secondaryHue = 180, .chroma = 0.08f, .darkMode = false};
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 18, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(600)},
		.normal = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
		.emphasis = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(600)},
		.subdued = {.size = 12, .style = LV_FREETYPE_FONT_STYLE_ITALIC | LV_FREETYPE_FONT_STYLE_WEIGHT(300)},
	};

	static CustomTheme s_theme("soft",
							   s_colors,
							   s_fontConfigs,
							   [](Theme* theme)
							   {
								   auto t = static_cast<DefaultTheme*>(theme);
								   auto& lvgl = t->getLvglStyles();
								   auto& components = t->getComponentStyles();
								   [[maybe_unused]] auto& colors = t->getColors();

								   /* Very rounded – pill shapes */
								   lv_style_set_radius(lvgl.card, 12);
								   lv_style_set_radius(lvgl.btn, LV_RADIUS_CIRCLE);
								   lv_style_set_radius(lvgl.knob, LV_RADIUS_CIRCLE);

								   /* Soft shadow instead of borders */
								   lv_style_set_border_width(lvgl.card, 0);
								   lv_style_set_shadow_width(lvgl.card, 12);
								   lv_style_set_shadow_spread(lvgl.card, 2);
								   lv_style_set_shadow_opa(lvgl.card, LV_OPA_10);
								   lv_style_set_shadow_color(lvgl.card, lv_color_black());

								   /* Generous padding for breathing room */
								   lv_style_set_pad_all(lvgl.card, 12);
								   lv_style_set_pad_row(lvgl.card, 6);
								   lv_style_set_pad_column(lvgl.card, 6);

								   /* Buttons: pill-shaped with subtle shadow, extra horizontal
									  padding to prevent text overflow from rounded corners */
								   lv_style_set_pad_hor(lvgl.btn, 14);
								   lv_style_set_shadow_width(lvgl.btn, 6);
								   lv_style_set_shadow_opa(lvgl.btn, LV_OPA_10);
								   lv_style_set_shadow_color(lvgl.btn, lv_color_black());
								   lv_style_set_shadow_offset_y(lvgl.btn, 2);
								   lv_style_set_border_width(lvgl.btn, 0);
								   lv_style_set_clip_corner(lvgl.btn, true);

								   /* Pressed feedback: shrink instead of recolor */
								   lv_style_set_recolor_opa(lvgl.pressed, 15);

								   /* Round scrollbar */
								   lv_style_set_radius(lvgl.scrollbar, LV_RADIUS_CIRCLE);
								   lv_style_set_width(lvgl.scrollbar, 6);
								   lv_style_set_bg_opa(lvgl.scrollbar, LV_OPA_20);

#if LV_USE_BUTTONMATRIX
								   lv_style_set_radius(lvgl.btnm_btn, LV_RADIUS_CIRCLE);
								   lv_style_set_pad_hor(lvgl.btnm_btn, 10);
								   lv_style_set_clip_corner(lvgl.btnm_btn, true);
								   lv_style_set_border_width(lvgl.btnm_btn, 0);
								   lv_style_set_shadow_width(lvgl.btnm_btn, 4);
								   lv_style_set_shadow_opa(lvgl.btnm_btn, LV_OPA_10);
								   lv_style_set_shadow_color(lvgl.btnm_btn, lv_color_black());
#endif

#if LV_USE_BAR
								   lv_style_set_radius(lvgl.bar, LV_RADIUS_CIRCLE);
								   lv_style_set_radius(lvgl.bar_indic, LV_RADIUS_CIRCLE);
#endif

#if LV_USE_SLIDER
								   lv_style_set_radius(lvgl.slider, LV_RADIUS_CIRCLE);
								   lv_style_set_radius(lvgl.slider_indic, LV_RADIUS_CIRCLE);
								   lv_style_set_radius(lvgl.slider_knob, LV_RADIUS_CIRCLE);
								   lv_style_set_height(lvgl.slider, 14);
								   lv_style_set_border_width(lvgl.slider_knob, 0);
								   lv_style_set_shadow_width(lvgl.slider_knob, 6);
								   lv_style_set_shadow_opa(lvgl.slider_knob, LV_OPA_20);
								   lv_style_set_shadow_color(lvgl.slider_knob, lv_color_black());
#endif

#if LV_USE_CHECKBOX
								   lv_style_set_radius(lvgl.cb_marker, LV_RADIUS_CIRCLE);
								   lv_style_set_border_width(lvgl.cb_marker, 1);
#endif

#if LV_USE_SWITCH
								   lv_style_set_radius(lvgl.bg_switch, LV_RADIUS_CIRCLE);
								   lv_style_set_radius(lvgl.switch_knob, LV_RADIUS_CIRCLE);
#endif

#if LV_USE_KEYBOARD
								   lv_style_set_radius(lvgl.keyboard_button, 10);
								   lv_style_set_border_width(lvgl.keyboard_button, 0);
								   lv_style_set_shadow_width(lvgl.keyboard_button, 4);
								   lv_style_set_shadow_opa(lvgl.keyboard_button, LV_OPA_10);
								   lv_style_set_shadow_color(lvgl.keyboard_button, lv_color_black());
#endif

#if LV_USE_TABVIEW
								   lv_style_set_border_width(lvgl.tab_btn, 0);
								   lv_style_set_pad_top(lvgl.tab_btn, 4);
#endif

#if LV_USE_LIST
								   lv_style_set_border_width(lvgl.list_btn, 0);
								   lv_style_set_pad_all(lvgl.list_btn, 6);
#endif

								   lv_style_set_radius(components.bar_label_bg, LV_RADIUS_CIRCLE);

								   lv_style_copy(components.folder, lvgl.btn);

								   lv_style_merge(components.file, lvgl.btn);
								   lv_style_merge(components.file, lvgl.bg_light);
								   lv_style_set_pad_hor(components.file, 16);
								   lv_style_set_pad_ver(components.file, 14);
							   });
} // namespace UI::Themes
