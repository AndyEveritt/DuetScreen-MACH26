/*
 * HardwareTestPresenter.cpp
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#include "HardwareTestPresenter.h"
#include "Debug.h"
#include "Hardware/Usb.h"
#include "HardwareTest.h"
#include "UI/Screens/Home/HomeView.h"
#include "nameof.hpp"
#include "utils/NetworkHelper.h"
#include "utils/StorageHelper.h"
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

	void TestProcedure::start()
	{
		state = TestState::InProgress;
		// Call the start callback if it exists
		if (start_cb)
		{
			start_cb(*this);
		}
	}
	bool TestProcedure::finish()
	{
		bool success = true;
		if (finish_cb)
		{
			success = finish_cb(*this);
			state = success ? TestState::Completed : TestState::Failed;
			failed = !success;
		}
		else
		{
			state = TestState::Completed;
		}
		return success;
	}
	void TestProcedure::cleanup()
	{
		if (cleanup_cb)
		{
			cleanup_cb(*this);
		}
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
		auto& touchScreenTest = getView()->getTouchScreenTest();
		getView()->showTest(&touchScreenTest);

		m_touchPointIndex = 0;
		showNextTouchPoint();
	}

	void HardwareTestPresenter::touchCalibrationFinished()
	{

		// Finish the touch calibration process
		nextTest();
	}

	void HardwareTestPresenter::showNextTouchPoint()
	{
		if (m_touchPointIndex >= m_touchPoints.size())
		{
			testFinished(TestId::TouchCalibration);
			return;
		}

		auto& touchScreenTest = getView()->getTouchScreenTest();
		lv_point_t point = m_touchPoints[m_touchPointIndex].first;
		touchScreenTest.setTouchTargetPosition(point.x, point.y);
	}

	void HardwareTestPresenter::logTouchEvent(int32_t x, int32_t y)
	{
		if (m_touchPointIndex >= m_touchPoints.size())
		{
			// Not in calibration mode
			return;
		}

		if (m_currentTest == nullptr || m_currentTest->getId() != TestId::TouchCalibration)
		{
			// Not in calibration mode
			return;
		}

		const lv_point_t& target = m_touchPoints[m_touchPointIndex].first;

		m_currentTest->output["touch_point"][m_touchPointIndex]["target"] = {{"x", target.x}, {"y", target.y}};
		m_currentTest->output["touch_point"][m_touchPointIndex]["recorded"] = {{"x", x}, {"y", y}};

		lv_point_t& recorded = m_touchPoints[m_touchPointIndex].second;
		recorded = {x, y};

		m_touchPointIndex++;

		showNextTouchPoint();
	}

	bool HardwareTestPresenter::checkTouchCalibration(TestProcedure& test)
	{
		const int32_t tolerance = 50; // pixels
		bool success = true;
		for (size_t i = 0; i < m_touchPoints.size(); ++i)
		{
			const lv_point_t& target = m_touchPoints[i].first;
			const lv_point_t& recorded = m_touchPoints[i].second;

			test.output["touch_point"][i] = {
				{"target", {{"x", target.x}, {"y", target.y}}},
				{"recorded", {{"x", recorded.x}, {"y", recorded.y}}},
			};

			if (std::abs(target.x - recorded.x) > tolerance || std::abs(target.y - recorded.y) > tolerance)
			{
				success = false;
			}
		}
		return success;
	}

	void HardwareTestPresenter::startDeadPixelTest()
	{
		m_colorIndex = 0;
		nextColor();
	}

	void HardwareTestPresenter::nextColor()
	{
		auto& deadPixelTest = getView()->getDeadPixelTest();

		if (m_colorIndex >= m_colors.size())
		{
			testFinished(TestId::DeadPixelTest);
		}

		lv_color_t color = m_colors[m_colorIndex].color;
		deadPixelTest.setScreenColour(color.red, color.green, color.blue);
	}

	void HardwareTestPresenter::deadPixelCheckPassed(bool passed)
	{
		if (m_currentTest == nullptr || m_currentTest->getId() != TestId::DeadPixelTest)
		{
			// Not in dead pixel test
			return;
		}

		color_test color_test = m_colors[m_colorIndex];
		LOG_INFO("Dead pixel check for color '{:s}' (0x{:02x}{:02x}{:02x}) - {:s}",
				 color_test.name,
				 color_test.color.red,
				 color_test.color.green,
				 color_test.color.blue,
				 passed ? "Passed" : "Failed");

		m_currentTest->output["color_test"][m_colorIndex] = {
			{"color",
			 fmt::format("0x{:02x}{:02x}{:02x}", color_test.color.red, color_test.color.green, color_test.color.blue)},
			{"name", color_test.name},
			{"result", passed},
		};

		m_colorIndex++;
		nextColor();
	}

	void HardwareTestPresenter::testMemory()
	{
		auto& commandTest = getView()->getCommandTest();
		getView()->showTest(&commandTest);
		commandTest.setMessage("Running memory test...");

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

				m_currentTest->output["result"] = result;

				std::this_thread::sleep_for(std::chrono::seconds(2));
				testFinished(TestId::MemoryTest);
			})
			.detach();
	}

	static std::string extractMacAddress(const std::string& input)
	{
		std::regex mac_regex("link/ether ((?:[0-9a-fA-F]{2}[:-]){5}(?:[0-9a-fA-F]{2}))");
		std::smatch match;
		if (std::regex_search(input, match, mac_regex))
		{
			return match.str(1);
		}
		return "";
	}

	void HardwareTestPresenter::testWifi()
	{
		auto& commandTest = getView()->getCommandTest();
		getView()->showTest(&commandTest);
		commandTest.setMessage("Running internal WiFi test...");

		commandTest.setOutput("Setting USB-C MUX to use internal WiFi...\n");
		int usb_mode = StorageHelper::getData(ID_USB_MODE, 0);
		m_currentTest->output["original_usb_mode"] = usb_mode;

		HomeView::instance().getSettingsView().getPresenter()->setUsbMode(UsbMode::InternalWiFi);

		commandTest.appendOutput("Enabling internal WiFi...\n");
		NetworkHelper::enable(true);

		std::thread(
			[this, &commandTest]()
			{
				/* Wait for wifi to be enabled */
				std::this_thread::sleep_for(std::chrono::seconds(5));

				std::string result;
				std::string cmd =
#if SIMULATION
					"echo '1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000\n"
					"    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00\n"
					"2: wlan0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq qlen 1000\n"
					"    link/ether 00:e0:20:2f:87:9d brd ff:ff:ff:ff:ff:ff'";
#else
					"ip link";
#endif
				FILE* pipe = ::popen(cmd.c_str(), "r");
				if (!pipe)
					return;
				char buffer[256];
				while (fgets(buffer, sizeof(buffer), pipe))
				{
					result.append(buffer);
					commandTest.appendOutput(buffer);
				}
				::pclose(pipe);

				m_currentTest->output["result"] = result;
				m_currentTest->output["mac_address"] = extractMacAddress(result);

				std::this_thread::sleep_for(std::chrono::seconds(2));
				testFinished(TestId::WifiTest);
			})
			.detach();
	}

	void HardwareTestPresenter::testUsbA()
	{
		if (m_currentTest == nullptr || m_currentTest->getId() != TestId::UsbATest)
		{
			// Not in USB-A test
			LOG_ERROR("Not in USB-A test");
			return;
		}

		getView()->showTest(&getView()->getUsbATest());

		if (!m_usbMountPath.empty())
		{
			usbADeviceConnected();
		}
	}

	void HardwareTestPresenter::usbADeviceConnected()
	{
		if (m_currentTest == nullptr || m_currentTest->getId() != TestId::UsbATest)
		{
			// Not in USB-A test
			return;
		}

		std::string result = runCommand("lsusb");
		LOG_INFO("USB-A device connected:\n{:s}", result);
		m_currentTest->output["result"]["after_device_connection"] = result;

		if (m_usbMountPath.empty())
		{
			LOG_ERROR("USB-A mount path is empty");
			m_currentTest->output["result"]["write_successful"] = false;
			m_currentTest->output["result"]["error_message"] = "No USB drive detected";
			testFinished(TestId::UsbATest);
			return;
		}

		std::string file = fmt::format("{:s}/test.txt", m_usbMountPath);
		std::ofstream out(file);
		if (!out)
		{
			LOG_ERROR("Failed to open log file");
			writeToLogFile("Failed to open log file");
			m_currentTest->output["result"]["write_successful"] = false;
			m_currentTest->output["result"]["error_message"] = "Failed to open log file";
			testFinished(TestId::UsbATest);
			return;
		}

		writeToLogFile(fmt::format("Writing test file to USB-A drive: {:s}", file));

		std::string text = fmt::format("DuetScreen USB-A test file\n{:s}\n", m_uid);
		out << text;
		out.close();
		m_currentTest->output["result"]["write_successful"] = true;
		m_currentTest->output["result"]["written_contents"] = text;

		std::string contents;
		USB::ReadFileContents(file, contents);

		m_currentTest->output["result"]["read_contents"] = contents;

		testFinished(TestId::UsbATest);
	}

	void HardwareTestPresenter::restartTests()
	{
		m_serialNumber.clear();
		m_testIndex = 0;

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

		getView()->showTest(&getView()->getSerialInput());
	}

	void HardwareTestPresenter::testFinished(TestId id)
	{
		if (m_currentTest == nullptr)
		{
			LOG_ERROR("Unexpected test finished: no current test");
			return;
		}

		if (m_currentTest->getId() != id)
		{
			LOG_ERROR("Unexpected test finished: expected {:s}, got {:s}",
					  nameof::nameof_enum(m_currentTest->getId()),
					  nameof::nameof_enum(id));
			return;
		}

		bool passed = m_currentTest->finish();
		writeToLogFile(fmt::format("Test '{:s}' {:s}:\n{:s}",
								   nameof::nameof_enum(id),
								   passed ? "passed" : "failed",
								   m_currentTest->getOutput().dump(4)));

		m_currentTest->cleanup();

		m_testIndex++;
		nextTest();
	}

	void HardwareTestPresenter::nextTest()
	{
		if (m_testIndex >= m_tests.size())
		{
			auto& testResults = getView()->getTestResults();
			testResults.clearResults();

			for (size_t i = 0; i < m_tests.size(); ++i)
			{
				auto& test = m_tests[i];
				testResults.addResult(nameof::nameof_enum(test.getId()), test.getOutput().dump(4), !test.getFailed());
			}
			getView()->showResults();
			return;
		}

		TestProcedure* test = &m_tests[m_testIndex];
		m_currentTest = test;
		writeToLogFile("\n----------------------\n");
		writeToLogFile(fmt::format("Starting test '{:s}'", nameof::nameof_enum(test->getId())));
		test->start();
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
	}

	bool HardwareTestPresenter::writeToLogFile(const std::string& message)
	{
		if (m_logFile.empty())
		{
			LOG_ERROR("Log file not created");
			return false;
		}

		LOG_INFO("{:s}", message);
		std::ofstream out(m_logFile, std::ios::app);
		if (!out)
		{
			LOG_FATAL_THROW("Failed to open log file");
			return false;
		}
		out << message << std::endl;
		return true;
	}

	void HardwareTestPresenter::createTestProcedure(TestId id,
													std::function<void(TestProcedure& test)> start_cb,
													std::function<bool(TestProcedure& test)> finish_cb,
													std::function<void(TestProcedure& test)> cleanup_cb)
	{
		m_tests.emplace_back(id, start_cb, finish_cb, cleanup_cb);
	}

	void HardwareTestPresenter::onInit()
	{
		getUid();
		std::srand(static_cast<unsigned int>(std::time(nullptr)));
		std::filesystem::create_directories(FOLDER);

		USB::UsbMonitor::getInstance().registerCallback(
			[this](const std::string& path, bool mounted)
			{
				if (mounted)
				{
					m_usbMountPath = path;
				}
				else
				{
					const auto& mounts = USB::UsbMonitor::getInstance().getMountedDrives();
					if (mounts.empty())
					{
						LOG_INFO("USB drive unmounted");
						m_usbMountPath.clear();
						return;
					}
					m_usbMountPath = mounts.front();
				}
				LOG_INFO("USB drive mounted at: {:s}", m_usbMountPath);
				usbADeviceConnected();
			},
			true);

		/* Create test procedures */
		createTestProcedure(
			TestId::TouchCalibration,
			[this](TestProcedure& test) { startTouchCalibration(); },
			[this](TestProcedure& test) { return checkTouchCalibration(test); });

		createTestProcedure(
			TestId::DeadPixelTest,
			[this](TestProcedure& test) { getView()->showTest(&getView()->getDeadPixelTest()); },
			[this](TestProcedure& test)
			{
				for (size_t i = 0; i < test.output["color_test"].size(); ++i)
				{
					if (!test.output["color_test"][i]["result"].get<bool>())
					{
						return false;
					}
				}
				return true;
			});

		createTestProcedure(
			TestId::MemoryTest,
			[this](TestProcedure& test) { testMemory(); },
			[this](TestProcedure& test)
			{
				std::string result = test.output["result"].get<std::string>();

				// Check `result` for `Finished pass 1 successfully`
				return result.find("Finished pass 1 successfully") != std::string::npos;
			});

		createTestProcedure(
			TestId::WifiTest,
			[this](TestProcedure& test) { testWifi(); },
			[this](TestProcedure& test)
			{
				std::string mac_addr = test.output["mac_address"].get<std::string>();

				// Check `result` for `Finished pass 1 successfully`
				return !mac_addr.empty();
			},
			[this](TestProcedure& test)
			{
				// Restore original USB mode
				int original_usb_mode = test.output["original_usb_mode"].get<int>();
				HomeView::instance().getSettingsView().getPresenter()->setUsbMode(
					static_cast<UsbMode>(original_usb_mode));
			});

		createTestProcedure(
			TestId::UsbATest,
			[this](TestProcedure& test) { testUsbA(); },
			[this](TestProcedure& test)
			{
				bool write_successful = test.output["result"]["write_successful"].get<bool>();

				std::string written_contents;
				std::string read_contents;
				if (test.output["result"].contains("written_contents"))
					written_contents = test.output["result"]["written_contents"].get<std::string>();
				if (test.output["result"].contains("read_contents"))
					read_contents = test.output["result"]["read_contents"].get<std::string>();
				return write_successful && !written_contents.empty() && written_contents == read_contents;
			});
	}

	void HardwareTestPresenter::onActivate()
	{
		if (isActive())
			return;

		restartTests();
	}
} // namespace UI
