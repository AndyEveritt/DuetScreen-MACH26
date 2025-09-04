/*
 * test_lvgl.cpp
 *
 *  Created on: 2025-09-03
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

// using namespace UI;

class TestLvgl : public UiTestSuite
{
  public:
	TestLvgl() {}
};

TEST_F(TestLvgl, FlexSizeContentGrow)
{
	lv_obj_t* cont = lv_obj_create(lv_screen_active());
	lv_obj_set_name(cont, "cont");
	lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
	lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

	lv_obj_t* header = lv_label_create(cont);
	lv_obj_set_name(header, "header");
	lv_label_set_text(header, "header");

	lv_obj_t* item = lv_obj_create(cont);
	lv_obj_set_name(item, "item");
	lv_obj_set_width(item, LV_PCT(100));
	lv_obj_set_flex_grow(item, 1);
	lv_obj_set_style_bg_color(item, lv_color_hex(0x00ff00), 0);
	lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);

	lv_obj_t* footer = lv_label_create(cont);
	lv_obj_set_name(footer, "footer");
	lv_label_set_text(footer, "footer");

	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content.png");

	lv_obj_set_style_min_height(item, 200, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_min_size.png");

	/* The min size of the cont should override the `LV_SIZE_CONTENT` height so item should be visible */
	lv_obj_set_style_min_height(item, 0, LV_PART_MAIN);
	lv_obj_set_style_min_height(cont, 500, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_min_size_cont.png");

	/* item should grow because min size of cont is set but be capped at max size of item */
	lv_obj_set_style_max_height(item, 200, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_max_size.png");

	/* item height should be 0 since min size is not set for item or cont, cont should not be max size since max size is
	 * larger than content */
	lv_obj_set_style_min_height(cont, 0, LV_PART_MAIN);
	lv_obj_set_style_max_height(item, 0, LV_PART_MAIN);
	lv_obj_set_style_max_height(cont, 500, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_max_size_cont.png");

	/* item height should be 0 since min size is not set for item or cont, cont should be max size since max size is
	 * smaller than content */
	lv_obj_set_style_max_height(item, 0, LV_PART_MAIN);
	lv_obj_set_style_max_height(cont, 40, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_max_size_cont2.png");

	lv_obj_set_style_min_height(item, 50, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_max_size_cont3.png");

	lv_obj_set_style_max_height(cont, LV_PCT(70), LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_col_grow_size_content_max_size_cont4.png");
}

TEST_F(TestLvgl, FlexSizeContentGrow2)
{
	lv_obj_t* cont = lv_obj_create(lv_screen_active());
	lv_obj_set_name(cont, "cont");
	lv_obj_set_style_min_width(cont, LV_SIZE_CONTENT, LV_PART_MAIN);
	lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_style_bg_color(cont, lv_color_hex(0xff0000), 0);
	lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

	lv_obj_t* label = lv_label_create(cont);
	lv_obj_set_name(label, "label");
	lv_label_set_text(label, "label");

	lv_obj_t* sub_cont = lv_obj_create(cont);
	lv_obj_set_name(sub_cont, "sub_cont");
	lv_obj_set_height(sub_cont, LV_SIZE_CONTENT);
	lv_obj_set_flex_grow(sub_cont, 1);
	lv_obj_set_flex_flow(sub_cont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_style_bg_color(sub_cont, lv_color_hex(0x00ff00), 0);
	lv_obj_set_style_bg_opa(sub_cont, LV_OPA_COVER, 0);

	for (size_t i = 0; i < 5; i++)
	{
		lv_obj_t* item = lv_label_create(sub_cont);
		lv_obj_set_name(item, "item_#");
		lv_label_set_text(item, fmt::format("item_{}", i).c_str());
		lv_obj_set_style_text_color(item, lv_color_black(), 0);
	}

	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_grow_size_content2.png");

	lv_obj_set_style_max_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_grow_size_content_max_size_content.png");

	for (size_t i = 0; i < 20; i++)
	{
		lv_obj_t* item = lv_label_create(sub_cont);
		lv_obj_set_name(item, "item_#");
		lv_label_set_text(item, fmt::format("item_{}", i).c_str());
		lv_obj_set_style_text_color(item, lv_color_black(), 0);
	}

	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_grow_size_content_max_size_content_wrap.png");
	lv_obj_set_style_max_width(sub_cont, LV_COORD_MAX, LV_PART_MAIN);
	lv_obj_update_layout(sub_cont);
	lv_obj_set_style_max_width(sub_cont, LV_SIZE_CONTENT, LV_PART_MAIN);
	EXPECT_EQUAL_SCREENSHOT("lvgl/flex_grow_size_content_max_size_content_wrap2.png");
}