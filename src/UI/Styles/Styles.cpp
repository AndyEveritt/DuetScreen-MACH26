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

namespace UI
{
	Styles::Styles()
		: defaultStyle("default")
		, btn("btn")
		, estop("estop")
#if DEBUG_BORDERS
		, debugBorders("debugBorders")
		, m_display(nullptr)
		, m_theme(nullptr)
#endif
	{
	}

	void Styles::init(lv_display_t* display)
	{
		UI_LOCK();
		m_display = display;
		lv_theme_t* baseTheme = lv_theme_default_init(m_display,
													  lv_palette_main(LV_PALETTE_BLUE),
													  lv_palette_main(LV_PALETTE_RED),
													  LV_THEME_DEFAULT_DARK,
													  &lv_font_montserrat_14);

		/*Initialize the new theme from the current theme*/
		lv_theme_t* th_act = lv_display_get_theme(display);
		m_theme = new lv_theme_t;
		*m_theme = *th_act;

		/*Set the parent theme and the style apply callback for the new theme*/
		lv_theme_set_parent(m_theme, th_act);
		lv_theme_set_apply_cb(m_theme, applyThemeCb);

		/*Assign the new theme to the current display*/
		lv_display_set_theme(display, m_theme);

		// m_theme->style.bg->body.main_color = lv_color_hex(0x2E3440);

		/* Init styles */
		lv_style_init(&defaultStyle.style);
		lv_style_init(&btn.style);
		lv_style_init(&estop.style);
#if DEBUG_BORDERS
		lv_style_init(&debugBorders.style);
#endif

		/* debugBorders */
#if DEBUG_BORDERS
		lv_style_set_border_color(&debugBorders.style, lv_color_black());
		lv_style_set_border_width(&debugBorders.style, 2);
		lv_style_set_border_opa(&debugBorders.style, LV_OPA_100);
#endif

		lv_theme_apply(lv_screen_active());

#if DEBUG_BORDERS
		bool debugBordersEnabeled = StorageHelper::getData<bool>(ID_DEBUG_BORDERS, false);
		showDebugBorders(lv_screen_active(), debugBordersEnabeled);
#endif
	}

	void Styles::applyTheme(lv_obj_t* obj, const bool recursive)
	{
		UI_LOCK();
		if (recursive)
		{
			uint32_t childCount = lv_obj_get_child_count(obj);
			for (uint32_t i = 0; i < childCount; ++i)
			{
				lv_obj_t* child = lv_obj_get_child(obj, i);
				applyTheme(child, true);
			}
		}

		applyThemeCb(m_theme, obj);
	}

	void Styles::removeTheme(lv_obj_t* obj, const bool recursive)
	{
		UI_LOCK();
		if (recursive)
		{
			uint32_t childCount = lv_obj_get_child_count(obj);
			for (uint32_t i = 0; i < childCount; ++i)
			{
				lv_obj_t* child = lv_obj_get_child(obj, i);
				removeTheme(child, true);
			}
		}

#if DEBUG_BORDERS
		lv_obj_remove_style(obj, &debugBorders.style, LV_PART_MAIN);
#endif
	}

	void Styles::applyThemeCb(lv_theme_t* th, lv_obj_t* obj)
	{
		UI_LOCK();
		LV_UNUSED(th);

#if DEBUG_BORDERS
		if (Styles::instance().hasStyle(lv_screen_active(), &Styles::instance().debugBorders.style))
		{
			lv_obj_add_style(obj, &Styles::instance().debugBorders.style, LV_PART_MAIN);
		}
#endif
	}

	bool Styles::hasStyle(lv_obj_t* obj, const lv_style_t* style) const
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

#if DEBUG_BORDERS
	bool Styles::isdebugBorderVisible(lv_obj_t* obj) const
	{
		UI_LOCK();
		return hasStyle(obj, &debugBorders.style);
	}

	void Styles::showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive)
	{
		UI_LOCK();
		_showDebugBorders(obj, show, recursive);
		lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);
	}

	void Styles::_showDebugBorders(lv_obj_t* obj, const bool show, const bool recursive)
	{
		UI_LOCK();
		if (recursive)
		{
			uint32_t childCount = lv_obj_get_child_count(obj);
			for (uint32_t i = 0; i < childCount; ++i)
			{
				lv_obj_t* child = lv_obj_get_child(obj, i);
				_showDebugBorders(child, show, true);
			}
		}

		if (show)
		{
			lv_obj_add_style(obj, &debugBorders.style, LV_PART_MAIN);
		}
		else
		{
			lv_obj_remove_style(obj, &debugBorders.style, LV_PART_MAIN);
		}
	}
#endif

} // namespace UI
