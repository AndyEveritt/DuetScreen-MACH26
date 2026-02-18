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
	static ColorCtx s_colorCtx = {
		.primaryHue = 245,
		.secondaryHue = 190,
		.chroma = 0,
		.darkMode = true,
		.customizer =
			[](ThemeColors& colors, const ColorCtx&)
		{
			colors.primary.setC(0);
			colors.primary_muted.setC(0);
			colors.secondary.setC(0);
			colors.secondary_muted.setC(0);
		},
	};
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 18, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.normal = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
		.emphasis = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.subdued = {.size = 12, .style = LV_FREETYPE_FONT_STYLE_ITALIC | LV_FREETYPE_FONT_STYLE_WEIGHT(200)},
	};

	static ThemeColors s_colors = createThemeColors(s_colorCtx);

	static DefaultTheme s_greyTheme("grey", s_colors, s_fontConfigs, []([[maybe_unused]] Theme* theme) {});
} // namespace UI::Themes
