/*
 * DefaultTheme.h
 *
 *  Created on: 2025-06-02
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Styles/Color.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	struct ThemeColors
	{
		Color bg_dark;
		Color bg;
		Color bg_light;
		Color primary;
		Color primary_muted;
		Color secondary;
		Color secondary_muted;

		Color text;
		Color text_muted;
		Color text_header;

		Color border;
		Color border_muted;
		Color highlight;
		Color shadow;

		Color success;
		Color warning;
		Color error;
	};

	ThemeColors createThemeColors(uint16_t primaryHue,
								  uint16_t secondaryHue,
								  float chroma,
								  bool darkMode,
								  std::function<void(ThemeColors& colors)> customizer = nullptr);

	class DefaultTheme : public Theme
	{
	  public:
		DefaultTheme(std::string_view name,
					 ThemeColors colors,
					 const lv_font_t* font,
					 std::string_view iconFolder,
					 std::function<void(Theme* theme)> styleOverrides = nullptr)
			: Theme(name, iconFolder, styleOverrides)
			, m_colors(colors)
			, m_fontSmall(font)
			, m_fontNormal(font)
			, m_fontLarge(font)
		{
		}

		void updateColors(const ThemeColors& colors);
		const ThemeColors& getColors() const { return m_colors; }

	  protected:
		void onInit() override;

		ThemeColors m_colors;
		const lv_font_t* m_fontSmall;
		const lv_font_t* m_fontNormal;
		const lv_font_t* m_fontLarge;

		lv_style_transition_dsc_t m_draggableTransition;
		lv_style_transition_dsc_t m_draggingTransition;
		lv_style_transition_dsc_t m_dragCompleteTransition;
	};
} // namespace UI::Themes
