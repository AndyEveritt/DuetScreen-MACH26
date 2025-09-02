/*
 * test_messagebox.cpp
 *
 *  Created on: 2025-09-02
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestMessagebox : public UiTestSuite
{
  public:
	TestMessagebox() {}

	MessageBox message_box{"message_box", lv_screen_active(), layout_t(0, 0, 50, 50)};
};

TEST_F(TestMessagebox, Basic)
{
	message_box.setTitle("title");
	EXPECT_EQUAL_SCREENSHOT("message_box_basic.png");
}

TEST_F(TestMessagebox, WithImage)
{
	message_box.setTitle("title");
}

TEST_F(TestMessagebox, HeightSizeContent)
{
	message_box.setTitle("title");
	message_box.setHeight(LV_SIZE_CONTENT);
	EXPECT_EQUAL_SCREENSHOT("message_box_height_size_content.png");
}

TEST_F(TestMessagebox, WidthSizeContent)
{
	message_box.setTitle("title");
	message_box.setWidth(LV_SIZE_CONTENT);
	EXPECT_EQUAL_SCREENSHOT("message_box_width_size_content.png");
}