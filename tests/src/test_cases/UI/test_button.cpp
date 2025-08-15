/*
 * test_button.cpp
 *
 *  Created on: 2025-08-15
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/Button/Button.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

class TestButton : public UiTestSuite
{
  public:
	TestButton()
		: btn("test_button", lv_screen_active())
	{
	}

	UI::Button btn;
};

TEST_F(TestButton, Basic)
{
	EXPECT_EQUAL_SCREENSHOT("button_basic.png");
}

TEST_F(TestButton, WithText)
{
	btn.setText("Click Me");
	EXPECT_EQUAL_SCREENSHOT("button_with_text.png");
}

TEST_F(TestButton, SetIconBmp)
{
	btn.setIcon(IMAGE_ASSET("examples/example.bmp"));
	EXPECT_EQUAL_SCREENSHOT("button_with_bmp.png");
}

TEST_F(TestButton, SetIconPng)
{
	btn.setIcon(IMAGE_ASSET("examples/example.png"));
	EXPECT_EQUAL_SCREENSHOT("button_with_png.png");
}

TEST_F(TestButton, SetIconBadPath)
{
	btn.setIcon(IMAGE_ASSET("bad_path.bmp"));
	EXPECT_EQUAL_SCREENSHOT("button_with_bad_path_icon.png");
}