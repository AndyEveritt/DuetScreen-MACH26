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

namespace UI::Themes
{
	static lv_theme_t s_theme;
	static std::vector<Style*> s_uninitializedStyles;
	static std::vector<Theme*> s_themes;

	static Style s_baseStyle("base");
	static Style s_containerStyle("container");
	static Style s_buttonStyle("button");
	static Style s_labelStyle("label");
	static Style s_estopStyle("estop");

#if DEBUG_BORDERS
	static Style s_debugBorders("debugBorders",
								[](lv_style_t* style)
								{
									lv_style_set_border_color(style, lv_color_black());
									lv_style_set_border_width(style, 2);
									lv_style_set_border_opa(style, LV_OPA_100);
								});
#endif

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
			s_uninitializedStyles.push_back(this);
		}
		else
		{
			initFunc(&style);
		}
	}

	Style::Style(const Style& other)
		: name(other.name)
	{
		lv_style_copy(&style, &other.style);
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
		lv_style_copy(&style, &other.style);
		return *this;
	}

	const Style& getBaseStyle()
	{
		return s_baseStyle;
	}

	const Style& getButtonStyle()
	{
		return s_buttonStyle;
	}

	const Style& getEStopStyle()
	{
		return s_estopStyle;
	}

	static bool themeExists(const char* name)
	{
		for (const auto& theme : s_themes)
		{
			if (theme->getName() == name)
			{
				return true;
			}
		}
		return false;
	}

	Theme::Theme(const char* name)
		: m_name(name)
		, m_base("base")
		, m_container("container")
		, m_label("label")
		, m_button("button")
		, m_estop("estop")
	{
		if (themeExists(name))
		{
			LOG_FATAL_THROW("Theme with name {:s} already exists", name);
			return;
		}
		s_themes.push_back(this);
		LOG_INFO("Theme {:s} created", name);
	}

	void Theme::applyTheme() const
	{
		LOG_INFO("Applying theme: {:s}", m_name);
		s_baseStyle = m_base;
		s_containerStyle = m_container;
		s_labelStyle = m_label;
		s_buttonStyle = m_button;
		s_estopStyle = m_estop;

		lv_obj_report_style_change(NULL);
		lv_obj_invalidate(lv_screen_active());
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

		lv_obj_add_style(obj, s_baseStyle, LV_PART_MAIN);

		if (lv_obj_check_type(obj, &lv_obj_class))
		{
			lv_obj_add_style(obj, s_containerStyle, 0);
		}

		if (lv_obj_check_type(obj, &lv_button_class))
		{
			lv_obj_add_style(obj, s_buttonStyle, 0);
		}

		if (lv_obj_check_type(obj, &lv_label_class))
		{
			lv_obj_add_style(obj, s_labelStyle, 0);
		}
#if DEBUG_BORDERS
		if (lv_obj_has_style(lv_screen_active(), s_debugBorders))
		{
			// Add debug borders to any newly created objects
			lv_obj_add_style(obj, s_debugBorders, LV_PART_MAIN);
		}
#endif
	}

	void init(lv_display_t* display)
	{
		UI_LOCK();
		LOG_INFO("Initializing themes");

		// Initialize uninitialized styles
		for (auto& style : s_uninitializedStyles)
		{
			style->init();
		}
		s_uninitializedStyles.clear();

		// Initialize all themes
		for (const auto& theme : s_themes)
		{
			theme->init();
		}

		lv_theme_t* baseTheme = lv_theme_default_init(
			display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, &lv_font_montserrat_14);

		/*Initialize the new theme from the current theme*/
		lv_theme_t* th_act = lv_display_get_theme(display);
		s_theme = *th_act;

		/*Set the parent theme and the style apply callback for the new theme*/
		lv_theme_set_parent(&s_theme, th_act);
		lv_theme_set_apply_cb(&s_theme, applyThemeCb);

		/*Assign the new theme to the current display*/
		lv_display_set_theme(display, &s_theme);

		lv_theme_apply(lv_screen_active());

#if DEBUG_BORDERS
		bool debugBordersEnabeled = StorageHelper::getData<bool>(ID_DEBUG_BORDERS, false);
		showDebugBorders(lv_screen_active(), debugBordersEnabeled);
#endif
	}

	const std::vector<Theme*>& getThemes()
	{
		return s_themes;
	}

	const Theme& getTheme(const size_t index)
	{
		if (index >= s_themes.size())
		{
			LOG_FATAL_THROW("Theme with index {:d} not found", index);
		}
		return *s_themes[index];
	}

	const Theme& getTheme(const char* name)
	{
		for (const auto& theme : s_themes)
		{
			if (theme->getName() == name)
			{
				return *theme;
			}
		}
		LOG_FATAL_THROW("Theme with name {:s} not found", name);
	}

	const size_t getThemeCount()
	{
		return s_themes.size();
	}

	const std::vector<std::string> getThemeNames()
	{
		std::vector<std::string> names;
		for (const auto& theme : s_themes)
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
