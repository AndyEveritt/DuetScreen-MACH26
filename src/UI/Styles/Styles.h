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
		lv_style_t style;
		const char* name;
		std::function<void(lv_style_t*)> initFunc;
	};

	struct LvglStyles
	{
		LvglStyles();

		Style base; // Base style applied to all objects

		/* Background */
		Style bg_dark;
		Style bg;
		Style bg_light;
		Style bg_color_primary; // button, button matrix (checked), bar indicator, slider indicator, table part focus
								// key, checkbox indicator, switch indicator, roller selected, dropdown list selected,
								// textarea selected, list button focus key, spinbox cursor, msgbox buttons
		Style bg_color_primary_muted;
		Style bg_color_secondary;
		Style bg_color_secondary_muted;
		Style bg_color_header;
		Style bg_color_list_item;

		/* Text */
		Style text;
		Style text_muted;
		Style text_header;

		/* Border */
		Style border;
		Style border_muted;
		Style border_highlight;
		Style border_color_primary;
		Style border_color_secondary;
		Style border_color_card;

		Style screen;
		Style scrollbar;
		Style scrollbar_scrolled;
		Style card;
		Style btn;
		Style btn_checked;

		/* Icons */
		Style icon;
		Style icon_recolor;

		/*Utility*/
		Style pressed;
		Style disabled;
		Style pad_base;
		Style pad_zero;
		Style pad_tiny;
		Style pad_small;
		Style pad_normal;
		Style pad_gap;
		Style line_space_large;	 // roller, dropdown
		Style text_align_center; // roller
		Style outline_primary;	 // Focused via keypad or encoder
		Style outline_secondary; // Edited by an encoder
		Style circle;
		Style no_radius;
		Style no_border;
		Style clip_corner;
		Style rotary_scroll;
		Style grow;
		Style transition_delayed;
		Style transition_normal;
		Style anim;
		Style anim_fast;

		Style actionBtn; // for UI elements that perform actions on the Duet
		Style input;
		Style long_press;
		Style draggable;
		Style dragging;
		Style drag_complete;

		/*Parts*/
		Style knob;

#if LV_USE_ARC
		Style arc_indic;
		Style arc_indic_primary;
#endif

#if LV_USE_BAR
		Style bar;
		Style bar_indic;
#endif

#if LV_USE_BUTTONMATRIX
		Style btnm_bg;
		Style btnm_btn;
#endif

#if LV_USE_CHART
		Style chart_series;
		Style chart_indic;
		Style chart_bg;
#endif

#if LV_USE_DROPDOWN
		Style dropdown_list;
#endif

#if LV_USE_CHECKBOX
		Style cb_marker;
		Style cb_marker_checked;
#endif

#if LV_USE_SWITCH
		Style bg_switch;
		Style switch_knob;
#endif

#if LV_USE_LINE
		Style line;
#endif

#if LV_USE_TABLE
		Style table;
		Style table_cell;
#endif

#if LV_USE_TEXTAREA
		Style ta_cursor;
		Style ta_placeholder;
#endif

#if LV_USE_CALENDAR
		Style calendar_btnm_bg;
		Style calendar_btnm_day;
		Style calendar_header;
#endif

#if LV_USE_CANVAS
		Style canvas;
#endif

#if LV_USE_MENU
		Style menu_bg;
		Style menu_cont;
		Style menu_sidebar_cont;
		Style menu_main_cont;
		Style menu_page;
		Style menu_header_cont;
		Style menu_header_btn;
		Style menu_section;
		Style menu_pressed;
		Style menu_separator;
#endif

#if LV_USE_MSGBOX
		Style msgbox_backdrop_bg;
#endif

#if LV_USE_KEYBOARD
		Style keyboard_button_bg;
		Style keyboard_button_checked_bg;
#endif

#if LV_USE_LIST
		Style list_bg;
		Style list_btn;
		Style list_item_grow;
#endif

#if LV_USE_TABVIEW
		Style tab_bg_focus;
		Style tab_btn;
#endif
#if LV_USE_LED
		Style led;
#endif

#if LV_USE_SCALE
		Style scale;
#endif

#if LV_USE_SLIDER
		Style slider;
		Style slider_indic;
		Style slider_knob;
#endif
	};

	struct ComponentStyles
	{
		Style estop;				 // Emergency stop button style
		Style file;					 // File item style
		Style folder;				 // Folder item style
		Style unhomed;				 // Used for the move view
		Style sidebar;				 // Sidebar style
		Style temperature_bar;		 // Temperature bar style
		Style temperature_bar_indic; // Temperature bar indicator style
		Style tool_selected;		 // Used in tool list
	};

	const LvglStyles& getLvglStyles();
	const ComponentStyles& getComponentStyles();

	class Theme
	{
	  public:
		Theme(const char* name, std::function<void(Theme* theme)> initFunc = nullptr);
		~Theme() = default;
		Theme& operator=(const Theme&) = delete;

		void init();

		void setThemeActive() const;

		const std::string& getName() const { return m_name; }

		// Base LVGL styles applied to existing and newly created objects
		LvglStyles lvgl;

		// Specific component styles
		ComponentStyles components;

	  private:
		virtual void onInit() {}

		const std::string m_name;
		std::function<void(Theme*)> m_initFunc;

		enum class DisplaySize_t
		{
			DISP_SMALL = 3,
			DISP_MEDIUM = 2,
			DISP_LARGE = 1,
		};
		DisplaySize_t m_displaySize;
	};

	void init(lv_display_t* display);
	const std::vector<Theme*>& getThemes();
	Theme* getCurrentTheme();
	const Theme* getTheme(const size_t index);
	const Theme* getThemeByName(const char* name);
	const size_t getThemeCount();
	bool refreshCurrentTheme();
	const std::vector<std::string> getThemeNames();

#if DEBUG_BORDERS
	bool isdebugBorderVisible(lv_obj_t* obj);
	void showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive = true);
#endif
} // namespace UI::Themes

bool lv_obj_has_style(lv_obj_t* obj, const lv_style_t* style);
void lv_obj_add_style(lv_obj_t* obj, const lv_style_t* style, lv_style_selector_t selector, const bool recursive);
void lv_obj_remove_style(lv_obj_t* obj, const lv_style_t* style, lv_style_selector_t selector, const bool recursive);
