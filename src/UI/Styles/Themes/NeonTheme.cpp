/*
 * NeonTheme.cpp
 *
 *  Created on: 2026-02-12
 *      Author: Andy Everitt
 *
 *  Cyberpunk-inspired dark theme with neon glow effects using coloured
 *  shadows, thin borders, tight padding, and vibrant accent colours.
 */

#include "CustomTheme.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 18, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.normal = {.size = 13, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
		.emphasis = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(600)},
		.subdued = {.size = 11, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(300)},
	};

	static CustomTheme s_theme("neon",
							   s_fontConfigs,
							   [](Theme* theme)
							   {
								   auto t = static_cast<DefaultTheme*>(theme);
								   auto& lvgl = t->getLvglStyles();
								   auto& components = t->getComponentStyles();
								   auto& colors = t->getColors();

								   /* Moderate rounding */
								   lv_style_set_radius(lvgl.card, 3);
								   lv_style_set_radius(lvgl.btn, 3);

								   /* Card: thin glowing border, coloured shadow */
								   lv_style_set_border_width(lvgl.card, 1);
								   lv_style_set_border_color(lvgl.card, colors.primary);
								   lv_style_set_border_opa(lvgl.card, LV_OPA_40);
								   lv_style_set_shadow_width(lvgl.card, 16);
								   lv_style_set_shadow_spread(lvgl.card, 1);
								   lv_style_set_shadow_color(lvgl.card, colors.primary);
								   lv_style_set_shadow_opa(lvgl.card, LV_OPA_20);

								   /* Tight padding for a dense information display */
								   lv_style_set_pad_all(lvgl.card, 4);
								   lv_style_set_pad_row(lvgl.card, 3);
								   lv_style_set_pad_column(lvgl.card, 3);

								   /* Buttons: neon glow border */
								   lv_style_set_bg_color(lvgl.btn, colors.bg_light);
								   lv_style_set_border_width(lvgl.btn, 1);
								   lv_style_set_border_color(lvgl.btn, colors.primary);
								   lv_style_set_border_opa(lvgl.btn, LV_OPA_70);
								   lv_style_set_text_color(lvgl.btn, colors.primary);
								   lv_style_set_shadow_width(lvgl.btn, 10);
								   lv_style_set_shadow_color(lvgl.btn, colors.primary);
								   lv_style_set_shadow_opa(lvgl.btn, LV_OPA_20);
								   lv_style_set_shadow_spread(lvgl.btn, 0);

								   /* Action buttons glow in secondary colour */
								   lv_style_set_bg_color(lvgl.actionBtn, colors.bg_light);
								   lv_style_set_border_width(lvgl.actionBtn, 1);
								   lv_style_set_border_color(lvgl.actionBtn, colors.secondary);
								   lv_style_set_text_color(lvgl.actionBtn, colors.secondary);

								   /* Checked button: filled with glow */
								   lv_style_set_shadow_width(lvgl.btn_checked, 14);
								   lv_style_set_shadow_color(lvgl.btn_checked, colors.primary);
								   lv_style_set_shadow_opa(lvgl.btn_checked, LV_OPA_30);

								   /* Pressed: subtle brightening */
								   lv_style_set_recolor(lvgl.pressed, lv_color_white());
								   lv_style_set_recolor_opa(lvgl.pressed, 25);

								   /* Knob: glows */
								   lv_style_set_shadow_width(lvgl.knob, 8);
								   lv_style_set_shadow_color(lvgl.knob, colors.primary);
								   lv_style_set_shadow_opa(lvgl.knob, LV_OPA_40);

								   /* Scrollbar: thin neon line */
								   lv_style_set_radius(lvgl.scrollbar, 1);
								   lv_style_set_width(lvgl.scrollbar, 3);
								   lv_style_set_bg_color(lvgl.scrollbar, colors.primary);
								   lv_style_set_bg_opa(lvgl.scrollbar, LV_OPA_50);

#if LV_USE_BUTTONMATRIX
								   lv_style_set_radius(lvgl.btnm_btn, 3);
								   lv_style_set_border_width(lvgl.btnm_btn, 1);
								   lv_style_set_border_color(lvgl.btnm_btn, colors.primary);
								   lv_style_set_border_opa(lvgl.btnm_btn, LV_OPA_30);
								   lv_style_set_shadow_width(lvgl.btnm_btn, 6);
								   lv_style_set_shadow_color(lvgl.btnm_btn, colors.primary);
								   lv_style_set_shadow_opa(lvgl.btnm_btn, LV_OPA_10);
#endif

#if LV_USE_BAR
								   lv_style_set_radius(lvgl.bar, 2);
								   lv_style_set_radius(lvgl.bar_indic, 2);
#endif

#if LV_USE_SLIDER
								   lv_style_set_radius(lvgl.slider, 2);
								   lv_style_set_radius(lvgl.slider_indic, 2);
								   lv_style_set_radius(lvgl.slider_knob, LV_RADIUS_CIRCLE);
								   lv_style_set_height(lvgl.slider, 16);
								   lv_style_set_shadow_width(lvgl.slider_knob, 10);
								   lv_style_set_shadow_color(lvgl.slider_knob, colors.primary);
								   lv_style_set_shadow_opa(lvgl.slider_knob, LV_OPA_40);
#endif

#if LV_USE_SWITCH
								   lv_style_set_radius(lvgl.bg_switch, 3);
								   lv_style_set_radius(lvgl.switch_knob, 2);
#endif

#if LV_USE_CHECKBOX
								   lv_style_set_radius(lvgl.cb_marker, 2);
								   lv_style_set_border_color(lvgl.cb_marker, colors.primary);
#endif

#if LV_USE_KEYBOARD
								   lv_style_set_radius(lvgl.keyboard_button, 3);
								   lv_style_set_border_width(lvgl.keyboard_button, 1);
								   lv_style_set_border_color(lvgl.keyboard_button, colors.primary);
								   lv_style_set_border_opa(lvgl.keyboard_button, LV_OPA_30);
#endif

#if LV_USE_TABVIEW
								   lv_style_set_border_color(lvgl.tab_btn, colors.primary);
								   lv_style_set_border_width(lvgl.tab_btn, 2);
#endif

								   /* Component adjustments */
								   lv_style_set_radius(components.bar_label_bg, 3);
								   lv_style_set_radius(components.estop, 3);

								   lv_style_copy(components.folder, lvgl.btn);
								   lv_style_merge(components.file, lvgl.btn);
								   lv_style_merge(components.file, lvgl.bg_light);
							   });
} // namespace UI::Themes
