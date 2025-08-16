/*
 * DefaultTheme.cpp
 *
 *  Created on: 2025-06-02
 *      Author: Andy Everitt
 */

#include "DefaultTheme.h"
#include "Debug.h"
#include <algorithm>

namespace UI::Themes
{
#define TRANSITION_TIME 80
#define BORDER_WIDTH 2
#define OUTLINE_WIDTH 3
#define SHADOW_WIDTH 2
#define SHADOW_OFFSET 3

#define RADIUS_DEFAULT 5

#define PAD_DEF 5
#define PAD_SMALL 2
#define PAD_TINY 1

	static constexpr float s_minColorChroma = 0.1f;

	ThemeColors createThemeColors(uint16_t primaryHue,
								  uint16_t secondaryHue,
								  float chroma,
								  bool darkMode,
								  std::function<void(ThemeColors& colors)> customizer)
	{
		ThemeColors colors;
		primaryHue = std::clamp<uint16_t>(primaryHue, 0u, 360u);
		secondaryHue = std::clamp<uint16_t>(secondaryHue, 0, 360u);
		chroma = std::clamp<float>(chroma, 0.0f, 0.2f);

		const float bgChroma = chroma / 2;
		const float colorChroma = std::max(s_minColorChroma, chroma);
		static float mutedDiff = 0.2f;

		colors.bg_dark = Color(darkMode ? 0.1 : 0.92, bgChroma, primaryHue);
		colors.bg = Color(darkMode ? 0.15 : 0.96, bgChroma, primaryHue);
		colors.bg_light = Color(darkMode ? 0.2 : 1.0, bgChroma, primaryHue);

		colors.primary = Color(darkMode ? 0.56 : 0.8, colorChroma, primaryHue);
		colors.primary_muted = Color(colors.primary.getL() - mutedDiff, colorChroma, primaryHue);
		colors.secondary = Color(darkMode ? 0.56 : 0.8, colorChroma, secondaryHue);
		colors.secondary_muted = Color(colors.secondary.getL() - mutedDiff, colorChroma, secondaryHue);

		colors.text = Color(darkMode ? 0.96 : 0.15, chroma, primaryHue);
		colors.text_muted = Color(darkMode ? 0.76 : 0.40, chroma, primaryHue);
		colors.text_header = colors.text;

		colors.border = Color(darkMode ? 0.40 : 0.6, chroma, primaryHue);
		colors.border_muted = Color(darkMode ? 0.30 : 0.7, chroma, primaryHue);
		colors.highlight = Color(darkMode ? 0.70 : 1.0, chroma, primaryHue);
		colors.shadow = Color(darkMode ? 0.2 : 0.4, bgChroma, primaryHue);

		if (customizer)
		{
			customizer(colors);
		}
		return colors;
	}

	void DefaultTheme::updateColors(const ThemeColors& colors)
	{
		m_colors = colors;
		init();
	}

	void DefaultTheme::onInit()
	{
		/* Backgrounds */

		lv_style_set_bg_color(getLvglStyles().bg_dark, m_colors.bg_dark);
		lv_style_set_bg_opa(getLvglStyles().bg_dark, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg_dark, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg, m_colors.bg);
		lv_style_set_bg_opa(getLvglStyles().bg, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg_light, m_colors.bg_light);
		lv_style_set_bg_opa(getLvglStyles().bg_light, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg_light, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg_color_primary, m_colors.primary);
		lv_style_set_bg_opa(getLvglStyles().bg_color_primary, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg_color_primary, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg_color_primary_muted, m_colors.primary_muted);
		lv_style_set_bg_opa(getLvglStyles().bg_color_primary_muted, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg_color_primary_muted, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg_color_secondary, m_colors.secondary);
		lv_style_set_bg_opa(getLvglStyles().bg_color_secondary, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg_color_secondary, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg_color_secondary_muted, m_colors.secondary_muted);
		lv_style_set_bg_opa(getLvglStyles().bg_color_secondary_muted, LV_OPA_COVER);
		lv_style_set_bg_grad_dir(getLvglStyles().bg_color_secondary_muted, LV_GRAD_DIR_NONE);

		lv_style_set_bg_color(getLvglStyles().bg_color_header, m_colors.bg_light);
		lv_style_set_bg_opa(getLvglStyles().bg_color_header, LV_OPA_COVER);
		lv_style_set_bg_grad_color(getLvglStyles().bg_color_header, m_colors.bg);

		lv_style_set_bg_color(getLvglStyles().bg_color_list_item, m_colors.bg);
		lv_style_set_bg_opa(getLvglStyles().bg_color_list_item, LV_OPA_COVER);
		lv_style_set_bg_grad_color(getLvglStyles().bg_color_list_item, m_colors.bg_light);

		/* Text */

		lv_style_set_text_color(getLvglStyles().text, m_colors.text);
		lv_style_set_text_font(getLvglStyles().text, m_fontNormal);

		lv_style_set_text_color(getLvglStyles().text_muted, m_colors.text_muted);
		lv_style_set_text_font(getLvglStyles().text_muted, m_fontNormal);

		lv_style_set_text_color(getLvglStyles().text_header, m_colors.text_header);
		lv_style_set_text_font(getLvglStyles().text_header, m_fontLarge);

		lv_style_set_text_line_space(getLvglStyles().line_space_large, LV_DPX_CALC(lv_display_get_dpi(NULL), 20));

		lv_style_set_text_align(getLvglStyles().text_align_center, LV_TEXT_ALIGN_CENTER);

		/* Borders */

		lv_style_set_border_color(getLvglStyles().border, m_colors.border);
		lv_style_set_border_width(getLvglStyles().border, BORDER_WIDTH);
		lv_style_set_border_opa(getLvglStyles().border, LV_OPA_COVER);
		lv_style_set_border_side(getLvglStyles().border, LV_BORDER_SIDE_FULL);

		lv_style_set_border_color(getLvglStyles().border_muted, m_colors.border_muted);
		lv_style_set_border_width(getLvglStyles().border_muted, BORDER_WIDTH);
		lv_style_set_border_opa(getLvglStyles().border_muted, LV_OPA_COVER);
		lv_style_set_border_side(getLvglStyles().border_muted, LV_BORDER_SIDE_FULL);

		lv_style_set_border_color(getLvglStyles().border_highlight, m_colors.highlight);
		lv_style_set_border_width(getLvglStyles().border_highlight, BORDER_WIDTH);
		lv_style_set_border_opa(getLvglStyles().border_highlight, LV_OPA_COVER);
		lv_style_set_border_side(getLvglStyles().border_highlight, LV_BORDER_SIDE_FULL);

		lv_style_set_border_color(getLvglStyles().border_color_primary, m_colors.primary);
		lv_style_set_border_width(getLvglStyles().border_color_primary, BORDER_WIDTH);
		lv_style_set_border_opa(getLvglStyles().border_color_primary, LV_OPA_COVER);
		lv_style_set_border_side(getLvglStyles().border_color_primary, LV_BORDER_SIDE_FULL);

		lv_style_set_border_color(getLvglStyles().border_color_secondary, m_colors.secondary);
		lv_style_set_border_width(getLvglStyles().border_color_secondary, BORDER_WIDTH);
		lv_style_set_border_opa(getLvglStyles().border_color_secondary, LV_OPA_COVER);
		lv_style_set_border_side(getLvglStyles().border_color_secondary, LV_BORDER_SIDE_FULL);

		/* Outlines */

		lv_style_set_outline_color(getLvglStyles().outline_primary, m_colors.primary);
		lv_style_set_outline_width(getLvglStyles().outline_primary, OUTLINE_WIDTH);
		lv_style_set_outline_pad(getLvglStyles().outline_primary, OUTLINE_WIDTH);
		lv_style_set_outline_opa(getLvglStyles().outline_primary, LV_OPA_50);

		lv_style_set_outline_color(getLvglStyles().outline_secondary, m_colors.secondary);
		lv_style_set_outline_width(getLvglStyles().outline_secondary, OUTLINE_WIDTH);
		lv_style_set_outline_opa(getLvglStyles().outline_secondary, LV_OPA_50);

		/* Scrollbar */

		lv_style_set_bg_color(getLvglStyles().scrollbar, m_colors.bg_light);
		lv_style_set_border_color(getLvglStyles().scrollbar, m_colors.border);
		lv_style_set_border_width(getLvglStyles().scrollbar, 1);
		lv_style_set_radius(getLvglStyles().scrollbar, LV_RADIUS_CIRCLE);
		lv_style_set_pad_all(getLvglStyles().scrollbar, LV_DPX_CALC(lv_display_get_dpi(NULL), 7));
		lv_style_set_width(getLvglStyles().scrollbar, LV_DPX_CALC(lv_display_get_dpi(NULL), 5));
		lv_style_set_bg_opa(getLvglStyles().scrollbar, LV_OPA_40);

		lv_style_set_bg_opa(getLvglStyles().scrollbar_scrolled, LV_OPA_COVER);

		/* Base */

		lv_style_set_bg_opa(getLvglStyles().base, LV_OPA_TRANSP);
		lv_style_set_text_color(getLvglStyles().base, m_colors.text);
		lv_style_set_text_font(getLvglStyles().base, m_fontNormal);

		/* Screen */

		lv_style_set_bg_color(getLvglStyles().screen, m_colors.bg_dark);
		lv_style_set_bg_opa(getLvglStyles().screen, LV_OPA_COVER);

		/* Card */

		lv_style_set_radius(getLvglStyles().card, RADIUS_DEFAULT);
		lv_style_set_pad_all(getLvglStyles().card, PAD_DEF);
		lv_style_set_pad_row(getLvglStyles().card, PAD_SMALL);
		lv_style_set_pad_column(getLvglStyles().card, PAD_SMALL);

		lv_style_set_bg_opa(getLvglStyles().card, LV_OPA_COVER);
		lv_style_set_bg_color(getLvglStyles().card, m_colors.bg_light);
		lv_style_set_bg_grad_color(getLvglStyles().card, m_colors.bg);
		lv_style_set_bg_grad_dir(getLvglStyles().card, LV_GRAD_DIR_VER);

		lv_style_set_border_color(getLvglStyles().card, m_colors.border);
		lv_style_set_border_width(getLvglStyles().card, BORDER_WIDTH);
		lv_style_set_border_opa(getLvglStyles().card, LV_OPA_COVER);
		lv_style_set_border_side(getLvglStyles().card, LV_BORDER_SIDE_FULL);

		/* Button */

		lv_style_set_radius(getLvglStyles().btn, RADIUS_DEFAULT);

		lv_style_set_bg_opa(getLvglStyles().btn, LV_OPA_COVER);
		lv_style_set_bg_color(getLvglStyles().btn, m_colors.primary);
		lv_style_set_bg_grad_color(getLvglStyles().btn, m_colors.primary_muted);
		lv_style_set_bg_grad_dir(getLvglStyles().btn, LV_GRAD_DIR_VER);

		lv_style_set_bg_grad_dir(getLvglStyles().btn_checked, LV_GRAD_DIR_NONE);
		lv_style_merge(getLvglStyles().btn_checked, getLvglStyles().border_highlight);

		lv_style_set_bg_opa(getLvglStyles().actionBtn, LV_OPA_COVER);
		lv_style_set_bg_color(getLvglStyles().actionBtn, m_colors.secondary);
		lv_style_set_bg_grad_color(getLvglStyles().actionBtn, m_colors.secondary_muted);
		lv_style_set_bg_grad_dir(getLvglStyles().actionBtn, LV_GRAD_DIR_VER);

		/* Icons */
		lv_style_set_recolor(getLvglStyles().icon_recolor, m_colors.text);
		lv_style_set_recolor_opa(getLvglStyles().icon_recolor, LV_OPA_COVER);

		/* Modifiers */

		lv_style_set_recolor(getLvglStyles().pressed, lv_color_black());
		lv_style_set_recolor_opa(getLvglStyles().pressed, 35);

		lv_style_set_recolor(getLvglStyles().disabled, m_colors.bg);
		lv_style_set_recolor_opa(getLvglStyles().disabled, LV_OPA_50);

		lv_style_set_clip_corner(getLvglStyles().clip_corner, true);
		lv_style_set_border_post(getLvglStyles().clip_corner, true);

		/* Padding */

		lv_style_set_pad_all(getLvglStyles().pad_normal, PAD_DEF);
		lv_style_set_pad_row(getLvglStyles().pad_normal, PAD_DEF);
		lv_style_set_pad_column(getLvglStyles().pad_normal, PAD_DEF);

		lv_style_set_pad_all(getLvglStyles().pad_small, PAD_SMALL);
		lv_style_set_pad_gap(getLvglStyles().pad_small, PAD_SMALL);

		lv_style_set_pad_row(getLvglStyles().pad_gap, LV_DPX_CALC(lv_display_get_dpi(NULL), 10));
		lv_style_set_pad_column(getLvglStyles().pad_gap, LV_DPX_CALC(lv_display_get_dpi(NULL), 10));

		lv_style_set_pad_all(getLvglStyles().pad_base, PAD_DEF);
		lv_style_set_pad_row(getLvglStyles().pad_base, PAD_DEF);
		lv_style_set_pad_column(getLvglStyles().pad_base, PAD_DEF);

		lv_style_set_pad_all(getLvglStyles().pad_zero, 0);
		lv_style_set_pad_row(getLvglStyles().pad_zero, 0);
		lv_style_set_pad_column(getLvglStyles().pad_zero, 0);

		lv_style_set_pad_all(getLvglStyles().pad_tiny, PAD_TINY);
		lv_style_set_pad_row(getLvglStyles().pad_tiny, PAD_TINY);
		lv_style_set_pad_column(getLvglStyles().pad_tiny, PAD_TINY);

		lv_style_set_radius(getLvglStyles().circle, LV_RADIUS_CIRCLE);

		lv_style_set_radius(getLvglStyles().no_radius, 0);

		lv_style_set_border_width(getLvglStyles().no_border, 0);

		lv_style_set_rotary_sensitivity(getLvglStyles().rotary_scroll, lv_display_get_dpi(NULL) / 4 * 256);

		lv_style_set_transform_width(getLvglStyles().grow, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));
		lv_style_set_transform_height(getLvglStyles().grow, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));

		lv_style_set_bg_color(getLvglStyles().knob, m_colors.primary);
		lv_style_set_bg_opa(getLvglStyles().knob, LV_OPA_COVER);
		lv_style_set_pad_all(getLvglStyles().knob, LV_DPX_CALC(lv_display_get_dpi(NULL), 6));
		lv_style_set_radius(getLvglStyles().knob, LV_RADIUS_CIRCLE);

		lv_style_set_anim_duration(getLvglStyles().anim, 200);

		lv_style_set_anim_duration(getLvglStyles().anim_fast, 120);

		lv_style_set_border_color(getLvglStyles().input, m_colors.border);
		lv_style_set_border_width(getLvglStyles().input, 1);
		lv_style_set_pad_all(getLvglStyles().input, 20);
		lv_style_set_radius(getLvglStyles().input, 2);
		lv_style_set_pad_ver(getLvglStyles().input, 0);
		lv_style_set_text_align(getLvglStyles().input, LV_TEXT_ALIGN_CENTER);

		// lv_style_set_transform_width(getLvglStyles().draggable, LV_DPX(4));
		// lv_style_set_transform_height(getLvglStyles().draggable, LV_DPX(4));
		// lv_style_set_transform_skew_x(getLvglStyles().draggable, 2);
		// lv_style_set_transform_skew_y(getLvglStyles().draggable, -2);

		static const lv_style_prop_t draggable_props[] = {LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT};
		lv_style_transition_dsc_init(&m_draggableTransition, draggable_props, lv_anim_path_ease_in_out, 200, 100, NULL);
		lv_style_set_transition(getLvglStyles().draggable, &m_draggableTransition);

		static const lv_style_prop_t dragging_props[] = {
			LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_BORDER_WIDTH};
		lv_style_transition_dsc_init(
			&m_draggingTransition, dragging_props, lv_anim_path_ease_in_out, TRANSITION_TIME, 0, NULL);
		lv_style_set_transition(getLvglStyles().dragging, &m_draggingTransition);

		lv_style_set_transform_width(getLvglStyles().dragging, 20);
		lv_style_set_transform_height(getLvglStyles().dragging, 20);
		lv_style_set_border_color(getLvglStyles().dragging, lv_color_white());
		lv_style_set_border_opa(getLvglStyles().dragging, LV_OPA_30);
		lv_style_set_border_width(getLvglStyles().dragging, 20);

		static const lv_style_prop_t drag_complete_props[] = {LV_STYLE_OUTLINE_WIDTH};
		lv_style_transition_dsc_init(
			&m_dragCompleteTransition, drag_complete_props, lv_anim_path_ease_in_out, TRANSITION_TIME, 0, NULL);
		lv_style_set_transition(getLvglStyles().dragging, &m_dragCompleteTransition);

		lv_style_set_outline_color(getLvglStyles().drag_complete, lv_color_white());
		lv_style_set_outline_width(getLvglStyles().drag_complete, 3);

#if LV_USE_ARC
		lv_style_set_arc_color(getLvglStyles().arc_indic, m_colors.highlight);
		lv_style_set_arc_width(getLvglStyles().arc_indic, LV_DPX_CALC(lv_display_get_dpi(NULL), 15));
		lv_style_set_arc_rounded(getLvglStyles().arc_indic, true);

		lv_style_set_arc_color(getLvglStyles().arc_indic_primary, m_colors.primary);
#endif

#if LV_USE_BAR
		lv_style_set_radius(getLvglStyles().bar, LV_RADIUS_CIRCLE);
		lv_style_set_radius(getLvglStyles().bar_indic, LV_RADIUS_CIRCLE);
#endif

#if LV_USE_BUTTONMATRIX
		lv_style_set_bg_color(getLvglStyles().btnm_btn, m_colors.primary);
#endif

#if LV_USE_DROPDOWN
		lv_style_set_max_height(getLvglStyles().dropdown_list, LV_DPI_DEF * 2);
#endif
#if LV_USE_CHECKBOX
		lv_style_set_pad_all(getLvglStyles().cb_marker, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));
		lv_style_set_border_width(getLvglStyles().cb_marker, BORDER_WIDTH);
		lv_style_set_border_color(getLvglStyles().cb_marker, m_colors.primary);
		lv_style_set_bg_color(getLvglStyles().cb_marker, m_colors.bg);
		lv_style_set_bg_opa(getLvglStyles().cb_marker, LV_OPA_COVER);
		lv_style_set_radius(getLvglStyles().cb_marker, RADIUS_DEFAULT / 2);
		lv_style_set_text_font(getLvglStyles().cb_marker, m_fontSmall);
		lv_style_set_text_color(getLvglStyles().cb_marker, m_colors.text);

		lv_style_set_bg_image_src(getLvglStyles().cb_marker_checked, LV_SYMBOL_OK);
#endif

#if LV_USE_SWITCH
		lv_style_set_bg_color(getLvglStyles().bg_switch, m_colors.bg_light);
		lv_style_set_bg_opa(getLvglStyles().bg_switch, LV_OPA_COVER);
		lv_style_set_radius(getLvglStyles().bg_switch, LV_RADIUS_CIRCLE);
		lv_style_set_pad_all(getLvglStyles().switch_knob, -LV_DPX_CALC(lv_display_get_dpi(NULL), 4));
		lv_style_set_bg_color(getLvglStyles().switch_knob, m_colors.text);
		lv_style_set_radius(getLvglStyles().switch_knob, LV_RADIUS_CIRCLE);
#endif

#if LV_USE_LINE
		lv_style_set_line_width(getLvglStyles().line, 1);
		lv_style_set_line_color(getLvglStyles().line, m_colors.highlight);
#endif

#if LV_USE_CHART
		lv_style_set_border_width(getLvglStyles().chart_bg, BORDER_WIDTH);
		lv_style_set_border_color(getLvglStyles().chart_bg, m_colors.highlight);
		lv_style_set_border_opa(getLvglStyles().chart_bg, LV_OPA_COVER);
		lv_style_set_border_post(getLvglStyles().chart_bg, false);
		lv_style_set_pad_column(getLvglStyles().chart_bg, LV_DPX_CALC(lv_display_get_dpi(NULL), 10));
		lv_style_set_line_color(getLvglStyles().chart_bg, m_colors.highlight);

		lv_style_set_line_width(getLvglStyles().chart_series, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));
		lv_style_set_radius(getLvglStyles().chart_series, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));

		int32_t chart_size = LV_DPX_CALC(lv_display_get_dpi(NULL), 8);
		lv_style_set_size(getLvglStyles().chart_series, chart_size, chart_size);
		lv_style_set_pad_column(getLvglStyles().chart_series, LV_DPX_CALC(lv_display_get_dpi(NULL), 2));

		lv_style_set_radius(getLvglStyles().chart_indic, LV_RADIUS_CIRCLE);
		lv_style_set_size(getLvglStyles().chart_indic, chart_size, chart_size);
		lv_style_set_bg_color(getLvglStyles().chart_indic, m_colors.primary);
		lv_style_set_bg_opa(getLvglStyles().chart_indic, LV_OPA_COVER);
#endif

#if LV_USE_MENU
		lv_style_set_pad_all(getLvglStyles().menu_bg, 0);
		lv_style_set_pad_gap(getLvglStyles().menu_bg, 0);
		lv_style_set_radius(getLvglStyles().menu_bg, 0);
		lv_style_set_clip_corner(getLvglStyles().menu_bg, true);
		lv_style_set_border_side(getLvglStyles().menu_bg, LV_BORDER_SIDE_NONE);

		lv_style_set_radius(getLvglStyles().menu_section, RADIUS_DEFAULT);
		lv_style_set_clip_corner(getLvglStyles().menu_section, true);
		lv_style_set_bg_opa(getLvglStyles().menu_section, LV_OPA_COVER);
		lv_style_set_bg_color(getLvglStyles().menu_section, m_colors.bg);
		lv_style_set_text_color(getLvglStyles().menu_section, m_colors.text);

		lv_style_set_pad_hor(getLvglStyles().menu_cont, PAD_SMALL);
		lv_style_set_pad_ver(getLvglStyles().menu_cont, PAD_SMALL);
		lv_style_set_pad_gap(getLvglStyles().menu_cont, PAD_SMALL);
		lv_style_set_border_width(getLvglStyles().menu_cont, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_opa(getLvglStyles().menu_cont, LV_OPA_10);
		lv_style_set_border_color(getLvglStyles().menu_cont, m_colors.text);
		lv_style_set_border_side(getLvglStyles().menu_cont, LV_BORDER_SIDE_NONE);

		lv_style_set_pad_all(getLvglStyles().menu_sidebar_cont, 0);
		lv_style_set_pad_gap(getLvglStyles().menu_sidebar_cont, 0);
		lv_style_set_border_width(getLvglStyles().menu_sidebar_cont, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_opa(getLvglStyles().menu_sidebar_cont, LV_OPA_10);
		lv_style_set_border_color(getLvglStyles().menu_sidebar_cont, m_colors.text);
		lv_style_set_border_side(getLvglStyles().menu_sidebar_cont, LV_BORDER_SIDE_RIGHT);

		lv_style_set_pad_all(getLvglStyles().menu_main_cont, 0);
		lv_style_set_pad_gap(getLvglStyles().menu_main_cont, 0);

		lv_style_set_pad_hor(getLvglStyles().menu_header_cont, PAD_SMALL);
		lv_style_set_pad_ver(getLvglStyles().menu_header_cont, PAD_TINY);
		lv_style_set_pad_gap(getLvglStyles().menu_header_cont, PAD_SMALL);

		lv_style_set_pad_hor(getLvglStyles().menu_header_btn, PAD_TINY);
		lv_style_set_pad_ver(getLvglStyles().menu_header_btn, PAD_TINY);
		lv_style_set_shadow_opa(getLvglStyles().menu_header_btn, LV_OPA_TRANSP);
		lv_style_set_bg_opa(getLvglStyles().menu_header_btn, LV_OPA_TRANSP);
		lv_style_set_text_color(getLvglStyles().menu_header_btn, m_colors.text);

		lv_style_set_pad_hor(getLvglStyles().menu_page, 0);
		lv_style_set_pad_gap(getLvglStyles().menu_page, 0);

		lv_style_set_bg_opa(getLvglStyles().menu_pressed, LV_OPA_20);
		lv_style_set_bg_color(getLvglStyles().menu_pressed, m_colors.bg_light);

		lv_style_set_bg_opa(getLvglStyles().menu_separator, LV_OPA_TRANSP);
		lv_style_set_pad_ver(getLvglStyles().menu_separator, PAD_TINY);
#endif

#if LV_USE_TABLE
		lv_style_set_radius(getLvglStyles().table, 0);
		lv_style_set_bg_color(getLvglStyles().table_cell, m_colors.bg);
		lv_style_set_bg_opa(getLvglStyles().table_cell, LV_OPA_COVER);
		lv_style_set_text_color(getLvglStyles().table_cell, m_colors.text);
		lv_style_set_border_width(getLvglStyles().table_cell, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_color(getLvglStyles().table_cell, m_colors.highlight);
		lv_style_set_border_side(getLvglStyles().table_cell,
								 static_cast<lv_border_side_t>(LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM));
#endif

#if LV_USE_TEXTAREA
		lv_style_set_border_color(getLvglStyles().ta_cursor, m_colors.text);
		lv_style_set_border_width(getLvglStyles().ta_cursor, LV_DPX_CALC(lv_display_get_dpi(NULL), 2));
		lv_style_set_pad_left(getLvglStyles().ta_cursor, -LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_side(getLvglStyles().ta_cursor, LV_BORDER_SIDE_LEFT);
		lv_style_set_anim_duration(getLvglStyles().ta_cursor, 400);

		lv_style_set_text_color(getLvglStyles().ta_placeholder, m_colors.text_muted);
#endif

#if LV_USE_CALENDAR
		lv_style_set_pad_all(getLvglStyles().calendar_btnm_bg, PAD_SMALL);
		lv_style_set_pad_gap(getLvglStyles().calendar_btnm_bg, PAD_SMALL / 2);

		lv_style_set_border_width(getLvglStyles().calendar_btnm_day, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_color(getLvglStyles().calendar_btnm_day, m_colors.highlight);
		lv_style_set_bg_color(getLvglStyles().calendar_btnm_day, m_colors.bg);
		lv_style_set_bg_opa(getLvglStyles().calendar_btnm_day, LV_OPA_20);

		lv_style_set_pad_hor(getLvglStyles().calendar_header, PAD_SMALL);
		lv_style_set_pad_top(getLvglStyles().calendar_header, PAD_SMALL);
		lv_style_set_pad_bottom(getLvglStyles().calendar_header, PAD_TINY);
		lv_style_set_pad_gap(getLvglStyles().calendar_header, PAD_SMALL);
#endif

#if LV_USE_CANVAS
		lv_style_set_bg_color(getLvglStyles().canvas, m_colors.bg);
		lv_style_set_bg_opa(getLvglStyles().canvas, LV_OPA_COVER);
		lv_style_set_border_width(getLvglStyles().canvas, BORDER_WIDTH);
		lv_style_set_border_color(getLvglStyles().canvas, m_colors.border);
		lv_style_set_border_side(getLvglStyles().canvas, LV_BORDER_SIDE_FULL);
#endif

#if LV_USE_MSGBOX
		// lv_style_set_bg_color(getLvglStyles().msgbox_backdrop_bg, m_colors.bg);
		// lv_style_set_bg_opa(getLvglStyles().msgbox_backdrop_bg, LV_OPA_50);
#endif
#if LV_USE_KEYBOARD
		lv_style_set_bg_color(getLvglStyles().keyboard_button_bg, m_colors.bg_light);
		lv_style_set_shadow_width(getLvglStyles().keyboard_button_bg, 0);
		lv_style_set_radius(getLvglStyles().keyboard_button_bg, RADIUS_DEFAULT);
		lv_style_set_bg_color(getLvglStyles().keyboard_button_checked_bg, m_colors.highlight);
		lv_style_set_bg_opa(getLvglStyles().keyboard_button_checked_bg, LV_OPA_COVER);
		lv_style_set_text_color(getLvglStyles().keyboard_button_checked_bg, m_colors.text);
#endif

#if LV_USE_TABVIEW
		lv_style_set_border_color(getLvglStyles().tab_btn, m_colors.primary);
		lv_style_set_border_width(getLvglStyles().tab_btn, BORDER_WIDTH * 2);
		lv_style_set_border_side(getLvglStyles().tab_btn, LV_BORDER_SIDE_BOTTOM);
		lv_style_set_pad_top(getLvglStyles().tab_btn, BORDER_WIDTH * 2);

		lv_style_set_outline_pad(getLvglStyles().tab_bg_focus, -BORDER_WIDTH);
#endif

#if LV_USE_LIST
		lv_style_set_pad_hor(getLvglStyles().list_bg, PAD_DEF);
		lv_style_set_pad_ver(getLvglStyles().list_bg, 0);
		lv_style_set_pad_gap(getLvglStyles().list_bg, 0);
		lv_style_set_clip_corner(getLvglStyles().list_bg, true);

		lv_style_set_border_width(getLvglStyles().list_btn, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_color(getLvglStyles().list_btn, m_colors.highlight);
		lv_style_set_border_side(getLvglStyles().list_btn, LV_BORDER_SIDE_BOTTOM);
		lv_style_set_pad_all(getLvglStyles().list_btn, PAD_SMALL);
		lv_style_set_pad_column(getLvglStyles().list_btn, PAD_SMALL);

		lv_style_set_transform_width(getLvglStyles().list_item_grow, PAD_DEF);
#endif

#if LV_USE_LED
		lv_style_set_bg_opa(getLvglStyles().led, LV_OPA_COVER);
		lv_style_set_bg_color(getLvglStyles().led, lv_color_white());
		lv_style_set_bg_grad_color(getLvglStyles().led, m_colors.bg_dark);
		lv_style_set_radius(getLvglStyles().led, LV_RADIUS_CIRCLE);
		lv_style_set_shadow_width(getLvglStyles().led, LV_DPX_CALC(lv_display_get_dpi(NULL), 15));
		lv_style_set_shadow_color(getLvglStyles().led, lv_color_white());
		lv_style_set_shadow_spread(getLvglStyles().led, LV_DPX_CALC(lv_display_get_dpi(NULL), 5));
#endif

#if LV_USE_SCALE
		lv_style_set_line_color(getLvglStyles().scale, m_colors.text);
		lv_style_set_line_width(getLvglStyles().scale, LV_DPX(2));
		lv_style_set_arc_color(getLvglStyles().scale, m_colors.text);
		lv_style_set_arc_width(getLvglStyles().scale, LV_DPX(2));
		lv_style_set_length(getLvglStyles().scale, LV_DPX(6));
#endif

#if LV_USE_SLIDER
		lv_style_set_radius(getLvglStyles().slider, LV_RADIUS_CIRCLE);
		lv_style_set_radius(getLvglStyles().slider_indic, LV_RADIUS_CIRCLE);
#endif

		lv_style_set_bg_color(getComponentStyles().estop, lv_palette_main(LV_PALETTE_RED));
		lv_style_set_bg_grad_color(getComponentStyles().estop, lv_palette_darken(LV_PALETTE_RED, 4));
		lv_style_set_text_color(getComponentStyles().estop, lv_color_white());
		lv_style_set_radius(getComponentStyles().estop, LV_RADIUS_CIRCLE);

		lv_style_copy(getComponentStyles().folder, getLvglStyles().btn);

		lv_style_merge(getComponentStyles().file, getLvglStyles().btn);
		lv_style_merge(getComponentStyles().file, getLvglStyles().bg_light);
		lv_style_merge(getComponentStyles().file, getLvglStyles().border);

		lv_style_set_bg_color(getComponentStyles().unhomed, lv_color_hex(0xfb9514));

		lv_style_set_bg_opa(getComponentStyles().temperature_bar, LV_OPA_COVER);
		lv_style_set_bg_color(getComponentStyles().temperature_bar, lv_palette_main(LV_PALETTE_BLUE));
		lv_style_set_bg_grad_color(getComponentStyles().temperature_bar, lv_palette_main(LV_PALETTE_RED));
		lv_style_set_bg_grad_dir(getComponentStyles().temperature_bar, LV_GRAD_DIR_HOR);

		lv_style_set_bg_color(getComponentStyles().temperature_bar_indic, m_colors.border);
		lv_style_set_bg_opa(getComponentStyles().temperature_bar_indic, LV_OPA_COVER);

		lv_style_merge(getComponentStyles().tool_selected, getLvglStyles().border_color_primary);
		lv_style_set_border_width(getComponentStyles().tool_selected, 3);
		lv_style_set_shadow_color(getComponentStyles().tool_selected, lv_color_darken(m_colors.primary, 10));
		lv_style_set_shadow_opa(getComponentStyles().tool_selected, LV_OPA_50);
		lv_style_set_shadow_width(getComponentStyles().tool_selected, 0);
		lv_style_set_shadow_offset_x(getComponentStyles().tool_selected, -SHADOW_OFFSET);
		lv_style_set_shadow_offset_y(getComponentStyles().tool_selected, SHADOW_OFFSET);
	}
} // namespace UI::Themes
