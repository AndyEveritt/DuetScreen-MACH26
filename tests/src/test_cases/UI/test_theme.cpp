/*
 * test_theme.cpp
 *
 *  Created on: 2025-08-19
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/Theme/ThemePreview.h"
#include "UI/Styles/Styles.h"
#include "UI/Styles/Themes/DefaultTheme.h"
#include "test_utils/UiTestSuite.h"
#include "utils/StorageHelper.h"
#include <gtest/gtest.h>

class TestTheme : public UiTestSuite
{
  public:
	TestTheme()
		: themePreview("preview", lv_screen_active())
		, theme("test_theme", colors, font, darkMode, [](UI::Themes::Theme* theme) {})
	{
		themePreview.setSize(LV_PCT(100), LV_PCT(100));

		theme.init();
		theme.setThemeActive();
	}

	~TestTheme()
	{
		const UI::Themes::Theme* default_theme = UI::Themes::getTheme(StorageHelper::getData<int>(ID_THEME, 0));
		if (default_theme == nullptr)
		{
			return;
		}
		default_theme->setThemeActive();
	}

	const uint16_t primaryHue = 245;
	const uint16_t secondaryHue = 50;
	const float chroma = 0.02f;
	const lv_font_t* font = LV_FONT_DEFAULT;
	bool darkMode = true;

	UI::Themes::ThemeColors colors = UI::Themes::createThemeColors(primaryHue, secondaryHue, chroma, darkMode);
	UI::Themes::DefaultTheme theme;

	UI::ThemePreview themePreview;
};

TEST_F(TestTheme, DefaultTheme)
{
	EXPECT_TRUE(UI::Themes::getCurrentTheme() == &theme);
	themePreview.setPrimaryHue(primaryHue);
	themePreview.setSecondaryHue(secondaryHue);
	themePreview.setChroma(chroma);
	themePreview.setDarkMode(darkMode);
	EXPECT_EQUAL_SCREENSHOT("theme_1.png");

	themePreview.setPrimaryHue(25);
	themePreview.setSecondaryHue(150);
	themePreview.setChroma(0.5f);
	themePreview.setDarkMode(false);
	EXPECT_EQUAL_SCREENSHOT("theme_2.png");

	themePreview.setPrimaryHue(300);
	themePreview.setSecondaryHue(50);
	themePreview.setChroma(0.5f);
	themePreview.setDarkMode(true);
	EXPECT_EQUAL_SCREENSHOT("theme_3.png");
}
