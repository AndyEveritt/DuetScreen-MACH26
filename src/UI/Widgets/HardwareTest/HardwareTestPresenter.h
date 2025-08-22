/*
 * HardwareTestPresenter.h
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class HardwareTest;

	class HardwareTestPresenter : public Presenter<HardwareTest>
	{
	  public:
		PRESENTER_CONSTRUCTOR(HardwareTestPresenter, HardwareTest);

		// Setters
		bool setSerialNumber(std::string_view serial_number);

		// Getters

		// Actions
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

		void getUid();
        void createLogFile();
		bool writeToLogFile(const std::string& message);
        void showNextTouchPoint();
        bool checkTouchCalibration();

	  private:
		std::string m_uid;
		std::string m_serialNumber;
		std::string m_logFile;

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

		enum class TestState
		{
			Start = 0,
			SerialInput,
			TouchCalibration,
			DeadPixelTest,
			MemoryTest,
			WifiTest
		} m_testState = TestState::Start;
	};
} // namespace UI
