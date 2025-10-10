/*
 * test_move_view.cpp
 *
 *  Created on: 2025-09-04
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Screens/Move/MoveView.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestMoveView : public UiTestSuite
{
  public:
	TestMoveView() {}

	MoveView move_view{screen};
};

TEST_F(TestMoveView, Basic)
{
	EXPECT_EQUAL_SCREENSHOT("move_view/basic.png");
}

TEST_F(TestMoveView, OpenNumberpad)
{
	load_model_data_from_file("tests/object_model/test_bench/model_move_vn.json");
	LvObj* label = move_view.getChildByName("axis_control.xy_control.x_label");
	ASSERT_NE(label, nullptr);
	label->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("move_view/numberpad.png");
}

TEST_F(TestMoveView, ExtrudeControl)
{
	LvObj* btn = move_view.getChildByName("axis_control.extruder_control.controls.distance_input.list.1");
	ASSERT_NE(btn, nullptr);

	btn->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("move_view/extrude_feedrate_select.png");

	btn->sendEvent(LV_EVENT_LONG_PRESSED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("move_view/extrude_feedrate_select_long.png");
}
