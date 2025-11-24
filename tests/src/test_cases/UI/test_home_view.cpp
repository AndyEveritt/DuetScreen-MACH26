/*
 * test_home_view.cpp
 *
 *  Created on: 2025-08-14
 *      Author: Andy Everitt
 */

#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
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
	EXPECT_EQUAL_SCREENSHOT("home_view/blank.png")

	view.showKeyboard(true);
	EXPECT_EQUAL_SCREENSHOT("home_view/keyboard.png");
}

TEST_F(TestHomeView, BlankControlView)
{
	auto& control = view.getControlView();

	openScreen(&control);
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_move_blank.png");

	control.showTemperatureView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_temperature_blank.png");

	control.showHeightmapView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_heightmap_blank.png");

	control.showFanView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_fan_blank.png");

	control.showMoveView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_move_blank.png");
}

TEST_F(TestHomeView, BlankConsoleView)
{
	openScreen(&view.getConsoleView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/console_view_blank.png")
}

TEST_F(TestHomeView, BlankMacroView)
{
	openScreen(&view.getMacroView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/macro_view_blank.png");
}

TEST_F(TestHomeView, BlankSettingsView)
{
	openScreen(&view.getSettingsView(), false);
	view.getSettingsView().getDuetSettingsView().show(true);
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view_blank.png")
}

TEST_F(TestHomeView, BlankStatusView)
{
	view.getDashboard().showStatusTab();
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view_blank.png")
}

#if SIDE_BAR_APP_DRAWER
TEST_F(TestHomeView, BlankMoveView)
{
	openScreen(&view.getMoveView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/move_view_blank.png");
}

TEST_F(TestHomeView, BlankTemperatureView)
{
	openScreen(&view.getTemperatureView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/temperature_view_blank.png")
}

TEST_F(TestHomeView, BlankFanView)
{
	openScreen(&view.getFanView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/fan_view_blank.png");
}

TEST_F(TestHomeView, BlankHeightmapView)
{
	openScreen(&view.getHeightmapView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/heightmap_view_blank.png");
}

TEST_F(TestHomeView, AppDrawer)
{
	view.show();
	view.getSideBar().showAppDrawer(true, LV_ANIM_OFF);
	EXPECT_EQUAL_SCREENSHOT("home_view/app_drawer.png");
}
#endif

TEST_F(TestHomeView, Response)
{
	view.getPresenter()->newResponse(ResponseType::INFO, "This is a response message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/response.png");
}

TEST_F(TestHomeView, SuccessResponse)
{
	view.getPresenter()->newResponse(ResponseType::SUCCESS, "This is a success message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/response_success.png");
}

TEST_F(TestHomeView, WarningResponse)
{
	view.getPresenter()->newResponse(ResponseType::WARNING, "This is a warning message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/response_warning.png");
}

TEST_F(TestHomeView, ErrorResponse)
{
	view.getPresenter()->newResponse(ResponseType::ERROR, "This is an error message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/response_error.png");
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
		load_model_data_from_file("tests/object_model/job/model_job_printing_layer_2.json");
		load_model_data_from_file("tests/object_model/test_bench/model_move_vn.json");
		load_model_data_from_file("tests/object_model/test_bench/model_network_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_sensors_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_spindles_v.json");
		load_model_data_from_file("tests/object_model/job/model_state_printing.json");
		load_model_data_from_file("tests/object_model/test_bench/model_tools_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_volumes_v.json");
		load_model_data_from_file("tests/object_model/test_bench/model_all_d99f.json");
		load_model_data_from_file("tests/object_model/rr_filelist/example1.json");

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
		view.getDashboard().getGraph().getPresenter()->tick();
		sensor->lastReading = (int32_t)(sensor->lastReading + 1) % 300;
	}
	EXPECT_EQUAL_SCREENSHOT("home_view/temperature_graph.png");

	/* Open the tool list numberpad */
	view.getDashboard().getToolList().getTool(0)->getHeater(0)->getChildByName("active")->sendEvent(LV_EVENT_CLICKED,
																									nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/tool_list_numberpad.png");
}

TEST_F(TestHomeViewWithData, ConsoleView)
{
	openScreen(&view.getConsoleView());

	auto presenter = view.getConsoleView().getPresenter();
	view.getConsoleView().addCommand("M115");
	presenter->newResponse(ResponseType::INFO, "Testing response");
	presenter->newResponse(ResponseType::INFO, "new multi-line response\nline 2");
	presenter->newResponse(ResponseType::SUCCESS, "Success response");
	presenter->newResponse(ResponseType::WARNING, "Warning response");
	presenter->newResponse(ResponseType::ERROR, "Error response");

	presenter->newLogMessage(Log::DebugLevel::Info, Log::log_time_t{}, "Testing log message");

	EXPECT_EQUAL_SCREENSHOT("home_view/console_view.png")

	view.getConsoleView().showCommandList(true, false);
	EXPECT_EQUAL_SCREENSHOT("home_view/console_view_command_list.png");

	view.getConsoleView().showKeyboard(true);
	EXPECT_EQUAL_SCREENSHOT("home_view/console_view_keyboard.png");
}

TEST_F(TestHomeViewWithData, ControlView)
{
	auto& control = view.getControlView();

	openScreen(&control);
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_move.png");

	control.showTemperatureView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_temperature.png");
	{
		auto& temperature = control.getTemperatureView();
		auto input = temperature.getChildByName("extruder_control.controls.distance_selector.topRow.valueDisplay");
		ASSERT_NE(input, nullptr);
		input->sendEvent(LV_EVENT_CLICKED, nullptr);
		EXPECT_EQUAL_SCREENSHOT("home_view/control_view_temperature_distance_input.png");
	}

	control.showHeightmapView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_heightmap.png");

	control.showFanView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_fan.png");

	control.showMoveView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view_move.png");
}

TEST_F(TestHomeViewWithData, MacroView)
{
	openScreen(&view.getMacroView());
	EXPECT_EQUAL_SCREENSHOT("home_view/macro_view.png")
}

TEST_F(TestHomeViewWithData, SettingsView)
{
	openScreen(&view.getSettingsView());
	view.getSettingsView().getDuetSettingsView().show(true);
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view.png")
}

TEST_F(TestHomeViewWithData, StatusView)
{
	view.getDashboard().showStatusTab();
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view.png")

	UI::LvObj* speed_factor =
		view.getDashboard().getStatusView().getChildByName("print_info.speed_cont.speed_multiplier");
	ASSERT_NE(speed_factor, nullptr);
	speed_factor->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view_speed_factor_numberpad.png")

	UI::closeAllModals();

	UI::LvObj* extrusion_factor =
		view.getDashboard().getStatusView().getChildByName("print_info.flow_cont.flow_multiplier");
	ASSERT_NE(extrusion_factor, nullptr);
	extrusion_factor->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view_extrusion_factor_numberpad.png")
}

#if SIDE_BAR_APP_DRAWER
TEST_F(TestHomeViewWithData, MoveView)
{
	openScreen(&view.getMoveView());
	EXPECT_EQUAL_SCREENSHOT("home_view/move_view.png")

	load_model_data_from_file("tests/object_model/5_axis/model_move_vn.json");
	view.getMoveView().activate();
	EXPECT_EQUAL_SCREENSHOT("home_view/move_view_5_axis.png")
}

TEST_F(TestHomeViewWithData, TemperatureView)
{
	openScreen(&view.getTemperatureView());
	EXPECT_EQUAL_SCREENSHOT("home_view/temperature_view.png")
}

TEST_F(TestHomeViewWithData, FanView)
{
	openScreen(&view.getFanView());
	EXPECT_EQUAL_SCREENSHOT("home_view/fan_view.png")
}

TEST_F(TestHomeViewWithData, HeightmapView)
{
	openScreen(&view.getHeightmapView());
	EXPECT_EQUAL_SCREENSHOT("home_view/heightmap_view.png")
}

TEST_F(TestHomeViewWithData, FineTuneView)
{
	UI::FineTune& fineTuneView = view.getFineTuneView();
	openScreen(&fineTuneView);
	EXPECT_EQUAL_SCREENSHOT("home_view/fine_tune_view.png")

	UI::LvObj* input = fineTuneView.getChildByName("sliders.speed.slider_cont.slider_input");
	ASSERT_NE(input, nullptr);

	input->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/fine_tune_view_keyboard.png")
}
#endif
