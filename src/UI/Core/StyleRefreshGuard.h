/*
 * StyleRefreshGuard.h
 *
 *  Created on: 2026-02-12
 *      Author: Andy Everitt
 */

#pragma once

#include "lvgl/lvgl.h"

namespace UI
{
	/**
	 * @brief RAII guard that disables global LVGL style refresh for its lifetime.
	 *
	 * Uses the nesting-safe counter in LVGL so multiple guards can be active simultaneously
	 * (e.g. when constructing a complex widget tree that itself creates sub-widgets).
	 *
	 * On destruction, re-enables style refresh and triggers a full refresh on the
	 * specified root object and all its children.
	 *
	 * Usage:
	 * @code
	 *     {
	 *         StyleRefreshGuard guard(rootObj);
	 *         // ... construct children, add styles, set layout ...
	 *     } // full style refresh happens here
	 * @endcode
	 */
	class StyleRefreshGuard
	{
	  public:
		explicit StyleRefreshGuard(lv_obj_t* root = nullptr) noexcept
			: m_root(root)
		{
			lv_obj_enable_style_refresh(m_root, false);
		}

		~StyleRefreshGuard()
		{
			lv_obj_enable_style_refresh(m_root, true);
			if (m_root)
			{
				lv_obj_refresh_style(m_root, LV_PART_ANY, LV_STYLE_PROP_ANY);
			}
		}

		StyleRefreshGuard(const StyleRefreshGuard&) = delete;
		StyleRefreshGuard& operator=(const StyleRefreshGuard&) = delete;

	  private:
		lv_obj_t* m_root;
	};
} // namespace UI
