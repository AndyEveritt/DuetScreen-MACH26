/*
 * HardwareTest.cpp
 *
 *  Created on: 2025-08-22
 *      Author: Andy Everitt
 */

#include "HardwareTest.h"
#include "Debug.h"

namespace UI
{
	HardwareTest::HardwareTest(const std::string& name, lv_obj_t* parent)
		: View<HardwareTestPresenter>(name, parent)
	{
		setStyleBgColor(lv_color_black());
		setStyleBgOpa(LV_OPA_COVER);

		m_serialInput.setSize(LV_PCT(100), LV_PCT(100));
		m_touchScreenTest.setSize(LV_PCT(100), LV_PCT(100));
		m_deadPixelTest.setSize(LV_PCT(100), LV_PCT(100));
	}

	HardwareTest::SerialInput::SerialInput(HardwareTest& parent)
		: LvContainer("serial_input_container", parent)
		, m_parent(parent)
	{
		setStyleBgColor(lv_color_black());
		setStyleBgOpa(LV_OPA_COVER);

		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_warning.setText("Invalid serial number");
		m_warning.hide();
		m_warning.setWidth(LV_PCT(100));
		m_warning.setAlign(LV_ALIGN_BOTTOM_MID);
		m_serialInput.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_serialInput.setOneLine(true);
		m_serialInput.setFlexGrow(1);
		m_kb.setSize(LV_PCT(100), LV_PCT(50));
		m_kb.setTextArea(&m_serialInput.getTextArea());
		m_kb.setMode(LV_KEYBOARD_MODE_TEXT_UPPER);

		m_kb.addEventCallback(
			[](lv_event_t* e)
			{
				auto* instance = static_cast<HardwareTest::SerialInput*>(lv_event_get_user_data(e));
				std::string_view serial_number = instance->m_serialInput.getText();
				if (!instance->m_parent.getPresenter()->setSerialNumber(serial_number))
				{
					// Handle invalid serial number
					instance->m_warning.show();
				}
			},
			LV_EVENT_READY,
			this);
	}

	void HardwareTest::SerialInput::onShow()
	{
		m_warning.hide(true);
#if DEBUG
		m_serialInput.setText("AC-04-01_0001");
#else
		m_serialInput.setText("");
#endif
		m_serialInput.setPlaceholderText("Enter serial number");
	}

	HardwareTest::TouchScreenTest::TouchScreenTest(HardwareTest& parent)
		: LvContainer("touch_screen_test_container", parent)
		, m_parent(parent)
	{
		setStyleBgColor(lv_palette_main(LV_PALETTE_BLUE));
		setStyleBgOpa(LV_OPA_COVER);

		setFlag(LV_OBJ_FLAG_CLICKABLE, true);
		addEventCallback(onTouchEvent, LV_EVENT_CLICKED, this);

		m_hint.setText("Tap the target");
		m_target.setSize(50, 50);
		m_target.setSrc(IMAGE_ASSET("touch_target.png"));
		m_target.setStyleRecolor(lv_color_white());
		m_target.setStyleRecolorOpa(LV_OPA_COVER);
		m_target.setFlag(LV_OBJ_FLAG_FLOATING, true);

		m_messageBox.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_messageBox.setTitle("Touch Calibration Results");
		m_messageBox.setCancelBtnText("Restart calibration");
		m_messageBox.cancelVisible(true);
		m_messageBox.setOkBtnText("Continue");
		m_messageBox.setCancelCallback([this]() { m_parent.getPresenter()->startTouchCalibration(); });
		m_messageBox.setOkCallback([this]() { m_parent.getPresenter()->touchCalibrationFinished(); });
	}

	void HardwareTest::TouchScreenTest::setTouchTargetPosition(int32_t x, int32_t y)
	{
		// Ensure the target is centered on (x, y)
		const lv_coord_t target_width = m_target.getWidth();
		const lv_coord_t target_height = m_target.getHeight();
		m_target.setPos(x - target_width / 2, y - target_height / 2);
	}

	void HardwareTest::TouchScreenTest::showResults(bool pass, std::string_view message)
	{
		m_messageBox.setText(message);
		m_messageBox.okVisible(pass);
		// m_messageBox.cancelVisible(!pass);
		m_messageBox.show(true);
	}

	void HardwareTest::TouchScreenTest::onTouchEvent(lv_event_t* e)
	{
		auto* instance = static_cast<TouchScreenTest*>(lv_event_get_user_data(e));
		lv_indev_t* indev = lv_event_get_indev(e);

		lv_point_t point;
		lv_indev_get_point(indev, &point);
		instance->m_parent.getPresenter()->logTouchEvent(point.x, point.y);
		return;
	}

	void HardwareTest::TouchScreenTest::onShow()
	{
		m_messageBox.hide();
	}

	HardwareTest::DeadPixelTest::DeadPixelTest(HardwareTest& parent)
		: LvContainer("dead_pixel_test_container", parent)
		, m_parent(parent)
	{
		setStyleBgColor(lv_color_black());
		setStyleBgOpa(LV_OPA_COVER);

		m_hint.setText("Check for dead pixels");

		m_messageBox.setFlag(LV_OBJ_FLAG_FLOATING, true);
		m_messageBox.setTitle("Dead pixel check");
		m_messageBox.cancelVisible(true);
		m_messageBox.okVisible(true);
		m_messageBox.setCancelBtnText("Yes");
		m_messageBox.setOkBtnText("No");
		m_messageBox.setCancelCallback([this]() { m_parent.getPresenter()->deadPixelCheckPassed(false); });
		m_messageBox.setOkCallback([this]() { m_parent.getPresenter()->deadPixelCheckPassed(true); });
		m_messageBox.setText("Are there dead pixels?");

		addEventCallback(onTouchEvent, LV_EVENT_CLICKED, this);
	}

	void HardwareTest::DeadPixelTest::setScreenColour(uint8_t red, uint8_t green, uint8_t blue)
	{
		setStyleBgColor(lv_color_make(red, green, blue));
		m_hint.hide();
	}

	void HardwareTest::DeadPixelTest::confirmWithUser()
	{
		m_messageBox.show();
	}

	void HardwareTest::DeadPixelTest::onTouchEvent(lv_event_t* e)
	{
		auto* instance = static_cast<DeadPixelTest*>(lv_event_get_user_data(e));
		instance->m_parent.getPresenter()->nextColor();
		return;
	}

	void HardwareTest::DeadPixelTest::onShow()
	{
		setStyleBgColor(lv_color_black());
		m_hint.show();
		m_messageBox.hide();
	}

	void HardwareTest::showTest(LvContainer* test)
	{
		for (auto* t : m_tests)
		{
			t->setVisible(t == test, true);
		}
	}

	void HardwareTest::onShow()
	{
		m_serialInput.setVisible(true);
		m_touchScreenTest.setVisible(false);
		m_deadPixelTest.setVisible(false);
	}

} // namespace UI
