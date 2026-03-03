/*
 * test_color.cpp
 *
 *  Created on: 2026-03-02
 */

#include "UI/Styles/Color.h"
#include <gtest/gtest.h>

using namespace UI;

TEST(TestColor, ClampLchValues)
{
	Color color;

	color.setL(-0.5f);
	EXPECT_FLOAT_EQ(color.getL(), 0.0f);
	color.setL(1.5f);
	EXPECT_FLOAT_EQ(color.getL(), 1.0f);

	color.setC(-0.25f);
	EXPECT_FLOAT_EQ(color.getC(), 0.0f);
	color.setC(0.75f);
	EXPECT_FLOAT_EQ(color.getC(), 0.4f);

	color.setH(-10.0f);
	EXPECT_FLOAT_EQ(color.getH(), 0.0f);
	color.setH(720.0f);
	EXPECT_FLOAT_EQ(color.getH(), 360.0f);
}

TEST(TestColor, ConvertFromAndToRgb)
{
	lv_color_t rgb{};
	rgb.red = 64;
	rgb.green = 128;
	rgb.blue = 192;

	Color fromRgb(rgb);
	EXPECT_GE(fromRgb.getL(), 0.0f);
	EXPECT_LE(fromRgb.getL(), 1.0f);
	EXPECT_GE(fromRgb.getC(), 0.0f);
	EXPECT_LE(fromRgb.getC(), 0.4f);
	EXPECT_GE(fromRgb.getH(), 0.0f);
	EXPECT_LE(fromRgb.getH(), 360.0f);

	Color assigned;
	assigned = rgb;
	lv_color_t out = static_cast<lv_color_t>(assigned);

	EXPECT_EQ(out.red, 64);
	EXPECT_EQ(out.green, 128);
	EXPECT_EQ(out.blue, 192);
}
