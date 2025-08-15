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

TEST_F(TestButton, Basic) {
    EXPECT_EQUAL_SCREENSHOT("button_basic.png");
}

TEST_F(TestButton, WithText) {
    btn.setText("Click Me");
    EXPECT_EQUAL_SCREENSHOT("button_with_text.png");
}