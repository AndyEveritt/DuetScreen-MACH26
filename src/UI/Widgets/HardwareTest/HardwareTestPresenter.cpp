/*
 * HardwareTestPresenter.cpp
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#include "HardwareTestPresenter.h"
#include "Debug.h"
#include "HardwareTest.h"
#include <filesystem>
#include <fstream>
#include <regex>

namespace UI
{
#if SIMULATION
#  define FOLDER "./logs/"
#else
#  define FOLDER "/boot/logs/"
#endif

	static std::string runCommand(const std::string& cmd)
	{
		std::string result;
		FILE* pipe = ::popen(cmd.c_str(), "r");
		if (!pipe)
			return result;
		char buffer[256];
		while (fgets(buffer, sizeof(buffer), pipe))
		{
			result.append(buffer);
		}
		::pclose(pipe);
		return result;
	}

	bool HardwareTestPresenter::setSerialNumber(std::string_view serial_number)
	{
		// Validate the serial number (example: must be 10 characters long and alphanumeric)
		std::regex serial_regex("^A[C-Z]-[0-9]{2}-[0-9]{2}_[0-9]{4}$");
		if (!std::regex_match(serial_number.begin(), serial_number.end(), serial_regex))
		{
			return false; // Invalid serial number
		}

		m_serialNumber = serial_number;

		createLogFile();

		// Proceed to the next test
		nextTest();

		return true; // Valid serial number
	}

	void HardwareTestPresenter::startTouchCalibration()
	{
		// Start the touch calibration process
		m_testState = TestState::TouchCalibration;

		auto& touchScreenTest = getView()->getTouchScreenTest();
		getView()->showTest(&touchScreenTest);

		m_touchPointIndex = 0;
		showNextTouchPoint();
	}

	void HardwareTestPresenter::touchCalibrationFinished()
	{
		if (m_testState != TestState::TouchCalibration)
		{
			LOG_ERROR("Touch calibration not in progress");
			return; // Not in calibration mode
		}

		// Finish the touch calibration process
		nextTest();
	}

	void HardwareTestPresenter::showNextTouchPoint()
	{
		if (m_testState != TestState::TouchCalibration)
		{
			LOG_ERROR("Touch calibration not in progress");
			return; // Not in calibration mode
		}

		auto& touchScreenTest = getView()->getTouchScreenTest();
		if (m_touchPointIndex < m_touchPoints.size())
		{

			lv_point_t point = m_touchPoints[m_touchPointIndex].first;
			touchScreenTest.setTouchTargetPosition(point.x, point.y);
		}
		else
		{
			bool passed = checkTouchCalibration();
			std::string result_msg = passed ? "Touch calibration passed.\n\n" : "Touch calibration failed.\n\n";
			writeToLogFile(result_msg);

			for (size_t i = 0; i < m_touchPoints.size(); ++i)
			{
				const lv_point_t& target = m_touchPoints[i].first;
				const lv_point_t& recorded = m_touchPoints[i].second;
				result_msg += fmt::format(
					"Target ({:d}, {:d}) - Recorded ({:d}, {:d})\n", target.x, target.y, recorded.x, recorded.y);
			}
			writeToLogFile("----------------------\n");
			touchScreenTest.showResults(passed, result_msg);
		}
	}

	void HardwareTestPresenter::logTouchEvent(int32_t x, int32_t y)
	{
		if (m_testState != TestState::TouchCalibration)
		{
			LOG_ERROR("Touch calibration not in progress");
			return;
		}

		if (m_touchPointIndex >= m_touchPoints.size())
		{
			// Not in calibration mode
			return;
		}
		const lv_point_t& target = m_touchPoints[m_touchPointIndex].first;
		lv_point_t& recorded = m_touchPoints[m_touchPointIndex].second;
		recorded = {x, y};

		writeToLogFile(
			fmt::format("Target ({:d}, {:d}) - Recorded ({:d}, {:d})", target.x, target.y, recorded.x, recorded.y));

		m_touchPointIndex++;

		showNextTouchPoint();
	}

	bool HardwareTestPresenter::checkTouchCalibration()
	{
		if (m_testState != TestState::TouchCalibration)
		{
			LOG_ERROR("Touch calibration not in progress");
			return false; // Not in calibration mode
		}

		const int32_t tolerance = 50; // pixels
		for (size_t i = 0; i < m_touchPoints.size(); ++i)
		{
			const lv_point_t& target = m_touchPoints[i].first;
			const lv_point_t& recorded = m_touchPoints[i].second;

			if (std::abs(target.x - recorded.x) > tolerance || std::abs(target.y - recorded.y) > tolerance)
			{
				return false; // Calibration failed
			}
		}
		return true; // Calibration passed
	}

	void HardwareTestPresenter::startDeadPixelTest()
	{
		m_testState = TestState::DeadPixelTest;
		m_colorIndex = 0;
		nextColor();
	}

	void HardwareTestPresenter::nextColor()
	{
		if (m_testState != TestState::DeadPixelTest)
		{
			LOG_ERROR("Dead pixel test not in progress");
			return; // Not in dead pixel test mode
		}

		auto& deadPixelTest = getView()->getDeadPixelTest();

		if (m_colorIndex >= m_colors.size())
		{
			nextTest();
		}

		lv_color_t color = m_colors[m_colorIndex].color;
		deadPixelTest.setScreenColour(color.red, color.green, color.blue);
	}

	void HardwareTestPresenter::deadPixelCheckPassed(bool passed)
	{
		if (m_testState != TestState::DeadPixelTest)
		{
			LOG_ERROR("Dead pixel test not in progress");
			return; // Not in dead pixel test mode
		}

		color_test color_test = m_colors[m_colorIndex];
		writeToLogFile(fmt::format("Dead pixel check for color '{:s}' (0x{:02x}{:02x}{:02x}) - {:s}",
								   color_test.name,
								   color_test.color.red,
								   color_test.color.green,
								   color_test.color.blue,
								   passed ? "Passed" : "Failed"));

		m_colorIndex++;
		nextColor();
	}

	void HardwareTestPresenter::testMemory()
	{
		m_testState = TestState::MemoryTest;
		auto& commandTest = getView()->getCommandTest();
		getView()->showTest(&commandTest);
		commandTest.setMessage("Running memory test...");

		writeToLogFile("\n----------------------\n");
		writeToLogFile("Starting memory test...");

		std::thread(
			[this, &commandTest]()
			{
				std::string result;
				std::string cmd =
#if SIMULATION
					"echo 'Memory test simulation output'";
#else
					"nandtest /dev/mtd0";
#endif
				FILE* pipe = ::popen(cmd.c_str(), "r");
				if (!pipe)
					return;
				char buffer[256];
				while (fgets(buffer, sizeof(buffer), pipe))
				{
					result.append(buffer);
					commandTest.setOutput(result);
				}
				::pclose(pipe);

				writeToLogFile(result);

				std::this_thread::sleep_for(std::chrono::seconds(2));
				nextTest();
			})
			.detach();
	}

	void HardwareTestPresenter::testWifi()
	{

		m_testState = TestState::WifiTest;
		auto& commandTest = getView()->getCommandTest();
		getView()->showTest(&commandTest);
		commandTest.setMessage("Running internal WiFi test...");

		writeToLogFile("\n----------------------\n");
		writeToLogFile("Starting WiFi test...");

		std::thread(
			[this, &commandTest]()
			{
				std::string result;
				std::string cmd =
#if SIMULATION
					"echo 'wifi test simulation output'";
#else
					"dmesg | grep mac_addr";
#endif
				FILE* pipe = ::popen(cmd.c_str(), "r");
				if (!pipe)
					return;
				char buffer[256];
				while (fgets(buffer, sizeof(buffer), pipe))
				{
					result.append(buffer);
					commandTest.setOutput(result);
				}
				::pclose(pipe);

				writeToLogFile(result);

				std::this_thread::sleep_for(std::chrono::seconds(2));
				nextTest();
			})
			.detach();
	}

	void HardwareTestPresenter::nextTest()
	{
		switch (m_testState)
		{
		case TestState::Start:
			m_testState = TestState::SerialInput;
			getView()->showTest(&getView()->getSerialInput());
			break;
		case TestState::SerialInput:
			startTouchCalibration();
			break;
		case TestState::TouchCalibration:
			getView()->showTest(&getView()->getDeadPixelTest());
			break;
		case TestState::DeadPixelTest:
			testMemory();
			break;
		case TestState::MemoryTest:
			testWifi();
			break;
		case TestState::WifiTest:
			// All tests completed
			getView()->hide();
			break;
		default:
			LOG_ERROR("Unknown test state");
		}
	}

	void HardwareTestPresenter::getUid()
	{
		std::string uid =
#if SIMULATION
			"\n0x03006200: 0x93406000 0x4c004814 0x01070a31 0x5c4d1c54";
#else
			runCommand("echo 0x03006200,0x0300620F > /sys/class/sunxi_dump/dump; cat /sys/class/sunxi_dump/dump");
#endif

		// Extract and reformat UID to the desired format: 93406000_4c004814_01070a31_5c4d1c54
		std::smatch match;
		std::regex uid_regex(
			R"(0x03006200:\s*(0x[0-9a-fA-F]+)\s*(0x[0-9a-fA-F]+)\s*(0x[0-9a-fA-F]+)\s*(0x[0-9a-fA-F]+))");
		if (std::regex_search(uid, match, uid_regex) && match.size() == 5)
		{
			m_uid = fmt::format("{:s}_{:s}_{:s}_{:s}",
								match[1].str().substr(2),
								match[2].str().substr(2),
								match[3].str().substr(2),
								match[4].str().substr(2));
		}
		else
		{
			LOG_FATAL_THROW("Failed to parse UID");
		}
	}

	void HardwareTestPresenter::createLogFile()
	{
		std::string filePath = fmt::format(FOLDER "{:s}_{:s}.log", m_serialNumber, m_uid);

#if SIMULATION
		std::filesystem::remove(filePath);
#endif

		size_t attempt = 0;
		while (std::filesystem::exists(filePath) && attempt < 1000)
		{
			size_t randNum = 1000 + (std::rand() % 9000);
			filePath = fmt::format(FOLDER "{:s}_{:s}_{:04d}.log", m_serialNumber, m_uid, randNum);
		}

		std::ofstream out(filePath);
		if (!out)
		{
			LOG_FATAL_THROW("Failed to create log file");
		}

		m_logFile = filePath;

		out << "DuetScreen Hardware Test Log - " << m_uid << std::endl;

		writeToLogFile(runCommand("dmesg"));
		writeToLogFile("----------------------\n");
	}

	bool HardwareTestPresenter::writeToLogFile(const std::string& message)
	{
		if (m_logFile.empty())
		{
			LOG_ERROR("Log file not created");
			return false;
		}

		std::ofstream out(m_logFile, std::ios::app);
		if (!out)
		{
			LOG_FATAL_THROW("Failed to open log file");
			return false;
		}
		out << message << std::endl;
		return true;
	}

	void HardwareTestPresenter::onInit()
	{
		getUid();
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		std::filesystem::create_directories(FOLDER);
	}

	void HardwareTestPresenter::onActivate()
	{
		if (isActive())
			return;

		m_serialNumber.clear();

		getView()->updateLayout();
		const int32_t x_res = getView()->getWidth();
		const int32_t y_res = getView()->getHeight();

		const int32_t x_min = 30;
		const int32_t x_max = x_res - 30;
		const int32_t y_min = 30;
		const int32_t y_max = y_res - 30;

		m_touchPoints = {{{x_min, y_min}, {0, 0}},
						 {{x_max, y_min}, {0, 0}},
						 {{x_min, y_max}, {0, 0}},
						 {{x_max, y_max}, {0, 0}},
						 {{x_res / 2, y_res / 2}, {0, 0}}};

		for (size_t i = 0; i < m_colors.size(); ++i)
		{
			m_colors[i].result = false;
		}

		nextTest();
	}
} // namespace UI
