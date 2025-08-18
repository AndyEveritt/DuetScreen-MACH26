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

TEST_F(HomeViewTest, BlankConsoleView)
{
	openScreen(&view.getConsoleView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_console_view_blank.png")
}

TEST_F(HomeViewTest, BlankMoveView)
{
	openScreen(&view.getMoveView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_move_view_blank.png")
}

TEST_F(HomeViewTest, BlankTemperatureView)
{
	openScreen(&view.getTemperatureView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_temperature_view_blank.png")
}

TEST_F(HomeViewTest, BlankFanView)
{
	openScreen(&view.getFanView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_fan_view_blank.png")
}

TEST_F(HomeViewTest, BlankMacroView)
{
	openScreen(&view.getMacroView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_macro_view_blank.png")
}

TEST_F(HomeViewTest, BlankHeightmapView)
{
	openScreen(&view.getHeightmapView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_heightmap_view_blank.png")
}

TEST_F(HomeViewTest, BlankSettingsView)
{
	openScreen(&view.getSettingsView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_settings_view_blank.png")
}

TEST_F(HomeViewTest, BlankStatusView)
{
	openScreen(&view.getStatusView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_status_view_blank.png")
}

class HomeViewWithDataTest : public HomeViewTest
{
  protected:
	HomeViewWithDataTest()
	{
		load_model_data_from_file("tests/object_model/model_boards_v.json");
		load_model_data_from_file("tests/object_model/model_directories_v.json");
		load_model_data_from_file("tests/object_model/model_fans_v.json");
		load_model_data_from_file("tests/object_model/model_heat_v.json");
		load_model_data_from_file("tests/object_model/model_job_vn.json");
		load_model_data_from_file("tests/object_model/model_move_vn.json");
		load_model_data_from_file("tests/object_model/model_network_v.json");
		load_model_data_from_file("tests/object_model/model_sensors_v.json");
		load_model_data_from_file("tests/object_model/model_spindles_v.json");
		load_model_data_from_file("tests/object_model/model_state_vn.json");
		load_model_data_from_file("tests/object_model/model_tools_v.json");
		load_model_data_from_file("tests/object_model/model_volumes_v.json");
		load_model_data_from_file("tests/object_model/model_all_d99f.json");
	}
};

TEST_F(HomeViewWithDataTest, HomeView)
{
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

TEST_F(HomeViewWithDataTest, ConsoleView)
{
	openScreen(&view.getConsoleView(), false);

	auto presenter = view.getConsoleView().getPresenter();
	view.getConsoleView().addCommand("M115");
	presenter->newResponse("Testing response");
	presenter->newResponse("new multi-line response\nline 2");

	presenter->newLogMessage(Log::DebugLevel::Info, Log::log_time_t{}, "Testing log message");

	EXPECT_EQUAL_SCREENSHOT("home_console_view.png")
}

TEST_F(HomeViewWithDataTest, MoveView)
{
	openScreen(&view.getMoveView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_move_view.png")
}

TEST_F(HomeViewWithDataTest, TemperatureView)
{
	openScreen(&view.getTemperatureView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_temperature_view.png")
}

TEST_F(HomeViewWithDataTest, FanView)
{
	openScreen(&view.getFanView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_fan_view.png")
}

TEST_F(HomeViewWithDataTest, MacroView)
{
	openScreen(&view.getMacroView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_macro_view.png")
}

TEST_F(HomeViewWithDataTest, HeightmapView)
{
	openScreen(&view.getHeightmapView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_heightmap_view.png")
}

TEST_F(HomeViewWithDataTest, SettingsView)
{
	openScreen(&view.getSettingsView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_settings_view.png")
}

TEST_F(HomeViewWithDataTest, StatusView)
{
	openScreen(&view.getStatusView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_status_view.png")
}