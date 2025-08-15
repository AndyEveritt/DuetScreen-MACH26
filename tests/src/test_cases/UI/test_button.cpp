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
	btn.getIcon().enableRecolor(false);
	EXPECT_EQUAL_SCREENSHOT("button_with_bmp.png");
}

TEST_F(TestButton, SetIconPng)
{
	btn.setIcon(IMAGE_ASSET("examples/example.png"));
	EXPECT_EQUAL_SCREENSHOT("button_with_png.png");
}

TEST_F(TestButton, IconScaling)
{
	btn.hide();

	UI::LvContainer cont("cont", lv_screen_active());
	cont.setSize(LV_PCT(100), LV_PCT(100));
	cont.setFlexFlow(LV_FLEX_FLOW_ROW_WRAP);

	UI::Button btn1("btn1", cont);
	btn1.setIcon(IMAGE_ASSET("examples/example.png"));

	UI::Button btn2("btn2", cont);
	btn2.setIcon(IMAGE_ASSET("examples/example.png"));
	btn2.setWidth(200);

	UI::Button btn3("btn3", cont);
	btn3.setIcon(IMAGE_ASSET("examples/example.png"));
	btn3.setHeight(200);

	UI::Button btn4("btn4", cont);
	btn4.setIcon(IMAGE_ASSET("examples/example.png"));
	btn4.setSize(200, 200);

	UI::Button btn5("btn5", cont, "Button 5");
	btn5.setFlag(LV_OBJ_FLAG_FLEX_IN_NEW_TRACK, true);
	btn5.setIcon(IMAGE_ASSET("examples/example.png"));

	UI::Button btn6("btn6", cont, "Button 6");
	btn6.setIcon(IMAGE_ASSET("examples/example.png"));
	btn6.setWidth(200);

	UI::Button btn7("btn7", cont, "Button 7");
	btn7.setIcon(IMAGE_ASSET("examples/example.png"));
	btn7.setHeight(200);

	UI::Button btn8("btn8", cont, "Button 8");
	btn8.setIcon(IMAGE_ASSET("examples/example.png"));
	btn8.setSize(200, 200);

	EXPECT_EQUAL_SCREENSHOT("button_icon_scaling.png");
}

TEST_F(TestButton, SetIconBadPath)
{
	btn.setIcon(IMAGE_ASSET("bad_path.bmp"));
	EXPECT_EQUAL_SCREENSHOT("button_with_bad_path_icon.png");
}