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
							 ColorCtx defaultColors,
							 FontConfigSet fontConfigSet,
							 std::function<void(Theme* theme)> styleOverrides)
		: DefaultTheme(name, createThemeColors(defaultColors), fontConfigSet, styleOverrides)
		, m_defaultColors(defaultColors)
		, m_storageKeys{
			  .primaryHue = {fmt::format(PRIMARY_HUE_KEY, name), defaultColors.primaryHue},
			  .secondaryHue = {fmt::format(SECONDARY_HUE_KEY, name), defaultColors.secondaryHue},
			  .chroma = {fmt::format(CHROMA_KEY, name), defaultColors.chroma},
			  .darkMode = {fmt::format(DARK_MODE_KEY, name), defaultColors.darkMode},
		  }
	{
	}

	void CustomTheme::setColors(uint16_t primaryHue, uint16_t secondaryHue, float chroma, bool darkMode)
	{
		auto colors = createThemeColors({.primaryHue = primaryHue,
										 .secondaryHue = secondaryHue,
										 .chroma = chroma,
										 .darkMode = darkMode,
										 .customizer = m_defaultColors.customizer});
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

	void CustomTheme::resetToDefaults()
	{
		setColors(
			m_defaultColors.primaryHue, m_defaultColors.secondaryHue, m_defaultColors.chroma, m_defaultColors.darkMode);
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
