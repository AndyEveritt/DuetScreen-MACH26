/*
 * Font.h
 *
 *  Created on: 2025-10-23
 *      Author: Andy Everitt
 */

#pragma once

#include "lvgl/lvgl.h"
#include <string_view>
#include <vector>

namespace UI::FontManager
{
	class Font
	{
	  public:
		Font(const lv_font_t* font = LV_FONT_DEFAULT);
		~Font();

		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

		Font(Font&& other) noexcept;
		Font& operator=(Font&& other) noexcept;

		const lv_font_t* get() const { return m_font; }

	  private:
		const lv_font_t* m_font = nullptr;
	};

	void init();
	Font createFont(const std::string& name, uint32_t size, uint32_t style = LV_FREETYPE_FONT_STYLE_NORMAL);

	const std::vector<std::string>& getLoadedFontNames();
	const std::string& getActiveTypefaceName();
	void setActiveTypeface(const std::string& name);
} // namespace UI::FontManager
