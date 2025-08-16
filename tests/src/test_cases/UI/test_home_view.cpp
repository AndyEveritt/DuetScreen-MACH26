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
	HomeViewTest()
	{
		// Override the singleton so code using HomeView::instance() uses our local view
		UI::HomeView::setInstance(&view);
	}

	virtual ~HomeViewTest()
	{
		// Reset override
		UI::HomeView::setInstance(nullptr);
	}

	UI::HomeView view;
};

TEST_F(HomeViewTest, BlankView){EXPECT_EQUAL_SCREENSHOT("home_view_blank.png")}

TEST_F(HomeViewTest, BlankFanView)
{
	openScreen(&view.getFanView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_fan_view_blank.png")
}

TEST_F(HomeViewTest, BlankHeightmapView)
{
	openScreen(&view.getHeightmapView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_heightmap_view_blank.png")
}

TEST_F(HomeViewTest, HomeView)
{
	load_model_data_from_file("tests/object_model/model_heat_v.json");
	load_model_data_from_file("tests/object_model/model_tools_v.json");
	load_model_data_from_file("tests/object_model/model_sensors_v.json");
	load_model_data_from_file("tests/object_model/model_all_d99f.json");

	EXPECT_EQ(OM::Heat::GetHeaterCount(), 4);
	EXPECT_EQ(OM::GetToolCount(), 4);

	view.show();

	auto sensor = OM::GetAnalogSensorBySlot(0);
	sensor->lastReading = 25.0f;
	for (size_t i = 0; i < 1000; i++)
	{
		view.getPresenter()->tick();
		sensor->lastReading = (int32_t)(sensor->lastReading + 1) % 300;
	}
	EXPECT_EQUAL_SCREENSHOT("home_view.png");
}