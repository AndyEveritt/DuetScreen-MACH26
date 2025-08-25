/*
 * HardwareTestPresenter.h
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"
#include <nlohmann/json.hpp>

namespace UI
{
	class HardwareTest;

	enum class TestId
	{
		Start = 0,
		SerialInput,
		TouchCalibration,
		DeadPixelTest,
		MemoryTest,
		WifiTest,
		Finished
	};

	enum class TestState
	{
		NotStarted,
		InProgress,
		Completed,
		Failed
	};

	class TestProcedure
	{
	  public:
		TestProcedure(TestId id,
					  std::function<void(TestProcedure& test)> start_cb,
					  std::function<bool(TestProcedure& test)> finish_cb,
					  std::function<void(TestProcedure& test)> cleanup_cb)
			: id(id)
			, start_cb(start_cb)
			, finish_cb(finish_cb)
			, cleanup_cb(cleanup_cb)
		{
		}

		TestId getId() const { return id; }
		const nlohmann::json& getOutput() const { return output; }
		void start();
		bool finish();
		void cleanup();

		nlohmann::json output; // JSON output for the test

	  private:
		TestId id;
		TestState state = TestState::NotStarted;
		std::function<void(TestProcedure& test)> start_cb;	 // Called when starting the test
		std::function<bool(TestProcedure& test)> finish_cb;	 // Called to finish the test, returns true if successful
		std::function<void(TestProcedure& test)> cleanup_cb; // Called before starting the next test
		bool failed = false;
	};

	class HardwareTestPresenter : public Presenter<HardwareTest>
	{
	  public:
		PRESENTER_CONSTRUCTOR(HardwareTestPresenter, HardwareTest);

		// Setters
		bool setSerialNumber(std::string_view serial_number);

		// Getters

		// Actions
		void testFinished(TestId id);

		void startTouchCalibration();
		void touchCalibrationFinished();
		void logTouchEvent(int32_t x, int32_t y);

		void startDeadPixelTest();
		void nextColor();
		void deadPixelCheckPassed(bool passed);

		void testMemory();
		void testWifi();

		// Observers

	  protected:
		virtual void onInit() override;
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}

		void nextTest();

		void createTestProcedure(TestId id,
								 std::function<void(TestProcedure& test)> start_cb,
								 std::function<bool(TestProcedure& test)> finish_cb,
								 std::function<void(TestProcedure& test)> cleanup_cb = nullptr);
		void getUid();
		void createLogFile();
		bool writeToLogFile(const std::string& message);
		void showNextTouchPoint();
		bool checkTouchCalibration(TestProcedure& test);

	  private:
		std::string m_uid;
		std::string m_serialNumber;
		std::string m_logFile;

		std::vector<TestProcedure> m_tests;
		TestProcedure* m_currentTest = nullptr;
		size_t m_testIndex = 0;

		size_t m_touchPointIndex = 0;
		std::vector<std::pair<lv_point_t, lv_point_t>> m_touchPoints;

		size_t m_colorIndex = 0;
		struct color_test
		{
			lv_color_t color;
			std::string_view name;
			bool result;
		};
		std::vector<color_test> m_colors = {{lv_color_hex(0xFF0000), "Red", false},
											{lv_color_hex(0x00FF00), "Green", false},
											{lv_color_hex(0x0000FF), "Blue", false},
											{lv_color_hex(0xFFFFFF), "White", false}};
	};
} // namespace UI
