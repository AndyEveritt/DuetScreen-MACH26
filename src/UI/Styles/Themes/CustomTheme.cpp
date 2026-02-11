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
	CustomTheme::CustomTheme(std::string_view name,
							 FontConfigSet fontConfigSet,
							 std::function<void(Theme* theme)> styleOverrides)
		: DefaultTheme(name, ThemeColors(), fontConfigSet, styleOverrides)
		, m_storageKeys{
			  .primaryHue = {fmt::format(PRIMARY_HUE_KEY, name), 245},
			  .secondaryHue = {fmt::format(SECONDARY_HUE_KEY, name), 50},
			  .chroma = {fmt::format(CHROMA_KEY, name), 0.02f},
			  .darkMode = {fmt::format(DARK_MODE_KEY, name), true},
		  }
	{
	}

	void CustomTheme::setColors(uint16_t primaryHue, uint16_t secondaryHue, float chroma, bool darkMode)
	{
		auto colors = createThemeColors(primaryHue, secondaryHue, chroma, darkMode);
		StorageHelper::setData(m_storageKeys.primaryHue, primaryHue);
		StorageHelper::setData(m_storageKeys.secondaryHue, secondaryHue);
		StorageHelper::setData(m_storageKeys.chroma, chroma);
		StorageHelper::setData(m_storageKeys.darkMode, darkMode);
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

		uint16_t primaryHue = StorageHelper::getData(m_storageKeys.primaryHue);
		uint16_t secondaryHue = StorageHelper::getData(m_storageKeys.secondaryHue);
		float chroma = StorageHelper::getData(m_storageKeys.chroma);
		bool darkMode = StorageHelper::getData(m_storageKeys.darkMode);

		setColors(primaryHue, secondaryHue, chroma, darkMode);
	}
} // namespace UI::Themes
