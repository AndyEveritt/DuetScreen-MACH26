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
#include <vector>

namespace UI::Themes
{
	struct Style
	{
		const char* name;
		lv_style_t style;

		Style(const char* name);
		Style(const Style&);

		Style& operator=(const Style& other);

		operator lv_style_t*() { return &style; }
		operator const lv_style_t*() const { return &style; }
	};

	const Style& getBaseStyle();
	const Style& getButtonStyle();
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
		Style m_base;
		Style m_container;
		Style m_label;
		Style m_button;
		Style m_estop;

	  private:
		const std::string m_name;
	};

	void initThemes();
	const std::vector<Theme*>& getThemes();
	const Theme& getTheme(const size_t index);
	const Theme& getTheme(const char* name);
	const size_t getThemeCount();
	const std::vector<std::string> getThemeNames();

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
