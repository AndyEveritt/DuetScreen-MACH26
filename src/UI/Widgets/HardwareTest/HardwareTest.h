/*
 * HardwareTest.h
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#pragma once

#include "HardwareTestPresenter.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/MessageBox/MessageBox.h"
#include "UI/Core/View.h"

namespace UI
{
	class HardwareTest : public View<HardwareTestPresenter>
	{
	  public:
		HardwareTest(const std::string& name, lv_obj_t* parent);

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
            MessageBox m_messageBox{"message_box", getRoot(), layout_t(0, 0, 50, 50)};

			HardwareTest& m_parent;
		};

		SerialInput& getSerialInput() { return m_serialInput; }
		TouchScreenTest& getTouchScreenTest() { return m_touchScreenTest; }
		DeadPixelTest& getDeadPixelTest() { return m_deadPixelTest; }

		void showTest(LvContainer* test);

	  private:
		void onShow() override;

		SerialInput m_serialInput{*this};
		TouchScreenTest m_touchScreenTest{*this};
		DeadPixelTest m_deadPixelTest{*this};

		std::vector<LvContainer*> m_tests = {&m_serialInput, &m_touchScreenTest, &m_deadPixelTest};
		size_t m_currentTestIndex = 0;
	};
} // namespace UI
