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

        void nextColor();
        void deadPixelCheckPassed(bool passed);

		// Observers

	  protected:
		virtual void onInit() override;
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}

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
        std::vector<lv_color_t> m_colors = {
            lv_color_hex(0xFF0000), // Red
            lv_color_hex(0x00FF00), // Green
            lv_color_hex(0x0000FF), // Blue
            lv_color_hex(0xFFFF00), // Yellow
            lv_color_hex(0xFF00FF), // Magenta
            lv_color_hex(0x00FFFF), // Cyan
            lv_color_hex(0xFFFFFF)  // White
        };
	};
} // namespace UI
