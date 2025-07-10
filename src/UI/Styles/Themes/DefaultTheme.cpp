/*
 * DefaultTheme.cpp
 *
 *  Created on: 2025-06-02
 *      Author: Andy Everitt
 */

#include "DefaultTheme.h"
#include "Debug.h"

namespace UI::Themes
{
#define TRANSITION_TIME 80
#define BORDER_WIDTH LV_DPX_CALC(lv_display_get_dpi(NULL), 2)
#define OUTLINE_WIDTH LV_DPX_CALC(lv_display_get_dpi(NULL), 3)

#define RADIUS_DEFAULT LV_DPX_CALC(lv_display_get_dpi(NULL), 8)

#define PAD_DEF 5
#define PAD_SMALL 2
#define PAD_TINY 1

#define GREY lv_palette_main(LV_PALETTE_GREY)
#define LIGHT_GREY lv_palette_lighten(LV_PALETTE_GREY, 2)
#define DARK_GREY lv_palette_darken(LV_PALETTE_GREY, 2)

	void DefaultTheme::onInit()
	{
		lv_style_set_bg_color(lvgl.scrollbar, GREY);
		lv_style_set_radius(lvgl.scrollbar, LV_RADIUS_CIRCLE);
		lv_style_set_pad_all(lvgl.scrollbar, LV_DPX_CALC(lv_display_get_dpi(NULL), 7));
		lv_style_set_width(lvgl.scrollbar, LV_DPX_CALC(lv_display_get_dpi(NULL), 5));
		lv_style_set_bg_opa(lvgl.scrollbar, LV_OPA_40);

		lv_style_set_bg_opa(lvgl.scrollbar_scrolled, LV_OPA_COVER);

		lv_style_set_bg_opa(lvgl.screen, LV_OPA_COVER);
		// lv_style_set_bg_color(lvgl.screen, theme->color_scr);
		lv_style_set_text_color(lvgl.screen, m_textColor);
		lv_style_set_text_font(lvgl.screen, m_fontNormal);
		lv_style_set_pad_row(lvgl.screen, PAD_SMALL);
		lv_style_set_pad_column(lvgl.screen, PAD_SMALL);
		lv_style_set_rotary_sensitivity(lvgl.screen, lv_display_get_dpi(NULL) / 4 * 256);

		lv_style_set_radius(lvgl.card, RADIUS_DEFAULT);
		lv_style_set_bg_opa(lvgl.card, LV_OPA_COVER);
		lv_style_set_bg_color(lvgl.card, m_cardColor);
		lv_style_set_border_color(lvgl.card, m_darkMode ? DARK_GREY : LIGHT_GREY);
		lv_style_set_border_width(lvgl.card, BORDER_WIDTH);
		lv_style_set_border_post(lvgl.card, true);
		lv_style_set_text_color(lvgl.card, m_textColor);
		lv_style_set_pad_all(lvgl.card, PAD_DEF);
		lv_style_set_pad_row(lvgl.card, PAD_SMALL);
		lv_style_set_pad_column(lvgl.card, PAD_SMALL);
		lv_style_set_line_color(lvgl.card, GREY);
		lv_style_set_line_width(lvgl.card, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));

		lv_style_set_outline_color(lvgl.outline_primary, m_primaryColor);
		lv_style_set_outline_width(lvgl.outline_primary, OUTLINE_WIDTH);
		lv_style_set_outline_pad(lvgl.outline_primary, OUTLINE_WIDTH);
		lv_style_set_outline_opa(lvgl.outline_primary, LV_OPA_50);

		lv_style_set_outline_color(lvgl.outline_secondary, m_secondaryColor);
		lv_style_set_outline_width(lvgl.outline_secondary, OUTLINE_WIDTH);
		lv_style_set_outline_opa(lvgl.outline_secondary, LV_OPA_50);

		lv_style_set_radius(lvgl.btn, RADIUS_DEFAULT);
		if (!m_darkMode)
		{
			lv_style_set_shadow_color(lvgl.btn, GREY);
			lv_style_set_shadow_width(lvgl.btn, LV_DPX(3));
			lv_style_set_shadow_opa(lvgl.btn, LV_OPA_50);
			lv_style_set_shadow_offset_y(lvgl.btn, LV_DPX_CALC(lv_display_get_dpi(NULL), LV_DPX(4)));
		}
		lv_style_set_text_color(lvgl.btn, m_textColor);
		lv_style_set_pad_hor(lvgl.btn, PAD_DEF);
		lv_style_set_pad_ver(lvgl.btn, PAD_DEF);
		lv_style_set_pad_column(lvgl.btn, PAD_DEF);
		lv_style_set_pad_row(lvgl.btn, PAD_DEF);

		lv_style_set_recolor(lvgl.pressed, lv_color_black());
		lv_style_set_recolor_opa(lvgl.pressed, 35);

		lv_style_set_recolor(lvgl.disabled, m_darkMode ? DARK_GREY : LIGHT_GREY);
		lv_style_set_recolor_opa(lvgl.disabled, LV_OPA_50);

		lv_style_set_clip_corner(lvgl.clip_corner, true);
		lv_style_set_border_post(lvgl.clip_corner, true);

		lv_style_set_pad_all(lvgl.pad_normal, PAD_DEF);
		lv_style_set_pad_row(lvgl.pad_normal, PAD_DEF);
		lv_style_set_pad_column(lvgl.pad_normal, PAD_DEF);

		lv_style_set_pad_all(lvgl.pad_small, PAD_SMALL);
		lv_style_set_pad_gap(lvgl.pad_small, PAD_SMALL);

		lv_style_set_pad_row(lvgl.pad_gap, LV_DPX_CALC(lv_display_get_dpi(NULL), 10));
		lv_style_set_pad_column(lvgl.pad_gap, LV_DPX_CALC(lv_display_get_dpi(NULL), 10));

		lv_style_set_text_line_space(lvgl.line_space_large, LV_DPX_CALC(lv_display_get_dpi(NULL), 20));

		lv_style_set_text_align(lvgl.text_align_center, LV_TEXT_ALIGN_CENTER);

		lv_style_set_pad_all(lvgl.pad_base, PAD_DEF);
		lv_style_set_pad_row(lvgl.pad_base, PAD_DEF);
		lv_style_set_pad_column(lvgl.pad_base, PAD_DEF);

		lv_style_set_pad_all(lvgl.pad_zero, 0);
		lv_style_set_pad_row(lvgl.pad_zero, 0);
		lv_style_set_pad_column(lvgl.pad_zero, 0);

		lv_style_set_pad_all(lvgl.pad_tiny, PAD_TINY);
		lv_style_set_pad_row(lvgl.pad_tiny, PAD_TINY);
		lv_style_set_pad_column(lvgl.pad_tiny, PAD_TINY);

		lv_style_set_bg_color(lvgl.bg_color_primary, m_primaryColor);
		lv_style_set_bg_opa(lvgl.bg_color_primary, LV_OPA_COVER);

		lv_style_set_bg_color(lvgl.bg_color_primary_muted, m_primaryColor);
		lv_style_set_text_color(lvgl.bg_color_primary_muted, m_primaryColor);
		lv_style_set_bg_opa(lvgl.bg_color_primary_muted, LV_OPA_20);

		lv_style_set_bg_color(lvgl.bg_color_secondary, m_secondaryColor);
		lv_style_set_bg_opa(lvgl.bg_color_secondary, LV_OPA_COVER);

		lv_style_set_bg_color(lvgl.bg_color_secondary_muted, m_secondaryColor);
		lv_style_set_text_color(lvgl.bg_color_secondary_muted, m_secondaryColor);
		lv_style_set_bg_opa(lvgl.bg_color_secondary_muted, LV_OPA_20);

		lv_style_set_bg_color(lvgl.bg_color_header, m_highlightColor);
		lv_style_set_bg_opa(lvgl.bg_color_header, LV_OPA_COVER);
		lv_style_set_text_color(lvgl.bg_color_header, m_textColor);

		lv_style_set_bg_color(lvgl.bg_color_list_item, m_cardColor);
		lv_style_set_bg_opa(lvgl.bg_color_list_item, LV_OPA_COVER);
		lv_style_set_text_color(lvgl.bg_color_list_item, m_textColor);

		lv_style_set_radius(lvgl.circle, LV_RADIUS_CIRCLE);

		lv_style_set_radius(lvgl.no_radius, 0);

		lv_style_set_border_width(lvgl.no_border, 0);

		lv_style_set_rotary_sensitivity(lvgl.rotary_scroll, lv_display_get_dpi(NULL) / 4 * 256);

		lv_style_set_transform_width(lvgl.grow, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));
		lv_style_set_transform_height(lvgl.grow, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));

		lv_style_set_bg_color(lvgl.knob, m_primaryColor);
		lv_style_set_bg_opa(lvgl.knob, LV_OPA_COVER);
		lv_style_set_pad_all(lvgl.knob, LV_DPX_CALC(lv_display_get_dpi(NULL), 6));
		lv_style_set_radius(lvgl.knob, LV_RADIUS_CIRCLE);

		lv_style_set_anim_duration(lvgl.anim, 200);

		lv_style_set_anim_duration(lvgl.anim_fast, 120);

		lv_style_set_border_color(lvgl.actionBtn, m_secondaryColor);
		lv_style_set_border_width(lvgl.actionBtn, BORDER_WIDTH);
		lv_style_set_border_opa(lvgl.actionBtn, LV_OPA_COVER);
		lv_style_set_border_side(lvgl.actionBtn, LV_BORDER_SIDE_FULL);

		lv_style_set_border_color(lvgl.input, m_darkMode ? LIGHT_GREY : DARK_GREY);
		lv_style_set_border_width(lvgl.input, 2);
		lv_style_set_radius(lvgl.input, RADIUS_DEFAULT);
		lv_style_set_pad_ver(lvgl.input, 0);
		lv_style_set_text_align(lvgl.input, LV_TEXT_ALIGN_CENTER);

		// lv_style_set_transform_width(lvgl.draggable, LV_DPX(4));
		// lv_style_set_transform_height(lvgl.draggable, LV_DPX(4));
		// lv_style_set_transform_skew_x(lvgl.draggable, 2);
		// lv_style_set_transform_skew_y(lvgl.draggable, -2);

		static const lv_style_prop_t draggable_props[] = {LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT};
		lv_style_transition_dsc_init(&m_draggableTransition, draggable_props, lv_anim_path_ease_in_out, 200, 100, NULL);
		lv_style_set_transition(lvgl.draggable, &m_draggableTransition);

		static const lv_style_prop_t dragging_props[] = {
			LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_BORDER_WIDTH};
		lv_style_transition_dsc_init(
			&m_draggingTransition, dragging_props, lv_anim_path_ease_in_out, TRANSITION_TIME, 0, NULL);
		lv_style_set_transition(lvgl.dragging, &m_draggingTransition);

		lv_style_set_transform_width(lvgl.dragging, 20);
		lv_style_set_transform_height(lvgl.dragging, 20);
		lv_style_set_border_color(lvgl.dragging, lv_color_white());
		lv_style_set_border_opa(lvgl.dragging, LV_OPA_30);
		lv_style_set_border_width(lvgl.dragging, 20);

		static const lv_style_prop_t drag_complete_props[] = {LV_STYLE_OUTLINE_WIDTH};
		lv_style_transition_dsc_init(
			&m_dragCompleteTransition, drag_complete_props, lv_anim_path_ease_in_out, TRANSITION_TIME, 0, NULL);
		lv_style_set_transition(lvgl.dragging, &m_dragCompleteTransition);

		lv_style_set_outline_color(lvgl.drag_complete, lv_color_white());
		lv_style_set_outline_width(lvgl.drag_complete, 3);

#if LV_USE_ARC
		lv_style_set_arc_color(lvgl.arc_indic, m_highlightColor);
		lv_style_set_arc_width(lvgl.arc_indic, LV_DPX_CALC(lv_display_get_dpi(NULL), 15));
		lv_style_set_arc_rounded(lvgl.arc_indic, true);

		lv_style_set_arc_color(lvgl.arc_indic_primary, m_primaryColor);
#endif

#if LV_USE_BAR
		lv_style_set_radius(lvgl.bar, LV_RADIUS_CIRCLE);
		lv_style_set_radius(lvgl.bar_indic, LV_RADIUS_CIRCLE);
#endif

#if LV_USE_BUTTONMATRIX
		lv_style_set_bg_color(lvgl.btnm_btn, m_primaryColor);
#endif

#if LV_USE_DROPDOWN
		lv_style_set_max_height(lvgl.dropdown_list, LV_DPI_DEF * 2);
#endif
#if LV_USE_CHECKBOX
		lv_style_set_pad_all(lvgl.cb_marker, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));
		lv_style_set_border_width(lvgl.cb_marker, BORDER_WIDTH);
		lv_style_set_border_color(lvgl.cb_marker, m_primaryColor);
		lv_style_set_bg_color(lvgl.cb_marker, m_cardColor);
		lv_style_set_bg_opa(lvgl.cb_marker, LV_OPA_COVER);
		lv_style_set_radius(lvgl.cb_marker, RADIUS_DEFAULT / 2);
		lv_style_set_text_font(lvgl.cb_marker, m_fontSmall);
		lv_style_set_text_color(lvgl.cb_marker, lv_color_white());

		lv_style_set_bg_image_src(lvgl.cb_marker_checked, LV_SYMBOL_OK);
#endif

#if LV_USE_SWITCH
		lv_style_set_bg_color(lvgl.bg_switch, GREY);
		lv_style_set_bg_opa(lvgl.bg_switch, LV_OPA_COVER);
		lv_style_set_radius(lvgl.bg_switch, LV_RADIUS_CIRCLE);
		lv_style_set_pad_all(lvgl.switch_knob, -LV_DPX_CALC(lv_display_get_dpi(NULL), 4));
		lv_style_set_bg_color(lvgl.switch_knob, lv_color_white());
		lv_style_set_radius(lvgl.switch_knob, LV_RADIUS_CIRCLE);
#endif

#if LV_USE_LINE
		lv_style_set_line_width(lvgl.line, 1);
		lv_style_set_line_color(lvgl.line, m_textColor);
#endif

#if LV_USE_CHART
		lv_style_set_border_width(lvgl.chart_bg, BORDER_WIDTH);
		lv_style_set_border_color(lvgl.chart_bg, m_highlightColor);
		lv_style_set_border_opa(lvgl.chart_bg, LV_OPA_COVER);
		lv_style_set_border_post(lvgl.chart_bg, false);
		lv_style_set_pad_column(lvgl.chart_bg, LV_DPX_CALC(lv_display_get_dpi(NULL), 10));
		lv_style_set_line_color(lvgl.chart_bg, m_highlightColor);

		lv_style_set_line_width(lvgl.chart_series, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));
		lv_style_set_radius(lvgl.chart_series, LV_DPX_CALC(lv_display_get_dpi(NULL), 3));

		int32_t chart_size = LV_DPX_CALC(lv_display_get_dpi(NULL), 8);
		lv_style_set_size(lvgl.chart_series, chart_size, chart_size);
		lv_style_set_pad_column(lvgl.chart_series, LV_DPX_CALC(lv_display_get_dpi(NULL), 2));

		lv_style_set_radius(lvgl.chart_indic, LV_RADIUS_CIRCLE);
		lv_style_set_size(lvgl.chart_indic, chart_size, chart_size);
		lv_style_set_bg_color(lvgl.chart_indic, m_primaryColor);
		lv_style_set_bg_opa(lvgl.chart_indic, LV_OPA_COVER);
#endif

#if LV_USE_MENU
		lv_style_set_pad_all(lvgl.menu_bg, 0);
		lv_style_set_pad_gap(lvgl.menu_bg, 0);
		lv_style_set_radius(lvgl.menu_bg, 0);
		lv_style_set_clip_corner(lvgl.menu_bg, true);
		lv_style_set_border_side(lvgl.menu_bg, LV_BORDER_SIDE_NONE);

		lv_style_set_radius(lvgl.menu_section, RADIUS_DEFAULT);
		lv_style_set_clip_corner(lvgl.menu_section, true);
		lv_style_set_bg_opa(lvgl.menu_section, LV_OPA_COVER);
		lv_style_set_bg_color(lvgl.menu_section, m_cardColor);
		lv_style_set_text_color(lvgl.menu_section, m_textColor);

		lv_style_set_pad_hor(lvgl.menu_cont, PAD_SMALL);
		lv_style_set_pad_ver(lvgl.menu_cont, PAD_SMALL);
		lv_style_set_pad_gap(lvgl.menu_cont, PAD_SMALL);
		lv_style_set_border_width(lvgl.menu_cont, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_opa(lvgl.menu_cont, LV_OPA_10);
		lv_style_set_border_color(lvgl.menu_cont, m_textColor);
		lv_style_set_border_side(lvgl.menu_cont, LV_BORDER_SIDE_NONE);

		lv_style_set_pad_all(lvgl.menu_sidebar_cont, 0);
		lv_style_set_pad_gap(lvgl.menu_sidebar_cont, 0);
		lv_style_set_border_width(lvgl.menu_sidebar_cont, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_opa(lvgl.menu_sidebar_cont, LV_OPA_10);
		lv_style_set_border_color(lvgl.menu_sidebar_cont, m_textColor);
		lv_style_set_border_side(lvgl.menu_sidebar_cont, LV_BORDER_SIDE_RIGHT);

		lv_style_set_pad_all(lvgl.menu_main_cont, 0);
		lv_style_set_pad_gap(lvgl.menu_main_cont, 0);

		lv_style_set_pad_hor(lvgl.menu_header_cont, PAD_SMALL);
		lv_style_set_pad_ver(lvgl.menu_header_cont, PAD_TINY);
		lv_style_set_pad_gap(lvgl.menu_header_cont, PAD_SMALL);

		lv_style_set_pad_hor(lvgl.menu_header_btn, PAD_TINY);
		lv_style_set_pad_ver(lvgl.menu_header_btn, PAD_TINY);
		lv_style_set_shadow_opa(lvgl.menu_header_btn, LV_OPA_TRANSP);
		lv_style_set_bg_opa(lvgl.menu_header_btn, LV_OPA_TRANSP);
		lv_style_set_text_color(lvgl.menu_header_btn, m_textColor);

		lv_style_set_pad_hor(lvgl.menu_page, 0);
		lv_style_set_pad_gap(lvgl.menu_page, 0);

		lv_style_set_bg_opa(lvgl.menu_pressed, LV_OPA_20);
		lv_style_set_bg_color(lvgl.menu_pressed, GREY);

		lv_style_set_bg_opa(lvgl.menu_separator, LV_OPA_TRANSP);
		lv_style_set_pad_ver(lvgl.menu_separator, PAD_TINY);
#endif

#if LV_USE_TABLE
		lv_style_set_radius(lvgl.table, 0);
		lv_style_set_bg_color(lvgl.table_cell, m_cardColor);
		lv_style_set_bg_opa(lvgl.table_cell, LV_OPA_COVER);
		lv_style_set_text_color(lvgl.table_cell, m_textColor);
		lv_style_set_border_width(lvgl.table_cell, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_color(lvgl.table_cell, m_highlightColor);
		lv_style_set_border_side(lvgl.table_cell,
								 static_cast<lv_border_side_t>(LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM));
#endif

#if LV_USE_TEXTAREA
		lv_style_set_border_color(lvgl.ta_cursor, m_textColor);
		lv_style_set_border_width(lvgl.ta_cursor, LV_DPX_CALC(lv_display_get_dpi(NULL), 2));
		lv_style_set_pad_left(lvgl.ta_cursor, -LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_side(lvgl.ta_cursor, LV_BORDER_SIDE_LEFT);
		lv_style_set_anim_duration(lvgl.ta_cursor, 400);

		lv_style_set_text_color(lvgl.ta_placeholder, m_darkMode ? DARK_GREY : LIGHT_GREY);
#endif

#if LV_USE_CALENDAR
		lv_style_set_pad_all(lvgl.calendar_btnm_bg, PAD_SMALL);
		lv_style_set_pad_gap(lvgl.calendar_btnm_bg, PAD_SMALL / 2);

		lv_style_set_border_width(lvgl.calendar_btnm_day, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_color(lvgl.calendar_btnm_day, m_highlightColor);
		lv_style_set_bg_color(lvgl.calendar_btnm_day, m_cardColor);
		lv_style_set_bg_opa(lvgl.calendar_btnm_day, LV_OPA_20);

		lv_style_set_pad_hor(lvgl.calendar_header, PAD_SMALL);
		lv_style_set_pad_top(lvgl.calendar_header, PAD_SMALL);
		lv_style_set_pad_bottom(lvgl.calendar_header, PAD_TINY);
		lv_style_set_pad_gap(lvgl.calendar_header, PAD_SMALL);
#endif

#if LV_USE_CANVAS
		lv_style_set_bg_color(lvgl.canvas, m_cardColor);
		lv_style_set_bg_opa(lvgl.canvas, LV_OPA_COVER);
		lv_style_set_border_width(lvgl.canvas, BORDER_WIDTH);
		lv_style_set_border_color(lvgl.canvas, m_darkMode ? LIGHT_GREY : DARK_GREY);
		lv_style_set_border_side(lvgl.canvas, LV_BORDER_SIDE_FULL);
#endif

#if LV_USE_MSGBOX
		lv_style_set_bg_color(lvgl.msgbox_backdrop_bg, GREY);
		lv_style_set_bg_opa(lvgl.msgbox_backdrop_bg, LV_OPA_50);
#endif
#if LV_USE_KEYBOARD
		lv_style_set_bg_color(lvgl.keyboard_button_bg, m_darkMode ? DARK_GREY : LIGHT_GREY);
		lv_style_set_shadow_width(lvgl.keyboard_button_bg, 0);
		lv_style_set_radius(lvgl.keyboard_button_bg, RADIUS_DEFAULT);
		lv_style_set_bg_color(lvgl.keyboard_button_checked_bg, m_highlightColor);
		lv_style_set_bg_opa(lvgl.keyboard_button_checked_bg, LV_OPA_COVER);
		lv_style_set_text_color(lvgl.keyboard_button_checked_bg, m_textColor);
#endif

#if LV_USE_TABVIEW
		lv_style_set_border_color(lvgl.tab_btn, m_primaryColor);
		lv_style_set_border_width(lvgl.tab_btn, BORDER_WIDTH * 2);
		lv_style_set_border_side(lvgl.tab_btn, LV_BORDER_SIDE_BOTTOM);
		lv_style_set_pad_top(lvgl.tab_btn, BORDER_WIDTH * 2);

		lv_style_set_outline_pad(lvgl.tab_bg_focus, -BORDER_WIDTH);
#endif

#if LV_USE_LIST
		lv_style_set_pad_hor(lvgl.list_bg, PAD_DEF);
		lv_style_set_pad_ver(lvgl.list_bg, 0);
		lv_style_set_pad_gap(lvgl.list_bg, 0);
		lv_style_set_clip_corner(lvgl.list_bg, true);

		lv_style_set_border_width(lvgl.list_btn, LV_DPX_CALC(lv_display_get_dpi(NULL), 1));
		lv_style_set_border_color(lvgl.list_btn, m_highlightColor);
		lv_style_set_border_side(lvgl.list_btn, LV_BORDER_SIDE_BOTTOM);
		lv_style_set_pad_all(lvgl.list_btn, PAD_SMALL);
		lv_style_set_pad_column(lvgl.list_btn, PAD_SMALL);

		lv_style_set_transform_width(lvgl.list_item_grow, PAD_DEF);
#endif

#if LV_USE_LED
		lv_style_set_bg_opa(lvgl.led, LV_OPA_COVER);
		lv_style_set_bg_color(lvgl.led, lv_color_white());
		lv_style_set_bg_grad_color(lvgl.led, GREY);
		lv_style_set_radius(lvgl.led, LV_RADIUS_CIRCLE);
		lv_style_set_shadow_width(lvgl.led, LV_DPX_CALC(lv_display_get_dpi(NULL), 15));
		lv_style_set_shadow_color(lvgl.led, lv_color_white());
		lv_style_set_shadow_spread(lvgl.led, LV_DPX_CALC(lv_display_get_dpi(NULL), 5));
#endif

#if LV_USE_SCALE
		lv_style_set_line_color(lvgl.scale, m_textColor);
		lv_style_set_line_width(lvgl.scale, LV_DPX(2));
		lv_style_set_arc_color(lvgl.scale, m_textColor);
		lv_style_set_arc_width(lvgl.scale, LV_DPX(2));
		lv_style_set_length(lvgl.scale, LV_DPX(6));
#endif

#if LV_USE_SLIDER
		lv_style_set_radius(lvgl.slider, LV_RADIUS_CIRCLE);
		lv_style_set_radius(lvgl.slider_indic, LV_RADIUS_CIRCLE);
#endif

		lv_style_set_bg_color(components.estop, lv_palette_main(LV_PALETTE_RED));
		lv_style_set_radius(components.estop, LV_RADIUS_CIRCLE);
		lv_style_set_bg_color(components.folder, m_primaryColor);

		lv_style_set_bg_color(components.unhomed, lv_color_hex(0xfb9514));
		lv_style_set_bg_opa(components.temperature_bar, LV_OPA_COVER);
		lv_style_set_bg_color(components.temperature_bar, lv_palette_main(LV_PALETTE_BLUE));
		lv_style_set_bg_grad_color(components.temperature_bar, lv_palette_main(LV_PALETTE_RED));
		lv_style_set_bg_grad_dir(components.temperature_bar, LV_GRAD_DIR_HOR);
	}
} // namespace UI::Themes
