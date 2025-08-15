/*
 * test_home_view.cpp
 *
 *  Created on: 2025-08-14
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "lv_i18n/lv_i18n.h"
#include "test_utils/UiTestSuite.h"
#include "utils/StorageHelper.h"
#include <gtest/gtest.h>

class HomeViewTest : public UiTestSuite
{
  protected:
	HomeViewTest() {}

	virtual ~HomeViewTest() {}

	UI::HomeView view;
};

TEST_F(HomeViewTest, BlankView)
{
	EXPECT_EQUAL_SCREENSHOT("home_view_blank.png")
}