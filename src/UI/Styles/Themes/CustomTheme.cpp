/*
 * CustomTheme.cpp
 *
 *  Created on: 2025-10-24
 *      Author: Andy Everitt
 */

#include "CustomTheme.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "utils/StorageHelper.h"

#define PRIMARY_HUE_KEY "themes:{:s}:primary_hue"
#define SECONDARY_HUE_KEY "themes:{:s}:secondary_hue"
#define CHROMA_KEY "themes:{:s}:chroma"
#define DARK_MODE_KEY "themes:{:s}:dark_mode"

namespace UI::Themes
{
	static const lv_font_t* s_font = LV_FONT_DEFAULT;
	static std::string_view s_iconFolder = "material";

	CustomTheme::CustomTheme(std::string_view name,
							 const lv_font_t* font,
							 std::string_view iconFolder,
							 std::function<void(Theme* theme)> styleOverrides)
		: DefaultTheme(name, ThemeColors(), font, iconFolder, styleOverrides)
	{
	}

	void CustomTheme::setColors(uint16_t primaryHue, uint16_t secondaryHue, float chroma, bool darkMode)
	{
		auto colors = createThemeColors(primaryHue, secondaryHue, chroma, darkMode);
		StorageHelper::setData<uint16_t>(fmt::format(PRIMARY_HUE_KEY, getName()), static_cast<uint16_t>(primaryHue));
		StorageHelper::setData<uint16_t>(fmt::format(SECONDARY_HUE_KEY, getName()),
										 static_cast<uint16_t>(secondaryHue));
		StorageHelper::setData<float>(fmt::format(CHROMA_KEY, getName()), chroma);
		StorageHelper::setData<bool>(fmt::format(DARK_MODE_KEY, getName()), darkMode);

		m_primaryHue = primaryHue;
		m_secondaryHue = secondaryHue;
		m_chroma = chroma;
		m_darkMode = darkMode;

		updateColors(colors);
	}

	void CustomTheme::onInit()
	{
		DefaultTheme::onInit();

		if (m_initialized)
		{
			return;
		}
		m_initialized = true;

		uint16_t primaryHue = StorageHelper::getData<uint16_t>(fmt::format(PRIMARY_HUE_KEY, getName()), 245);
		uint16_t secondaryHue = StorageHelper::getData<uint16_t>(fmt::format(SECONDARY_HUE_KEY, getName()), 50);
		float chroma = StorageHelper::getData<float>(fmt::format(CHROMA_KEY, getName()), 0.02f);
		bool darkMode = StorageHelper::getData<bool>(fmt::format(DARK_MODE_KEY, getName()), true);

		setColors(primaryHue, secondaryHue, chroma, darkMode);
	}

	static CustomTheme s_customTheme("custom", s_font, s_iconFolder, []([[maybe_unused]] Theme* theme) {});
} // namespace UI::Themes
