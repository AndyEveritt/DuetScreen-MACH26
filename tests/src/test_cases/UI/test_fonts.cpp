/*
 * test_fonts.cpp
 *
 *  Created on: 2025-10-17
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "test_utils/UiTestSuite.h"
#include <filesystem>
#include <gtest/gtest.h>

using namespace UI;

class TestFonts : public UiTestSuite
{
  public:
	TestFonts() {}
};

static LvLabel createLabel(LvObj& parent,
						   const std::string& name,
						   const std::string& text,
						   const std::filesystem::path& font_path,
						   uint32_t size = 14,
						   lv_freetype_font_style_t style = LV_FREETYPE_FONT_STYLE_NORMAL)
{
	LvLabel label(name, parent);
	label.setText(text);

	if (!font_path.empty())
	{
		if (!std::filesystem::exists(font_path))
		{
			LOG_ERROR("Font file does not exist: {}", font_path.string());
			return label;
		}
		lv_font_t* font =
			lv_freetype_font_create(font_path.string().c_str(), LV_FREETYPE_FONT_RENDER_MODE_BITMAP, size, style);

		label.setStyleFont(font);
	}
	return label;
}

#define FONT_PATH "assets/fonts/"
#define LABEL_TXT "Hello, World!"

TEST_F(TestFonts, LoadFreeTypeFont)
{
	LvLabel label_1("label_1", screen);
	label_1.setText("Hello, World!");

	LvLabel label_libra_24 = createLabel(screen,
										 "label_libra_24",
										 LABEL_TXT " - libra sans regular",
										 FONT_PATH "libra-sans.regular.ttf",
										 24,
										 LV_FREETYPE_FONT_STYLE_NORMAL);

	LvLabel label_libra_14 = createLabel(screen,
										 "label_libra_14",
										 LABEL_TXT " - libra sans regular",
										 FONT_PATH "libra-sans.regular.ttf",
										 14,
										 LV_FREETYPE_FONT_STYLE_NORMAL);

	LvLabel label_berkshire_24 = createLabel(screen,
											 "label_berkshire_24",
											 LABEL_TXT " - berkshire swash regular",
											 FONT_PATH "berkshire-swash.regular.ttf",
											 24,
											 LV_FREETYPE_FONT_STYLE_NORMAL);

	LvLabel label_berkshire_14 = createLabel(screen,
											 "label_berkshire_14",
											 LABEL_TXT " - berkshire swash regular",
											 FONT_PATH "berkshire-swash.regular.ttf",
											 14,
											 LV_FREETYPE_FONT_STYLE_NORMAL);

	EXPECT_EQUAL_SCREENSHOT("fonts/freetype.png");
}