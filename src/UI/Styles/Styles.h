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

	void init(lv_display_t* display);
	const std::vector<Theme*>& getThemes();
	const Theme& getTheme(const size_t index);
	const Theme& getTheme(const char* name);
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
