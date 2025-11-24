/*
 * test_extruder_control.cpp
 *
 *  Created on: 2025-11-17
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Widgets/ExtruderControl/ExtruderControl.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestExtruderControl : public UiTestSuite
{
  public:
	TestExtruderControl() {}
};

TEST_F(TestExtruderControl, Basic)
{
	ExtruderControl control("extruder_control", screen);

	control.setSize(400, LV_PCT(80));

	EXPECT_EQUAL_SCREENSHOT("extruder_control/basic.png")
}
