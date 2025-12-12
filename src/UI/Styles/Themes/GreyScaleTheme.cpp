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
	static bool s_darkMode = true;
	static std::string_view s_iconFolder = "material";
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 18, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.normal = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
		.emphasis = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.subdued = {.size = 12, .style = LV_FREETYPE_FONT_STYLE_ITALIC | LV_FREETYPE_FONT_STYLE_WEIGHT(200)},
	};

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

	static DefaultTheme s_greyTheme(
		"grey", s_colors, s_fontConfigs, s_iconFolder, []([[maybe_unused]] Theme* theme) {});
} // namespace UI::Themes
