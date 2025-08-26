/*
 * HardwareTest.h
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#pragma once

#include "HardwareTestPresenter.h"
#include "UI/Components/Containers/Row.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/List/List.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Core/View.h"

namespace UI
{
	class HardwareTest : public View<HardwareTestPresenter>
	{
	  public:
		HardwareTest();

		class SerialInput : public LvContainer
		{
		  public:
			SerialInput(HardwareTest& parent);

		  private:
			void onShow() override;

			LvLabel m_warning{"warning", getRoot()};
			TextBox m_serialInput{"serial_input", getRoot()};
			LvKeyboard m_kb{"keyboard", getRoot()};

			HardwareTest& m_parent;
		};

		class TouchScreenTest : public LvContainer
		{
		  public:
			TouchScreenTest(HardwareTest& parent);

			void setTouchTargetPosition(int32_t x, int32_t y);
			void showResults(bool pass, std::string_view message);

		  private:
			static void onTouchEvent(lv_event_t* e);

			void onShow() override;

			LvLabel m_hint{"hint", getRoot()};
			LvImage m_target{"target", getRoot()};
			MessageBox m_messageBox{"message_box", getRoot(), layout_t(0, 0, 50, 50)};

			HardwareTest& m_parent;
		};

		class DeadPixelTest : public LvContainer
		{
		  public:
			DeadPixelTest(HardwareTest& parent);

			void setScreenColour(uint8_t red, uint8_t green, uint8_t blue);
			void confirmWithUser();

		  private:
			static void onTouchEvent(lv_event_t* e);

			void onShow() override;

			LvLabel m_hint{"hint", getRoot()};
			LvObj m_divider{lv_line_create, "divider", getRoot()};
			LvLabel m_fail{"fail", getRoot()};
			LvLabel m_pass{"pass", getRoot()};
			Button m_start{"start", getRoot()};
			MessageBox m_messageBox{"message_box", getRoot(), layout_t(0, 0, 50, 50)};

			HardwareTest& m_parent;
		};

		class CommandTest : public LvContainer
		{
		  public:
			CommandTest(HardwareTest& parent);

			void setMessage(std::string_view message);
			void setOutput(std::string_view output);
			void appendOutput(std::string_view output);

			std::string_view getOutput() const { return m_output.getText(); }

		  protected:
			LvLabel m_message{"message", getRoot()};
			LvTextArea m_output{"output", getRoot()};

			HardwareTest& m_parent;
		};

		class UsbATest : public CommandTest
		{
		  public:
			UsbATest(HardwareTest& parent);

			void showButton(bool show) { m_button.setVisible(show); }

		  private:
			Button m_button{"button", getRoot()};
		};

		class SpeakerTest : public LvContainer
		{
		  public:
			SpeakerTest(HardwareTest& parent);

		  private:
			LvLabel m_label{"label", getRoot()};
			Row m_buttons{"buttons", getRoot()};
			Button m_no{"no", m_buttons};
			Button m_yes{"yes", m_buttons};
			Button m_playAgain{"play_again", getRoot()};

			HardwareTest& m_parent;
		};

		class TestResults : public LvContainer
		{
		  public:
			class TestResult : public ListItem
			{
			  public:
				TestResult(size_t index, lv_obj_t* parent);

				void setName(std::string_view name);
				void setOutput(std::string_view output);
				void setPassed(bool passed);

			  private:
				LvLabel m_name{"name", getRoot()};
				LvTextArea m_output{"output", getRoot()};
			};

			TestResults(HardwareTest& parent);

			void addResult(std::string_view name, std::string_view output, bool passed);
			void clearResults();

		  private:
			LvLabel m_title{"title", getRoot()};
			List<TestResult> m_passed{"passed", getRoot()};
			List<TestResult> m_failed{"failed", getRoot()};

			Row m_buttons{"buttons", getRoot()};
			Button m_restart{"restart", m_buttons};
			Button m_exit{"exit", m_buttons};

			HardwareTest& m_parent;
		};

		SerialInput& getSerialInput() { return m_serialInput; }
		TouchScreenTest& getTouchScreenTest() { return m_touchScreenTest; }
		DeadPixelTest& getDeadPixelTest() { return m_deadPixelTest; }
		CommandTest& getCommandTest() { return m_commandTest; }
		UsbATest& getUsbATest() { return m_usbATest; }
		SpeakerTest& getSpeakerTest() { return m_speakerTest; }

		TestResults& getTestResults() { return m_testResults; }

		void showTest(LvContainer* test);
		void showResults();

	  private:
		void onShow() override;

		SerialInput m_serialInput{*this};
		TouchScreenTest m_touchScreenTest{*this};
		DeadPixelTest m_deadPixelTest{*this};
		CommandTest m_commandTest{*this};
		UsbATest m_usbATest{*this};
		SpeakerTest m_speakerTest{*this};

		TestResults m_testResults{*this};

		std::vector<LvContainer*> m_tests = {
			&m_serialInput, &m_touchScreenTest, &m_deadPixelTest, &m_commandTest, &m_usbATest, &m_speakerTest};
		size_t m_currentTestIndex = 0;
	};
} // namespace UI
