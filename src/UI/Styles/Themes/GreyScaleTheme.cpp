/*
 * DarkTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "DefaultTheme.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static const uint16_t s_primaryHue = 245;
	static const uint16_t s_secondaryHue = 190;
	static const float s_chroma = 0;
	static const lv_font_t* s_font = LV_FONT_DEFAULT;
	static bool s_darkMode = true;

	static ThemeColors s_colors = createThemeColors(s_primaryHue,
													s_secondaryHue,
													s_chroma,
													s_darkMode,
													[](ThemeColors& colors)
													{
														colors.primary.setC(0);
														colors.primary_muted.setC(0);
														colors.secondary.setC(0);
														colors.secondary_muted.setC(0);
													});

	static DefaultTheme s_greyTheme("theme_grey", s_colors, s_font, s_darkMode, [](Theme* theme) {});
} // namespace UI::Themes
