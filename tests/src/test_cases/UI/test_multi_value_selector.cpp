/*
 * test_multi_value_selector.cpp
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Components/Input/MultiValueSelector.h"
#include "test_utils/UiTestSuite.h"
#include <gtest/gtest.h>

using namespace UI;

class TestMultiValueSelector : public UiTestSuite
{
  public:
	TestMultiValueSelector() {}
};

TEST_F(TestMultiValueSelector, Basic)
{
	MultiValueSelector mvs("multiValueSelector", screen);

	EXPECT_EQUAL_SCREENSHOT("multi_value_selector/basic.png");
}