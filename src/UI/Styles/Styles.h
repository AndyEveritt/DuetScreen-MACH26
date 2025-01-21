/*
 * Styles.h
 *
 *  Created on: 2025-01-21
 *      Author: Andy Everitt
 */

#pragma once

#include "lvgl/lvgl.h"
#include <map>
#include <string>

namespace UI
{
	struct Style
	{
		const char* name;
		lv_style_t style;

		Style(const char* name)
			: name(name)
		{
		}
	};

	class Styles
	{
	  public:
		static Styles& instance()
		{
			static Styles styles;
			return styles;
		}

		void init(lv_display_t* display);
		lv_display_t* getDisplay() { return m_display; }

		void applyTheme(lv_obj_t* obj, const bool recursive = true);
		void removeTheme(lv_obj_t* obj, const bool recursive = true);
		bool hasStyle(lv_obj_t* obj, const lv_style_t* style) const;

#if DEBUG_BORDERS
		bool isdebugBorderVisible(lv_obj_t* obj) const;
		void showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive = true);
#endif

		Style defaultStyle;
		Style btn;
		Style estop;
#if DEBUG_BORDERS
		Style debugBorders;
#endif

	  private:
		Styles();
		static void applyThemeCb(lv_theme_t* th, lv_obj_t* obj);
#if DEBUG_BORDERS
		void _showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive = true);
#endif

		lv_display_t* m_display;
		lv_theme_t* m_theme; // owned by this class
	};
} // namespace UI
