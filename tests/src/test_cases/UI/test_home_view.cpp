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

class TestHomeView : public UiTestSuite
{
  protected:
	TestHomeView()
	{
		// Override the singleton so code using HomeView::instance() uses our local view
		UI::HomeView::setInstance(&view);
	}

	virtual ~TestHomeView()
	{
		// Reset override
		UI::HomeView::setInstance(nullptr);
	}

	UI::HomeView view;
};

TEST_F(TestHomeView, BlankView)
{
	EXPECT_EQUAL_SCREENSHOT("home_view_blank.png")

	view.showKeyboard(true);
	EXPECT_EQUAL_SCREENSHOT("home_view_keyboard.png");
}

TEST_F(TestHomeView, BlankConsoleView)
{
	openScreen(&view.getConsoleView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_console_view_blank.png")
}

TEST_F(TestHomeView, BlankMoveView)
{
	openScreen(&view.getMoveView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_move_view_blank.png")
}

TEST_F(TestHomeView, BlankTemperatureView)
{
	openScreen(&view.getTemperatureView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_temperature_view_blank.png")
}

TEST_F(TestHomeView, BlankFanView)
{
	openScreen(&view.getFanView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_fan_view_blank.png")
}

TEST_F(TestHomeView, BlankMacroView)
{
	openScreen(&view.getMacroView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_macro_view_blank.png")
}

TEST_F(TestHomeView, BlankHeightmapView)
{
	openScreen(&view.getHeightmapView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_heightmap_view_blank.png")
}

TEST_F(TestHomeView, BlankSettingsView)
{
	openScreen(&view.getSettingsView(), false);
	view.getSettingsView().getDuetSettingsView().show(true);
	EXPECT_EQUAL_SCREENSHOT("home_settings_view_blank.png")
}

TEST_F(TestHomeView, BlankStatusView)
{
	openScreen(&view.getStatusView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_status_view_blank.png")
}

TEST_F(TestHomeView, AppDrawer)
{
	view.show();
	view.getSideBar().showAppDrawer(true, LV_ANIM_OFF);
	EXPECT_EQUAL_SCREENSHOT("home_view_app_drawer.png");
}

TEST_F(TestHomeView, Response)
{
	view.getPresenter()->newResponse("This is a response message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view_response.png");
}

class TestHomeViewWithData : public TestHomeView
{
  protected:
	TestHomeViewWithData()
	{
		load_model_data_from_file("tests/object_model/test_bench/model_boards_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_directories_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_fans_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_heat_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_job_vn.json");
		load_model_data_from_file("tests/object_model/test_bench/model_move_vn.json");
		load_model_data_from_file("tests/object_model/test_bench/model_network_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_sensors_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_spindles_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_state_vn.json");
		load_model_data_from_file("tests/object_model/test_bench/model_tools_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_volumes_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_all_d99f.json");

		view.show();
	}
};

TEST_F(TestHomeViewWithData, HomeView)
{
	EXPECT_EQ(OM::Heat::GetHeaterCount(), 4);
	EXPECT_EQ(OM::GetToolCount(), 4);

	/* Populate graph with fake sensor data */
	auto sensor = OM::GetAnalogSensorBySlot(0);
	sensor->lastReading = 25.0f;
	for (size_t i = 0; i < 1000; i++)
	{
		view.getPresenter()->tick();
		sensor->lastReading = (int32_t)(sensor->lastReading + 1) % 300;
	}
	EXPECT_EQUAL_SCREENSHOT("home_view.png");

	/* Open the tool list numberpad */
	view.getToolList().getToolListItem(0)->getPresenter()->configureNumberPad(true);
	view.getToolList().showNumberPad();
	EXPECT_EQUAL_SCREENSHOT("home_view_tool_list_numberpad.png");
}

TEST_F(TestHomeViewWithData, ConsoleView)
{
	openScreen(&view.getConsoleView());

	auto presenter = view.getConsoleView().getPresenter();
	view.getConsoleView().addCommand("M115");
	presenter->newResponse("Testing response");
	presenter->newResponse("new multi-line response\nline 2");

	presenter->newLogMessage(Log::DebugLevel::Info, Log::log_time_t{}, "Testing log message");

	EXPECT_EQUAL_SCREENSHOT("home_console_view.png")
}

TEST_F(TestHomeViewWithData, MoveView)
{
	openScreen(&view.getMoveView());
	EXPECT_EQUAL_SCREENSHOT("home_move_view.png")

	load_model_data_from_file("tests/object_model/5_axis/model_move_vn.json");
	view.getMoveView().activate();
	EXPECT_EQUAL_SCREENSHOT("home_move_view_5_axis.png")
}

TEST_F(TestHomeViewWithData, TemperatureView)
{
	openScreen(&view.getTemperatureView());
	EXPECT_EQUAL_SCREENSHOT("home_temperature_view.png")
}

TEST_F(TestHomeViewWithData, FanView)
{
	openScreen(&view.getFanView());
	EXPECT_EQUAL_SCREENSHOT("home_fan_view.png")
}

TEST_F(TestHomeViewWithData, MacroView)
{
	openScreen(&view.getMacroView());
	EXPECT_EQUAL_SCREENSHOT("home_macro_view.png")
}

TEST_F(TestHomeViewWithData, HeightmapView)
{
	openScreen(&view.getHeightmapView());
	EXPECT_EQUAL_SCREENSHOT("home_heightmap_view.png")
}

TEST_F(TestHomeViewWithData, SettingsView)
{
	openScreen(&view.getSettingsView());
	view.getSettingsView().getDuetSettingsView().show(true);
	EXPECT_EQUAL_SCREENSHOT("home_settings_view.png")
}

TEST_F(TestHomeViewWithData, StatusView)
{
	openScreen(&view.getStatusView());
	EXPECT_EQUAL_SCREENSHOT("home_status_view.png")
}

TEST_F(TestHomeViewWithData, FineTuneView)
{
	UI::FineTune& fineTuneView = view.getFineTuneView();
	openScreen(&fineTuneView);
	EXPECT_EQUAL_SCREENSHOT("home_fine_tune_view.png")

	fineTuneView.showKeyboard(true);
	EXPECT_EQUAL_SCREENSHOT("home_fine_tune_view_keyboard.png")
}
