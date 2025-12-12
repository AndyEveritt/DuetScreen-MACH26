/*
 * test_fonts.cpp
 *
 *  Created on: 2025-10-17
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Styles/Font.h"
#include "test_utils/UiTestSuite.h"
#include <filesystem>
#include <gtest/gtest.h>

using namespace UI;

class TestFonts : public UiTestSuite
{
  public:
	TestFonts() {}
};

TEST_F(TestFonts, FontManager)
{
	FontManager::Font font1 = FontManager::createFont("OpenSans", 14, LV_FREETYPE_FONT_STYLE_NORMAL);
	ASSERT_NE(font1.get(), nullptr);

	ASSERT_EQ(FontManager::createFont("bad_font_name", 14).get(), nullptr);
	ASSERT_EQ(FontManager::createFont("", 14).get(), nullptr);

	FontManager::Font fallback_font = FontManager::createFont("bad_font_name,OpenSans", 14);
	ASSERT_NE(fallback_font.get(), nullptr);
}