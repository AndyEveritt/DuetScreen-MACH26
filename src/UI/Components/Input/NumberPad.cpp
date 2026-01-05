/*
 * NumberPad.cpp
 *
 *  Created on: 2025-01-17
 *      Author: Andy Everitt
 */

#include "NumberPad.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include <string>

namespace UI
{
#define LV_KEYBOARD_CTRL_BUTTON_FLAGS                                                                                  \
	(LV_BUTTONMATRIX_CTRL_NO_REPEAT | LV_BUTTONMATRIX_CTRL_CLICK_TRIG | LV_BUTTONMATRIX_CTRL_CHECKED)

	// clang-format off
	static const char* btnm_map[] = {
		"1", "2", "3", "\n",
		"4", "5", "6", "\n",
		"7", "8", "9", "\n",
		"+/-", "0", ".", "\n",
		LV_SYMBOL_BACKSPACE, LV_SYMBOL_OK, ""};

  static const lv_buttonmatrix_ctrl_t btnm_ctrl_map[] = {
	  LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_1,
	  LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_1,
	  LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_1,
	  LV_BUTTONMATRIX_CTRL_CHECKED, LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_CHECKED,
	  LV_BUTTONMATRIX_CTRL_CHECKED, LV_BUTTONMATRIX_CTRL_CHECKED,
	};
	// clang-format on

	NumberPad::NumberPad(const std::string& name, LvObj& parent, layout_t layout)
		: LvContainer(name, parent, layout)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		// Overall layout
		m_header.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_textCont.setFlexGrow(0);						  // Don't grow the text container
		m_textCont.setSize(LV_PCT(100), LV_SIZE_CONTENT); // LV_SIZE_CONTENT is bugged
		m_btnMatrix.setFlexGrow(1);
		// m_btnMatrix.setWidth(LV_PCT(100));
		m_btnMatrix.setWidth(LV_PCT(100));
		m_btnMatrix.setMinWidth(100);

		// setHeader(""); // Set an empty header by default

		// Text Entry Layout
		m_textCont.addStyle(Themes::getLvglStyles().no_border);
		m_textBox.addStyle(Themes::getLvglStyles().no_border);
		m_textCont.addStyle(Themes::getLvglStyles().pad_small);
		m_textCont.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_textBox.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_textCont.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_textCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		m_textBox.setFlexGrow(1);
		// m_textBox.setHeight(LV_SIZE_CONTENT);
		m_textBox.setHeight(LV_SIZE_CONTENT);
		// m_textBox.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_clearBtn.setSize(LV_PCT(20), LV_PCT(100));
		m_clearBtn.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_textBox.setOneLine(true);
		m_textBox.setAcceptedChars("0123456789.-");

		// Button Matrix Layout
		m_btnMatrix.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_btnMatrix.setFlag(LV_OBJ_FLAG_CLICK_FOCUSABLE, false); // to keep the text area focused on button clicks
		m_btnMatrix.addEventCallback(onReadyEventHandler, LV_EVENT_READY, this);
		m_btnMatrix.addEventCallback(btnmEventHandler, LV_EVENT_VALUE_CHANGED, &m_textBox);
		m_btnMatrix.setMap(btnm_map);
		m_btnMatrix.setCtrlMap(btnm_ctrl_map);

		m_textBox.setUserData(this);
		m_textBox.setText("");

		m_clearBtn.addClickedCallback(clearBtnEventHandler, this);

		validateInput();
	}

	NumberPad::NumberPad(const std::string& name, LvObj& parent, layout_t layout, const NumberPadConfig& config)
		: NumberPad(name, parent, layout)
	{
		// Configure
		setMinValue(config.minValue);
		setMaxValue(config.maxValue);
		validateInput();
	}

	bool NumberPad::back()
	{
		// This custom back method prevents the previous screen from reopening
		close();
		return true;
	}

	void NumberPad::clear()
	{
		m_textBox.setText("");
		validateInput();
	}

	void NumberPad::close()
	{
		closeScreen(this, false);
	}

	void NumberPad::confirm()
	{
		if (validateInput())
		{
			// Call the confirm callback
			m_btnMatrix.sendEvent(LV_EVENT_READY, this);
		}
	}

	void NumberPad::setHeader(std::string_view text)
	{
		m_header.setFlag(LV_OBJ_FLAG_HIDDEN, text.empty());
		m_header.setText(text);
	}

	void NumberPad::setMinValue(float value)
	{
		m_minValue = value;

		m_textBox.setAcceptedChars(value < 0 ? "0123456789.-" : "0123456789.");

		if (value < 0)
		{
			m_btnMatrix.clearButtonCtrl(9, LV_BTNMATRIX_CTRL_DISABLED);
		}
		else
		{
			m_btnMatrix.setButtonCtrl(9, LV_BTNMATRIX_CTRL_DISABLED);
		}

		validateInput();
	}

	void NumberPad::setMaxValue(float value)
	{
		m_maxValue = value;
		validateInput();
	}

	void NumberPad::setRange(float minValue, float maxValue)
	{
		UI_LOCK();
		m_minValue = minValue;
		m_maxValue = maxValue;
		validateInput();
	}

	void NumberPad::setValue(float value)
	{
		UI_LOCK();
		m_textBox.setText(fmt::format("{:g}", value));
		m_textBox.setCursorPos(LV_TEXTAREA_CURSOR_LAST);
		validateInput();
	}

	float NumberPad::getValue() const
	{
		return static_cast<float>(atof(m_textBox.getText().data()));
	}

	void NumberPad::setText(std::string_view text)
	{
		m_textBox.setText(text);
		m_textBox.setCursorPos(LV_TEXTAREA_CURSOR_LAST);
	}

	std::string_view NumberPad::getText() const
	{
		return m_textBox.getText();
	}

	bool NumberPad::validateInput()
	{
		UI_LOCK();
		float value = getValue();
		if (value < m_minValue || value > m_maxValue)
		{
			m_btnMatrix.setButtonCtrl(13, LV_BTNMATRIX_CTRL_DISABLED);
			return false;
		}
		else
		{
			m_btnMatrix.clearButtonCtrl(13, LV_BTNMATRIX_CTRL_DISABLED);
			return true;
		}
	}

	void NumberPad::setConfirmIsAction(bool isAction)
	{
		if (isAction)
		{
			m_btnMatrix.setButtonCtrl(13, LV_BTNMATRIX_CTRL_CUSTOM_1);
		}
		else
		{
			m_btnMatrix.clearButtonCtrl(13, LV_BTNMATRIX_CTRL_CUSTOM_1);
		}
	}

	void NumberPad::setConfirmCallback(confirm_cb_t eventCb)
	{
		UI_LOCK();
		m_confirmCb = std::move(eventCb);
	}

	void NumberPad::clearBtnEventHandler(lv_event_t* e)
	{
		UI_LOCK();
		NumberPad* np = (NumberPad*)lv_event_get_user_data(e);
		np->clear();
	}

	void NumberPad::onReadyEventHandler(lv_event_t* e)
	{
		UI_LOCK();
		NumberPad& np = *(NumberPad*)lv_event_get_user_data(e);
		if (np.m_confirmCb.has_value())
		{
			std::visit(overloaded{[&](const std::function<void(float)>& cb)
								  {
									  if (np.validateInput())
									  {
										  float value = np.getValue();
										  std::invoke(cb, value);
									  }
								  },
								  [&](const std::function<void(std::string_view)>& cb)
								  {
									  std::string_view text = np.getText();
									  std::invoke(cb, text);
								  }},
					   np.m_confirmCb.value());
		}
		if (np.getCloseOnConfirm())
		{
			np.close();
		}
	}

	void NumberPad::btnmEventHandler(lv_event_t* e)
	{
		UI_LOCK();
		lv_obj_t* obj = lv_event_get_target_obj(e);
		TextBox& ta = *(TextBox*)lv_event_get_user_data(e);
		NumberPad& np = *(NumberPad*)ta.getUserData();

		uint32_t btn_id = lv_buttonmatrix_get_selected_button(obj);
		if (btn_id == LV_BUTTONMATRIX_BUTTON_NONE)
			return;

		const char* txt = lv_buttonmatrix_get_button_text(obj, btn_id);
		if (txt == NULL)
			return;

		if (lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
		{
			ta.deleteChar();
			np.validateInput();
		}
		else if (lv_strcmp(txt, LV_SYMBOL_OK) == 0)
		{
			np.confirm();
		}
		else if (lv_strcmp(txt, "+/-") == 0)
		{
			uint32_t cur = ta.getCursorPos();
			std::string_view ta_txt = ta.getText();
			if (ta_txt.empty() || ta_txt[0] != '-')
			{
				ta.setCursorPos(0);
				ta.addChar('-');
				ta.setCursorPos(cur + 1);
			}
			else
			{
				ta.setCursorPos(1);
				ta.deleteChar();
				ta.setCursorPos(cur > 0 ? cur - 1 : 0);
			}
			np.validateInput();
		}
		else
		{
			ta.addText(txt);
			np.validateInput();
		}
	}
} // namespace UI
