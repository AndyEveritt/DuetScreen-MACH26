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
		ZoneScoped;
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
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/blank.png")

	view.showKeyboard(true);
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/keyboard.png");
}

TEST_F(TestHomeView, BlankControlView)
{
	auto& control = view.getControlView();

	openScreen(&control);
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/move_blank.png");

	control.showTemperatureView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/temperature_blank.png");

	control.showHeightmapView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/heightmap_blank.png");

	control.showFanView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/fan_blank.png");

	control.showMoveView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/move_blank.png");
}

TEST_F(TestHomeView, BlankConsoleView)
{
	openScreen(&view.getConsoleView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/console_view/blank.png")
}

TEST_F(TestHomeView, BlankMacroView)
{
	auto& files = view.getFileView();
	openScreen(&files, false);
	files.setActiveTab(0);
	EXPECT_EQUAL_SCREENSHOT("home_view/files_view/macros_blank.png");
}

TEST_F(TestHomeView, BlankJobView)
{
	auto& files = view.getFileView();
	openScreen(&files, false);
	files.setActiveTab(1);
	EXPECT_EQUAL_SCREENSHOT("home_view/files_view/jobs_blank.png");
}

TEST_F(TestHomeView, BlankStatusView)
{
	view.getDashboard().showStatusTab();
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view/blank.png")
}

#if SIDE_BAR_APP_DRAWER
TEST_F(TestHomeView, BlankMoveView)
{
	openScreen(&view.getMoveView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/move_view/blank.png");
}

TEST_F(TestHomeView, BlankTemperatureView)
{
	openScreen(&view.getTemperatureView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/temperature_view/blank.png")
}

TEST_F(TestHomeView, BlankFanView)
{
	openScreen(&view.getFanView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/fan_view/blank.png");
}

TEST_F(TestHomeView, BlankHeightmapView)
{
	openScreen(&view.getHeightmapView(), false);
	EXPECT_EQUAL_SCREENSHOT("home_view/heightmap_view/blank.png");
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
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/response.png");
}

TEST_F(TestHomeView, SuccessResponse)
{
	view.getPresenter()->newResponse(ResponseType::SUCCESS, "This is a success message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/response_success.png");
}

TEST_F(TestHomeView, WarningResponse)
{
	view.getPresenter()->newResponse(ResponseType::WARNING, "This is a warning message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/response_warning.png");
}

TEST_F(TestHomeView, ErrorResponse)
{
	view.getPresenter()->newResponse(ResponseType::ERROR, "This is an error message from the Duet");
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/response_error.png");
}

TEST_F(TestHomeView, AlertS0)
{
	load_model_data_from_file("tests/object_model/m291/s0.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s0.png");
}

TEST_F(TestHomeView, AlertS1)
{
	load_model_data_from_file("tests/object_model/m291/s1.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s1.png");
}

TEST_F(TestHomeView, AlertS2)
{
	load_model_data_from_file("tests/object_model/m291/s2.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s2.png");
}

TEST_F(TestHomeView, AlertS3)
{
	load_model_data_from_file("tests/object_model/m291/s3.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s3.png");

	load_model_data_from_file("tests/object_model/test_bench/model_move_vn.json");
	load_model_data_from_file("tests/object_model/m291/s3_axis.json");
	view.show();
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s3_axis.png");
}

TEST_F(TestHomeView, AlertS4)
{
	load_model_data_from_file("tests/object_model/m291/s4.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s4.png");

	load_model_data_from_file("tests/object_model/m291/s4_j1.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s4_j1.png");
}

TEST_F(TestHomeView, AlertS5)
{
	load_model_data_from_file("tests/object_model/m291/s5.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5.png");

	load_model_data_from_file("tests/object_model/m291/s5_j1.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_j1.png");

	load_model_data_from_file("tests/object_model/m291/s5_limits.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_limits.png");

	auto input_obj = view.getChildByName("modal_bg.alert.body.input_cont.input");
	ASSERT_NE(input_obj, nullptr);
	input_obj->sendEvent(LV_EVENT_FOCUSED);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_limits_input.png");

	auto& input = static_cast<UI::LvTextarea&>(*input_obj);
	input.setText("-1");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_limits_min.png");

	input.setText("11");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_limits_max.png");

	input.setText("10");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_limits_ok.png");

	input.setText("5.5");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s5_limits_float.png");
}

TEST_F(TestHomeView, AlertS6)
{
	load_model_data_from_file("tests/object_model/m291/s6.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6.png");

	load_model_data_from_file("tests/object_model/m291/s6_j1.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_j1.png");

	load_model_data_from_file("tests/object_model/m291/s6_limits.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_limits.png");

	auto input_obj = view.getChildByName("modal_bg.alert.body.input_cont.input");
	ASSERT_NE(input_obj, nullptr);
	input_obj->sendEvent(LV_EVENT_FOCUSED);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_limits_input.png");

	auto& input = static_cast<UI::LvTextarea&>(*input_obj);
	input.setText("-0.1");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_limits_min.png");

	input.setText("10.1");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_limits_max.png");

	input.setText("10");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_limits_ok.png");

	input.setText("5.5");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s6_limits_float.png");
}

TEST_F(TestHomeView, AlertS7)
{
	load_model_data_from_file("tests/object_model/m291/s7_j1.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s7_j1.png");

	load_model_data_from_file("tests/object_model/m291/s7_limits.json");
	view.getPresenter()->newAlertData(OM::g_currentAlert);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s7_limits.png");

	auto input_obj = view.getChildByName("modal_bg.alert.body.input_cont.input");
	ASSERT_NE(input_obj, nullptr);
	input_obj->sendEvent(LV_EVENT_FOCUSED);
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s7_limits_input.png");

	auto& input = static_cast<UI::LvTextarea&>(*input_obj);
	input.setText("short");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s7_limits_min.png");

	input.setText("too long..........");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s7_limits_max.png");

	input.setText("this is ok");
	EXPECT_EQUAL_SCREENSHOT("home_view/m291/s7_limits_ok.png");
}

class TestHomeViewWithData : public TestHomeView
{
  protected:
	TestHomeViewWithData()
	{
		ZoneScoped;
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

TEST_F(TestHomeViewWithData, Dashboard)
{
	ZoneScoped;
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
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/temperature_graph.png");

	/* Open the tool list numberpad */
	view.getDashboard().getToolList().getTool(0)->getHeater(0)->getChildByName("active")->sendEvent(LV_EVENT_CLICKED,
																									nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/tool_list_numberpad.png");
}

TEST_F(TestHomeViewWithData, SimplePrinterDashboard)
{
	ZoneScoped;
	load_model_data_from_file("tests/object_model/5_axis/model_heat_v.json");
	load_model_data_from_file("tests/object_model/5_axis/model_tools_v.json");

	EXPECT_EQ(OM::Heat::GetHeaterCount(), 1);
	EXPECT_EQ(OM::GetToolCount(), 1);
	EXPECT_EQ(OM::GetBedCount(), 0);
	EXPECT_EQ(OM::GetChamberCount(), 0);

	EXPECT_EQUAL_SCREENSHOT("home_view/dashboard/simple_printer.png");
}

TEST_F(TestHomeViewWithData, ConsoleView)
{
	ZoneScoped;
	openScreen(&view.getConsoleView());

	auto presenter = view.getConsoleView().getPresenter();
	view.getConsoleView().addCommand("M115");
	presenter->newResponse(ResponseType::INFO, "Testing response");
	presenter->newResponse(ResponseType::INFO, "new multi-line response\nline 2");
	presenter->newResponse(ResponseType::SUCCESS, "Success response");
	presenter->newResponse(ResponseType::WARNING, "Warning response");
	presenter->newResponse(ResponseType::ERROR, "Error response");

	presenter->newLogMessage(Log::DebugLevel::Info, Log::log_time_t{}, "Testing log message");

	EXPECT_EQUAL_SCREENSHOT("home_view/console_view/duet_responses.png")

	view.getConsoleView().showCommandList(true, false);
	EXPECT_EQUAL_SCREENSHOT("home_view/console_view/command_list.png");

	view.getConsoleView().showKeyboard(true);
	EXPECT_EQUAL_SCREENSHOT("home_view/console_view/keyboard.png");
}

TEST_F(TestHomeViewWithData, ControlView)
{
	ZoneScoped;
	auto& control = view.getControlView();

	openScreen(&control);
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/move.png");
	{
		auto btn =
			control.getMoveView().getChildByName("central_row.babystep.button_panel.value_list.list.value_btn_1");
		ASSERT_NE(btn, nullptr);

		btn->sendEvent(LV_EVENT_LONG_PRESSED);
		EXPECT_EQUAL_SCREENSHOT("home_view/control_view/move_babystep_long_press.png");
	}

	control.showTemperatureView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/temperature.png");
	{
		auto& temperature = control.getTemperatureView();
		auto input =
			temperature.getChildByName("control_cont.extruder_control.controls.distance_selector.topRow.valueDisplay");
		ASSERT_NE(input, nullptr);
		input->sendEvent(LV_EVENT_CLICKED, nullptr);
		EXPECT_EQUAL_SCREENSHOT("home_view/control_view/temperature_distance_input.png");
	}

	control.showHeightmapView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/heightmap.png");

	control.showFanView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/fan.png");

	control.showMoveView();
	EXPECT_EQUAL_SCREENSHOT("home_view/control_view/move.png");
}

TEST_F(TestHomeViewWithData, MacroView)
{
	auto& files = view.getFileView();
	openScreen(&files, false);
	files.setActiveTab(0);
	EXPECT_EQUAL_SCREENSHOT("home_view/files_view/macros.png")
}

TEST_F(TestHomeViewWithData, SettingsView)
{
	auto& settings = view.getSettingsView();
	openScreen(&settings);
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view/initial.png")

	settings.showGeneralSettings();
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view/general.png")

	settings.showConnectionSettings();
	std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for wifi networks to load
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view/connection.png")

	settings.showDisplaySettings();
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view/display.png")

	settings.showDeveloperSettings();
	EXPECT_EQUAL_SCREENSHOT("home_view/settings_view/developer.png")
}

TEST_F(TestHomeViewWithData, StatusView)
{
	view.getDashboard().showStatusTab();
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view/printing.png")

	UI::LvObj* speed_factor =
		view.getDashboard().getStatusView().getChildByName("print_info.speed_cont.speed_multiplier");
	ASSERT_NE(speed_factor, nullptr);
	speed_factor->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view/speed_factor_numberpad.png")

	UI::closeAllModals();

	UI::LvObj* extrusion_factor =
		view.getDashboard().getStatusView().getChildByName("print_info.flow_cont.flow_multiplier");
	ASSERT_NE(extrusion_factor, nullptr);
	extrusion_factor->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/status_view/extrusion_factor_numberpad.png")
}

#if SIDE_BAR_APP_DRAWER
TEST_F(TestHomeViewWithData, MoveView)
{
	openScreen(&view.getMoveView());
	EXPECT_EQUAL_SCREENSHOT("home_view/move_view/move_view.png")

	load_model_data_from_file("tests/object_model/5_axis/model_move_vn.json");
	view.getMoveView().activate();
	EXPECT_EQUAL_SCREENSHOT("home_view/move_view/5_axis.png")
}

TEST_F(TestHomeViewWithData, TemperatureView)
{
	openScreen(&view.getTemperatureView());
	EXPECT_EQUAL_SCREENSHOT("home_view/temperature_view/temperature_view.png")
}

TEST_F(TestHomeViewWithData, FanView)
{
	openScreen(&view.getFanView());
	EXPECT_EQUAL_SCREENSHOT("home_view/fan_view/fan_view.png")
}

TEST_F(TestHomeViewWithData, HeightmapView)
{
	openScreen(&view.getHeightmapView());
	EXPECT_EQUAL_SCREENSHOT("home_view/heightmap_view/heightmap_view.png")
}

TEST_F(TestHomeViewWithData, FineTuneView)
{
	UI::FineTune& fineTuneView = view.getFineTuneView();
	openScreen(&fineTuneView);
	EXPECT_EQUAL_SCREENSHOT("home_view/fine_tune_view/fine_tune_view.png")

	UI::LvObj* input = fineTuneView.getChildByName("sliders.speed.slider_cont.slider_input");
	ASSERT_NE(input, nullptr);

	input->sendEvent(LV_EVENT_CLICKED, nullptr);
	EXPECT_EQUAL_SCREENSHOT("home_view/fine_tune_view/keyboard.png")
}
#endif
