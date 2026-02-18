/*
 * DarkTheme.cpp
 *
 *  Created on: 2025-05-21
 *      Author: Andy Everitt
 */

#include "CustomTheme.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI::Themes
{
	static const ColorCtx s_defaultColorCtx = {
		.primaryHue = 245,
		.secondaryHue = 50,
		.chroma = 0.02f,
		.darkMode = true,
	};
	static FontConfigSet s_fontConfigs = {
		.header = {.size = 18, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.normal = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_NORMAL | LV_FREETYPE_FONT_STYLE_WEIGHT(400)},
		.emphasis = {.size = 14, .style = LV_FREETYPE_FONT_STYLE_BOLD | LV_FREETYPE_FONT_STYLE_WEIGHT(700)},
		.subdued = {.size = 12, .style = LV_FREETYPE_FONT_STYLE_ITALIC | LV_FREETYPE_FONT_STYLE_WEIGHT(200)},
	};

	static CustomTheme s_theme("duetscreen", s_defaultColorCtx, s_fontConfigs, [](Theme* /* theme */) {});
} // namespace UI::Themes
