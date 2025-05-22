/*
 * Styles.h
 *
 *  Created on: 2025-01-21
 *      Author: Andy Everitt
 */

#pragma once

#include "lvgl/lvgl.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace UI::Themes
{
	struct Style
	{
		Style();
		Style(const char* name);
		Style(const char* name, std::function<void(lv_style_t*)> initFunc);
		Style(const Style&);
		void init();

		Style& operator=(const Style& other);

		operator lv_style_t*() { return &style; }
		operator const lv_style_t*() const { return &style; }

	  private:
		const char* name;
		lv_style_t style;
		std::function<void(lv_style_t*)> initFunc;
	};

	struct LvglStyles
	{
		LvglStyles();

		Style screen;
		Style scrollbar;
		Style scrollbar_scrolled;
		Style card;
		Style btn;

		/*Utility*/
		Style bg_color_primary;
		Style bg_color_primary_muted;
		Style bg_color_secondary;
		Style bg_color_secondary_muted;
		Style bg_color_grey;
		Style bg_color_white;
		Style pressed;
		Style disabled;
		Style pad_zero;
		Style pad_tiny;
		Style pad_small;
		Style pad_normal;
		Style pad_gap;
		Style line_space_large;
		Style text_align_center;
		Style outline_primary;
		Style outline_secondary;
		Style circle;
		Style no_radius;
		Style clip_corner;
		Style rotary_scroll;
#if LV_THEME_DEFAULT_GROW
		Style grow;
#endif
		Style transition_delayed;
		Style transition_normal;
		Style anim;
		Style anim_fast;

		/*Parts*/
		Style knob;

#if LV_USE_ARC
		Style arc_indic;
		Style arc_indic_primary;
#endif

#if LV_USE_CHART
		Style chart_series, chart_indic, chart_bg;
#endif

#if LV_USE_DROPDOWN
		Style dropdown_list;
#endif

#if LV_USE_CHECKBOX
		Style cb_marker, cb_marker_checked;
#endif

#if LV_USE_SWITCH
		Style switch_knob;
#endif

#if LV_USE_LINE
		Style line;
#endif

#if LV_USE_TABLE
		Style table_cell;
#endif

#if LV_USE_TEXTAREA
		Style ta_cursor, ta_placeholder;
#endif

#if LV_USE_CALENDAR
		Style calendar_btnm_bg, calendar_btnm_day, calendar_header;
#endif

#if LV_USE_MENU
		Style menu_bg, menu_cont, menu_sidebar_cont, menu_main_cont, menu_page, menu_header_cont, menu_header_btn,
			menu_section, menu_pressed, menu_separator;
#endif

#if LV_USE_MSGBOX
		Style msgbox_backdrop_bg;
#endif

#if LV_USE_KEYBOARD
		Style keyboard_button_bg;
#endif

#if LV_USE_LIST
		Style list_bg, list_btn, list_item_grow;
#endif

#if LV_USE_TABVIEW
		Style tab_bg_focus, tab_btn;
#endif
#if LV_USE_LED
		Style led;
#endif

#if LV_USE_SCALE
		Style scale;
#endif
	};

	const LvglStyles& getLvglStyles();
	const Style& getBaseStyle();
	const Style& getEStopStyle();

	class Theme
	{
	  public:
		Theme(const char* name);
		~Theme() = default;
		Theme& operator=(const Theme&) = delete;

		virtual void init() {}

		void applyTheme() const;
		const std::string& getName() const { return m_name; }

	  protected:
		LvglStyles m_lvglStyles;
		Style m_estop;

	  private:
		const std::string m_name;
	};

	void init(lv_display_t* display);
	const std::vector<Theme*>& getThemes();
	const Theme& getCurrentTheme();
	const Theme* getTheme(const size_t index);
	const Theme* getThemeByName(const char* name);
	const size_t getThemeCount();
	const std::vector<std::string> getThemeNames();

#if DEBUG_BORDERS
	bool isdebugBorderVisible(lv_obj_t* obj);
	void showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive = true);
#endif
} // namespace UI

bool lv_obj_has_style(lv_obj_t* obj, const lv_style_t* style);
void lv_obj_add_style(lv_obj_t* obj, const lv_style_t* style, lv_style_selector_t selector, const bool recursive);
void lv_obj_remove_style(lv_obj_t* obj, const lv_style_t* style, lv_style_selector_t selector, const bool recursive);
