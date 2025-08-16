/*
 * Styles.cpp
 *
 *  Created on: 2025-01-21
 *      Author: Andy Everitt
 */

#include "Styles.h"
#include "Debug.h"
#include "UI/Core/View.h"
#include "lvgl/src/lvgl_private.h"
#include "utils/StorageHelper.h"
#include <array>
#include <map>
#include <memory>

namespace UI::Themes
{
	static lv_theme_t s_theme;
	// Use function-local statics to avoid cross-TU static initialization order issues
	static std::vector<Style*>& uninitializedStyles()
	{
		static std::vector<Style*> v;
		return v;
	}

	static std::vector<Theme*>& themes()
	{
		static std::vector<Theme*> v;
		return v;
	}

#define STATIC_ON_FIRST_USE(type, name)                                                                                \
	static type& name()                                                                                                \
	{                                                                                                                  \
		static type s_##name;                                                                                          \
		return s_##name;                                                                                               \
	}

	static std::unique_ptr<LvglStyles> s_lvglStyles;

	static std::unique_ptr<ComponentStyles> s_componentStyles;

	static Theme* s_currentTheme = nullptr;

#if DEBUG_BORDERS
	static Style s_debugBorders("debugBorders",
								[](lv_style_t* style)
								{
									lv_style_set_border_color(style, lv_color_black());
									lv_style_set_border_width(style, 2);
									lv_style_set_border_opa(style, LV_OPA_100);
								});
#endif

	Style::Style()
		: name(nullptr)
		, initFunc(nullptr)
	{
		lv_style_init(&style);
	}

	Style::Style(const char* name)
		: name(name)
		, initFunc(nullptr)
	{
		lv_style_init(&style);
	}

	Style::Style(const char* name, std::function<void(lv_style_t*)> initFunc)
		: Style(name)
	{
		if (!lv_is_initialized())
		{
			LOG_DBG("LVGL not initialized, deferring style initialization");
			this->initFunc = initFunc;
			uninitializedStyles().push_back(this);
		}
		else
		{
			initFunc(&style);
		}
	}

	Style::Style(const Style& other)
		: name(other.name)
	{
		UI_LOCK();
		lv_style_copy(&style, &other.style);
		lv_obj_report_style_change(&style);
	}

	void Style::init()
	{
		if (initFunc)
		{
			initFunc(&style);
			initFunc = nullptr;
		}
	}

	Style& Style::operator=(const Style& other)
	{
		UI_LOCK();
		lv_style_copy(&style, &other.style);
		lv_obj_report_style_change(&style);
		return *this;
	}

	LvglStyles::LvglStyles() {}

	const LvglStyles& getLvglStyles()
	{
		if (!s_lvglStyles)
		{
			s_lvglStyles = std::make_unique<LvglStyles>();
		}
		return *s_lvglStyles;
	}

	void setLvglStyles(const LvglStyles& styles)
	{
		if (!s_lvglStyles)
		{
			s_lvglStyles = std::make_unique<LvglStyles>();
		}
		*s_lvglStyles = styles;
	}

	const ComponentStyles& getComponentStyles()
	{
		if (!s_componentStyles)
		{
			s_componentStyles = std::make_unique<ComponentStyles>();
		}
		return *s_componentStyles;
	}

	void setComponentStyles(const ComponentStyles& styles)
	{
		if (!s_componentStyles)
		{
			s_componentStyles = std::make_unique<ComponentStyles>();
		}
		*s_componentStyles = styles;
	}

	static bool themeExists(const char* name)
	{
		for (const auto& theme : themes())
		{
			if (theme->getName() == name)
			{
				return true;
			}
		}
		return false;
	}

	Theme::Theme(const char* name, std::function<void(Theme* theme)> initFunc)
		: m_name(name)
		, m_initFunc(initFunc)
	{
		if (themeExists(name))
		{
			LOG_FATAL_THROW("Theme with name {:s} already exists", name);
			return;
		}
		themes().push_back(this);
		LOG_INFO("Theme {:s} created", name);
	}

	void Theme::init()
	{
		UI_LOCK();
		LOG_INFO("Initializing theme: {:s}", m_name);

		lvgl = std::make_unique<LvglStyles>();
		components = std::make_unique<ComponentStyles>();

		onInit();

		if (m_initFunc)
		{
			m_initFunc(this);
		}
	}

	void Theme::setThemeActive() const
	{
		LOG_INFO("Applying theme: {:s}", m_name);
		setLvglStyles(getLvglStyles());
		setComponentStyles(getComponentStyles());

		s_currentTheme = const_cast<Theme*>(this);

		lv_obj_report_style_change(NULL);
		lv_obj_invalidate(lv_screen_active());
	}

	const LvglStyles& Theme::getLvglStyles() const
	{
		if (!lvgl)
		{
			LOG_FATAL_THROW("LVGL styles not initialized");
		}
		return *lvgl;
	}

	const ComponentStyles& Theme::getComponentStyles() const
	{
		if (!components)
		{
			LOG_FATAL_THROW("Component styles not initialized");
		}
		return *components;
	}

	LvglStyles& Theme::getLvglStyles()
	{
		if (!lvgl)
		{
			lvgl = std::make_unique<LvglStyles>();
		}
		return *lvgl;
	}

	ComponentStyles& Theme::getComponentStyles()
	{
		if (!components)
		{
			components = std::make_unique<ComponentStyles>();
		}
		return *components;
	}

	/**
	 * @brief Callback function to apply theme styles to LVGL objects
	 *
	 * This function applies various styles to LVGL objects based on their type:
	 * - Applies base style to all objects
	 * - Container style to generic objects
	 * - Button style to button objects
	 * - Label style to label objects
	 * - Debug borders in debug mode if enabled
	 *
	 * @param th Pointer to the LVGL theme (unused)
	 * @param obj Pointer to the LVGL object to apply styles to
	 *
	 * @note Function is protected by UI_LOCK()
	 */
	static void applyThemeCb(lv_theme_t* th, lv_obj_t* obj)
	{
		UI_LOCK();
		LV_UNUSED(th);

		lv_obj_t* parent = lv_obj_get_parent(obj);

		lv_obj_add_style(obj, getLvglStyles().base, 0);

		if (parent == NULL)
		{
			lv_obj_add_style(obj, getLvglStyles().screen, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			return;
		}

#if 1
		if (lv_obj_check_type(obj, &lv_obj_class))
		{
#  if LV_USE_TABVIEW
			/*Tabview content area*/
			if (lv_obj_check_type(parent, &lv_tabview_class) && lv_obj_get_child(parent, 1) == obj)
			{
				return;
			}
			/*Tabview button container*/
			else if (lv_obj_check_type(parent, &lv_tabview_class) && lv_obj_get_child(parent, 0) == obj)
			{
				lv_obj_add_style(obj, getLvglStyles().bg_color_list_item, 0);
				lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
				lv_obj_add_style(obj, getLvglStyles().tab_bg_focus, LV_STATE_FOCUS_KEY);
				return;
			}
			/*Tabview pages*/
			else if (lv_obj_check_type(lv_obj_get_parent(parent), &lv_tabview_class))
			{
				lv_obj_add_style(obj, getLvglStyles().pad_normal, 0);
				lv_obj_add_style(obj, getLvglStyles().rotary_scroll, 0);
				lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
				lv_obj_add_style(obj,
								 getLvglStyles().scrollbar_scrolled,
								 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
				return;
			}
#  endif

#  if LV_USE_WIN
			/*Header*/
			if (lv_obj_check_type(parent, &lv_win_class) && lv_obj_get_child(parent, 0) == obj)
			{
				lv_obj_add_style(obj, getLvglStyles().bg_color_header, 0);
				lv_obj_add_style(obj, getLvglStyles().pad_tiny, 0);
				return;
			}
			/*Content*/
			else if (lv_obj_check_type(parent, &lv_win_class) && lv_obj_get_child(parent, 1) == obj)
			{
				lv_obj_add_style(obj, getLvglStyles().screen, 0);
				lv_obj_add_style(obj, getLvglStyles().pad_normal, 0);
				lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
				lv_obj_add_style(obj,
								 getLvglStyles().scrollbar_scrolled,
								 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
				return;
			}
#  endif

#  if LV_USE_CALENDAR
			if (lv_obj_check_type(parent, &lv_calendar_class))
			{
				/*No style*/
				return;
			}
#  endif

			lv_obj_add_style(obj, getLvglStyles().pad_base, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
		}
#  if LV_USE_BUTTON
		else if (lv_obj_check_type(obj, &lv_button_class))
		{

#	if LV_USE_TABVIEW
			lv_obj_t* tv = lv_obj_get_parent(parent); /*parent is the tabview header*/
			if (tv && lv_obj_get_child(tv, 0) == parent)
			{ /*The button is on the tab view header*/
				if (lv_obj_check_type(tv, &lv_tabview_class))
				{
					lv_obj_add_style(obj, getLvglStyles().pressed, LV_STATE_PRESSED);
					lv_obj_add_style(obj, getLvglStyles().bg_color_primary_muted, LV_STATE_CHECKED);
					lv_obj_add_style(obj, getLvglStyles().tab_btn, LV_STATE_CHECKED);
					lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
					lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
					lv_obj_add_style(obj, getLvglStyles().tab_bg_focus, LV_STATE_FOCUS_KEY);
					return;
				}
			}

#	endif
			lv_obj_add_style(obj, getLvglStyles().pad_base, 0);
			lv_obj_add_style(obj, getLvglStyles().btn, 0);
			lv_obj_add_style(obj, getLvglStyles().transition_delayed, 0);
			lv_obj_add_style(obj, getLvglStyles().pressed, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().transition_normal, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().grow, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().btn_checked, LV_STATE_CHECKED);
			lv_obj_add_style(obj, getLvglStyles().disabled, LV_STATE_DISABLED);

#	if LV_USE_MENU
			if (lv_obj_check_type(parent, &lv_menu_sidebar_header_cont_class) ||
				lv_obj_check_type(parent, &lv_menu_main_header_cont_class))
			{
				lv_obj_add_style(obj, getLvglStyles().menu_header_btn, 0);
				lv_obj_add_style(obj, getLvglStyles().menu_pressed, LV_STATE_PRESSED);
			}
#	endif
		}
#  endif

#  if LV_USE_LINE
		else if (lv_obj_check_type(obj, &lv_line_class))
		{
			lv_obj_add_style(obj, getLvglStyles().line, 0);
		}
#  endif

#  if LV_USE_BUTTONMATRIX
		else if (lv_obj_check_type(obj, &lv_buttonmatrix_class))
		{

#	if LV_USE_CALENDAR
			if (lv_obj_check_type(parent, &lv_calendar_class))
			{
				lv_obj_add_style(obj, getLvglStyles().calendar_btnm_bg, 0);
				lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
				lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
				lv_obj_add_style(obj, getLvglStyles().calendar_btnm_day, LV_PART_ITEMS);
				lv_obj_add_style(
					obj, getLvglStyles().pressed, static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_PRESSED));
				lv_obj_add_style(obj,
								 getLvglStyles().disabled,
								 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_DISABLED));
				lv_obj_add_style(obj,
								 getLvglStyles().outline_primary,
								 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_FOCUS_KEY));
				lv_obj_add_style(obj,
								 getLvglStyles().outline_secondary,
								 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_EDITED));
				return;
			}
#	endif
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().btnm_bg, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().btn, LV_PART_ITEMS);
			lv_obj_add_style(obj, getLvglStyles().btnm_btn, LV_PART_ITEMS);
			lv_obj_add_style(
				obj, getLvglStyles().disabled, static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_DISABLED));
			lv_obj_add_style(
				obj, getLvglStyles().pressed, static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_secondary,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(obj,
							 getLvglStyles().outline_primary,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_FOCUS_KEY));
			lv_obj_add_style(obj,
							 getLvglStyles().outline_secondary,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_EDITED));
		}
#  endif

#  if LV_USE_CANVAS
		else if (lv_obj_check_type(obj, &lv_canvas_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().canvas, LV_PART_MAIN);
		}
#  endif

#  if LV_USE_BAR
		else if (lv_obj_check_type(obj, &lv_bar_class))
		{
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary_muted, 0);
			lv_obj_add_style(obj, getLvglStyles().bar, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().bar_indic, LV_PART_INDICATOR);
		}
#  endif

#  if LV_USE_SLIDER
		else if (lv_obj_check_type(obj, &lv_slider_class))
		{
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary_muted, 0);
			lv_obj_add_style(obj, getLvglStyles().slider, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().slider_indic, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().knob, LV_PART_KNOB);
			lv_obj_add_style(obj, getLvglStyles().slider_knob, LV_PART_KNOB);
			lv_obj_add_style(
				obj, getLvglStyles().grow, static_cast<int>(LV_PART_KNOB) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(obj, getLvglStyles().transition_delayed, LV_PART_KNOB);
			lv_obj_add_style(obj,
							 getLvglStyles().transition_normal,
							 static_cast<int>(LV_PART_KNOB) | static_cast<int>(LV_STATE_PRESSED));
		}
#  endif

#  if LV_USE_TABLE
		else if (lv_obj_check_type(obj, &lv_table_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_zero, 0);
			lv_obj_add_style(obj, getLvglStyles().table, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			lv_obj_add_style(obj, getLvglStyles().table_cell, LV_PART_ITEMS);
			lv_obj_add_style(obj, getLvglStyles().pad_normal, LV_PART_ITEMS);
			lv_obj_add_style(
				obj, getLvglStyles().pressed, static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_primary,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_FOCUS_KEY));
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_secondary,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_EDITED));
		}
#  endif

#  if LV_USE_CHECKBOX
		else if (lv_obj_check_type(obj, &lv_checkbox_class))
		{
			lv_obj_add_style(obj, getLvglStyles().pad_gap, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj,
							 getLvglStyles().disabled,
							 static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_DISABLED));
			lv_obj_add_style(obj, getLvglStyles().cb_marker, LV_PART_INDICATOR);
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_primary,
							 static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(obj,
							 getLvglStyles().cb_marker_checked,
							 static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(
				obj, getLvglStyles().pressed, static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(
				obj, getLvglStyles().grow, static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(obj,
							 getLvglStyles().transition_normal,
							 static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(obj, getLvglStyles().transition_delayed, LV_PART_INDICATOR);
		}
#  endif

#  if LV_USE_SWITCH
		else if (lv_obj_check_type(obj, &lv_switch_class))
		{
			lv_obj_add_style(obj, getLvglStyles().bg_switch, 0);
			lv_obj_add_style(obj, getLvglStyles().anim_fast, 0);
			lv_obj_add_style(obj, getLvglStyles().disabled, LV_STATE_DISABLED);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_primary,
							 static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(obj, getLvglStyles().knob, LV_PART_KNOB);
			lv_obj_add_style(obj, getLvglStyles().switch_knob, LV_PART_KNOB);

			lv_obj_add_style(obj,
							 getLvglStyles().transition_normal,
							 static_cast<int>(LV_PART_INDICATOR) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(obj, getLvglStyles().transition_normal, LV_PART_INDICATOR);
		}
#  endif

#  if LV_USE_CHART
		else if (lv_obj_check_type(obj, &lv_chart_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_small, 0);
			lv_obj_add_style(obj, getLvglStyles().chart_bg, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			lv_obj_add_style(obj, getLvglStyles().chart_series, LV_PART_ITEMS);
			lv_obj_add_style(obj, getLvglStyles().chart_indic, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().chart_series, LV_PART_CURSOR);
		}
#  endif

#  if LV_USE_ROLLER
		else if (lv_obj_check_type(obj, &lv_roller_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().anim, 0);
			lv_obj_add_style(obj, getLvglStyles().line_space_large, 0);
			lv_obj_add_style(obj, getLvglStyles().text_align_center, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_PART_SELECTED);
		}
#  endif

#  if LV_USE_DROPDOWN
		else if (lv_obj_check_type(obj, &lv_dropdown_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_small, 0);
			lv_obj_add_style(obj, getLvglStyles().transition_delayed, 0);
			lv_obj_add_style(obj, getLvglStyles().transition_normal, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().pressed, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().transition_normal, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().disabled, LV_STATE_DISABLED);
		}
		else if (lv_obj_check_type(obj, &lv_dropdownlist_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().clip_corner, 0);
			lv_obj_add_style(obj, getLvglStyles().line_space_large, 0);
			lv_obj_add_style(obj, getLvglStyles().dropdown_list, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_primary,
							 static_cast<int>(LV_PART_SELECTED) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(
				obj, getLvglStyles().pressed, static_cast<int>(LV_PART_SELECTED) | static_cast<int>(LV_STATE_PRESSED));
		}
#  endif

#  if LV_USE_ARC
		else if (lv_obj_check_type(obj, &lv_arc_class))
		{
			lv_obj_add_style(obj, getLvglStyles().arc_indic, 0);
			lv_obj_add_style(obj, getLvglStyles().arc_indic, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().arc_indic_primary, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().knob, LV_PART_KNOB);
		}
#  endif

#  if LV_USE_SPINNER
		else if (lv_obj_check_type(obj, &lv_spinner_class))
		{
			lv_obj_add_style(obj, getLvglStyles().arc_indic, 0);
			lv_obj_add_style(obj, getLvglStyles().arc_indic, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().arc_indic_primary, LV_PART_INDICATOR);
		}
#  endif

#  if LV_USE_TEXTAREA
		else if (lv_obj_check_type(obj, &lv_textarea_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_small, 0);
			lv_obj_add_style(obj, getLvglStyles().disabled, LV_STATE_DISABLED);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			lv_obj_add_style(
				obj, getLvglStyles().ta_cursor, static_cast<int>(LV_PART_CURSOR) | static_cast<int>(LV_STATE_FOCUSED));
			lv_obj_add_style(obj, getLvglStyles().ta_placeholder, LV_PART_TEXTAREA_PLACEHOLDER);
		}
#  endif

#  if LV_USE_CALENDAR
		else if (lv_obj_check_type(obj, &lv_calendar_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_zero, 0);
		}

#	if LV_USE_CALENDAR_HEADER_ARROW
		else if (lv_obj_check_type(obj, &lv_calendar_header_arrow_class))
		{
			lv_obj_add_style(obj, getLvglStyles().calendar_header, 0);
		}
#	endif

#	if LV_USE_CALENDAR_HEADER_DROPDOWN
		else if (lv_obj_check_type(obj, &lv_calendar_header_dropdown_class))
		{
			lv_obj_add_style(obj, getLvglStyles().calendar_header, 0);
		}
#	endif
#  endif

#  if LV_USE_KEYBOARD
		else if (lv_obj_check_type(obj, &lv_keyboard_class))
		{
			lv_obj_add_style(obj, getLvglStyles().screen, 0);
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_small, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().btn, LV_PART_ITEMS);
			lv_obj_add_style(obj, getLvglStyles().disabled, static_cast<int>(LV_PART_ITEMS) | LV_STATE_DISABLED);
			lv_obj_add_style(obj, getLvglStyles().keyboard_button_bg, LV_PART_ITEMS);
			lv_obj_add_style(
				obj, getLvglStyles().pressed, static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_PRESSED));
			lv_obj_add_style(obj,
							 getLvglStyles().keyboard_button_checked_bg,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_primary_muted,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_FOCUS_KEY));
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_secondary_muted,
							 static_cast<int>(LV_PART_ITEMS) | static_cast<int>(LV_STATE_EDITED));
		}
#  endif

#  if LV_USE_LABEL && LV_USE_TEXTAREA
		else if (lv_obj_check_type(obj, &lv_label_class) && lv_obj_check_type(parent, &lv_textarea_class))
		{
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_PART_SELECTED);
		}
#  endif

#  if LV_USE_LIST
		else if (lv_obj_check_type(obj, &lv_list_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_normal, 0);
			lv_obj_add_style(obj, getLvglStyles().list_bg, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			return;
		}
		else if (lv_obj_check_type(obj, &lv_list_text_class))
		{
			lv_obj_add_style(obj, getLvglStyles().bg_color_header, 0);
			lv_obj_add_style(obj, getLvglStyles().list_item_grow, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_small, 0);
		}
		else if (lv_obj_check_type(obj, &lv_list_button_class))
		{
			lv_obj_add_style(obj, getLvglStyles().bg_color_list_item, 0);
			lv_obj_add_style(obj, getLvglStyles().list_btn, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_normal, 0);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().list_item_grow, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().list_item_grow, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().pressed, LV_STATE_PRESSED);
		}
#  endif
#  if LV_USE_MENU
		else if (lv_obj_check_type(obj, &lv_menu_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().menu_bg, 0);
		}
		else if (lv_obj_check_type(obj, &lv_menu_sidebar_cont_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_sidebar_cont, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
		}
		else if (lv_obj_check_type(obj, &lv_menu_main_cont_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_main_cont, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
		}
		else if (lv_obj_check_type(obj, &lv_menu_cont_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_cont, 0);
			lv_obj_add_style(obj, getLvglStyles().menu_pressed, LV_STATE_PRESSED);
			lv_obj_add_style(obj,
							 getLvglStyles().bg_color_primary_muted,
							 static_cast<int>(LV_STATE_PRESSED) | static_cast<int>(LV_STATE_CHECKED));
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary_muted, LV_STATE_CHECKED);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_STATE_FOCUS_KEY);
		}
		else if (lv_obj_check_type(obj, &lv_menu_sidebar_header_cont_class) ||
				 lv_obj_check_type(obj, &lv_menu_main_header_cont_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_header_cont, 0);
		}
		else if (lv_obj_check_type(obj, &lv_menu_page_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_page, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
		}
		else if (lv_obj_check_type(obj, &lv_menu_section_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_section, 0);
		}
		else if (lv_obj_check_type(obj, &lv_menu_separator_class))
		{
			lv_obj_add_style(obj, getLvglStyles().menu_separator, 0);
		}
#  endif
#  if LV_USE_MSGBOX
		else if (lv_obj_check_type(obj, &lv_msgbox_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().clip_corner, 0);
			return;
		}
		else if (lv_obj_check_type(obj, &lv_msgbox_backdrop_class))
		{
			lv_obj_add_style(obj, getLvglStyles().msgbox_backdrop_bg, 0);
			return;
		}
		else if (lv_obj_check_type(obj, &lv_msgbox_header_class))
		{
			lv_obj_add_style(obj, getLvglStyles().pad_tiny, 0);
			lv_obj_add_style(obj, getLvglStyles().bg_color_header, 0);
			return;
		}
		else if (lv_obj_check_type(obj, &lv_msgbox_footer_class))
		{
			lv_obj_add_style(obj, getLvglStyles().pad_tiny, 0);
			return;
		}
		else if (lv_obj_check_type(obj, &lv_msgbox_content_class))
		{
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
			lv_obj_add_style(obj, getLvglStyles().pad_tiny, 0);
			return;
		}
		else if (lv_obj_check_type(obj, &lv_msgbox_header_button_class) ||
				 lv_obj_check_type(obj, &lv_msgbox_footer_button_class))
		{
			lv_obj_add_style(obj, getLvglStyles().btn, 0);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, 0);
			lv_obj_add_style(obj, getLvglStyles().transition_delayed, 0);
			lv_obj_add_style(obj, getLvglStyles().pressed, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().transition_normal, LV_STATE_PRESSED);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().bg_color_secondary, LV_STATE_CHECKED);
			lv_obj_add_style(obj, getLvglStyles().disabled, LV_STATE_DISABLED);
			return;
		}

#  endif

#  if LV_USE_SPINBOX
		else if (lv_obj_check_type(obj, &lv_spinbox_class))
		{
			lv_obj_add_style(obj, getLvglStyles().card, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_small, 0);
			lv_obj_add_style(obj, getLvglStyles().outline_primary, LV_STATE_FOCUS_KEY);
			lv_obj_add_style(obj, getLvglStyles().outline_secondary, LV_STATE_EDITED);
			lv_obj_add_style(obj, getLvglStyles().bg_color_primary, LV_PART_CURSOR);
		}
#  endif
#  if LV_USE_TILEVIEW
		else if (lv_obj_check_type(obj, &lv_tileview_class))
		{
			lv_obj_add_style(obj, getLvglStyles().screen, 0);
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
		}
		else if (lv_obj_check_type(obj, &lv_tileview_tile_class))
		{
			lv_obj_add_style(obj, getLvglStyles().scrollbar, LV_PART_SCROLLBAR);
			lv_obj_add_style(obj,
							 getLvglStyles().scrollbar_scrolled,
							 static_cast<int>(LV_PART_SCROLLBAR) | static_cast<int>(LV_STATE_SCROLLED));
		}
#  endif

#  if LV_USE_TABVIEW
		else if (lv_obj_check_type(obj, &lv_tabview_class))
		{
			lv_obj_add_style(obj, getLvglStyles().screen, 0);
			lv_obj_add_style(obj, getLvglStyles().pad_zero, 0);
		}
#  endif

#  if LV_USE_WIN
		else if (lv_obj_check_type(obj, &lv_win_class))
		{
			lv_obj_add_style(obj, getLvglStyles().clip_corner, 0);
		}
#  endif

#  if LV_USE_LED
		else if (lv_obj_check_type(obj, &lv_led_class))
		{
			lv_obj_add_style(obj, getLvglStyles().led, 0);
		}
#  endif

#  if LV_USE_SCALE
		else if (lv_obj_check_type(obj, &lv_scale_class))
		{
			lv_obj_add_style(obj, getLvglStyles().scale, LV_PART_MAIN);
			lv_obj_add_style(obj, getLvglStyles().scale, LV_PART_INDICATOR);
			lv_obj_add_style(obj, getLvglStyles().scale, LV_PART_ITEMS);
		}
#  endif

#  if DEBUG_BORDERS
		if (lv_obj_has_style(lv_screen_active(), s_debugBorders))
		{
			// Add debug borders to any newly created objects
			lv_obj_add_style(obj, s_debugBorders, LV_PART_MAIN);
		}
#  endif
#endif
	}

	void init(lv_display_t* display)
	{
		UI_LOCK();
		LOG_INFO("Initializing themes");

		if (display == NULL)
		{
			display = lv_display_get_default();
		}

		// Initialize uninitialized styles
		for (auto& style : uninitializedStyles())
		{
			style->init();
		}
		uninitializedStyles().clear();

		s_lvglStyles.reset();
		s_componentStyles.reset();

		// Initialize all themes
		for (const auto& theme : themes())
		{
			theme->init();
		}

		// lv_theme_t* baseTheme = lv_theme_default_init(display,
		// 											  lv_palette_main(LV_PALETTE_BLUE),
		// 											  lv_palette_main(LV_PALETTE_RED),
		// 											  LV_THEME_DEFAULT_DARK,
		// 											  &lv_font_montserrat_14);

		/*Initialize the new theme from the current theme*/
		lv_theme_t* th_act = lv_display_get_theme(display);
		s_theme = *th_act;

		/*Set the parent theme and the style apply callback for the new theme*/
		lv_theme_set_parent(&s_theme, th_act);
		lv_theme_set_apply_cb(&s_theme, applyThemeCb);

		/*Assign the new theme to the current display*/
		lv_display_set_theme(display, &s_theme);

		lv_theme_apply(lv_screen_active());

		const Theme* theme = getTheme(StorageHelper::getData<int>(ID_THEME, 0));
		if (theme == nullptr)
		{
			LOG_ERROR("Theme not found, defaulting to theme 0");
			theme = getTheme(0);
			StorageHelper::setData<int>(ID_THEME, 0);
		}
		if (theme != nullptr)
		{
			theme->setThemeActive();
		}

#if DEBUG_BORDERS
		bool debugBordersEnabeled = StorageHelper::getData<bool>(ID_DEBUG_BORDERS, false);
		showDebugBorders(lv_screen_active(), debugBordersEnabeled);
#endif
	}

	const std::vector<Theme*>& getThemes()
	{
		return themes();
	}

	Theme* getCurrentTheme()
	{
		return s_currentTheme;
	}

	const Theme* getTheme(const size_t index)
	{
		if (index < 0 || index >= themes().size())
		{
			LOG_ERROR("Theme with index {:d} not found", index);
			return nullptr;
		}
		return themes()[index];
	}

	const Theme* getThemeByName(const char* name)
	{
		for (const auto& theme : themes())
		{
			if (theme->getName() == name)
			{
				return theme;
			}
		}
		LOG_ERROR("Theme with name {:s} not found", name);
		return nullptr;
	}

	const size_t getThemeCount()
	{
		return themes().size();
	}

	bool refreshCurrentTheme()
	{
		UI_LOCK();
		if (s_currentTheme)
		{
			s_currentTheme->setThemeActive();
			return true;
		}
		else
		{
			LOG_ERROR("No current theme set");
			return false;
		}
	}

	const std::vector<std::string> getThemeNames()
	{
		std::vector<std::string> names;
		for (const auto& theme : themes())
		{
			names.push_back(theme->getName());
		}
		return names;
	}

#if DEBUG_BORDERS
	bool isdebugBorderVisible(lv_obj_t* obj)
	{
		UI_LOCK();
		return lv_obj_has_style(obj, s_debugBorders);
	}

	static void _showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive)
	{
		if (show)
		{
			lv_obj_add_style(obj, s_debugBorders, LV_PART_MAIN, recursive);
		}
		else
		{
			lv_obj_remove_style(obj, s_debugBorders, LV_PART_MAIN, recursive);
		}
	}

	void showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive)
	{
		UI_LOCK();
		_showDebugBorders(obj, show, recursive);
		lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);
	}
#endif

} // namespace UI::Themes

bool lv_obj_has_style(lv_obj_t* obj, const lv_style_t* style)
{
	UI_LOCK();

	for (size_t i = 0; i < obj->style_cnt; i++)
	{
		if (obj->styles[i].style == style)
		{
			return true;
		}
	}
	return false;
}

void lv_obj_add_style(lv_obj_t* obj, const lv_style_t* style, const lv_style_selector_t selector, const bool recursive)
{
	UI_LOCK();
	if (recursive)
	{
		uint32_t childCount = lv_obj_get_child_count(obj);
		for (uint32_t i = 0; i < childCount; ++i)
		{
			lv_obj_t* child = lv_obj_get_child(obj, i);
			lv_obj_add_style(child, style, selector, true);
		}
	}

	// Raw lvgl call
	lv_obj_add_style(obj, style, selector);
}

void lv_obj_remove_style(lv_obj_t* obj,
						 const lv_style_t* style,
						 const lv_style_selector_t selector,
						 const bool recursive)
{
	UI_LOCK();
	if (recursive)
	{
		uint32_t childCount = lv_obj_get_child_count(obj);
		for (uint32_t i = 0; i < childCount; ++i)
		{
			lv_obj_t* child = lv_obj_get_child(obj, i);
			lv_obj_remove_style(child, style, selector, true);
		}
	}

	// Raw lvgl call
	lv_obj_remove_style(obj, style, selector);
}
