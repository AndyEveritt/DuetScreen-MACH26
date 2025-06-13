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
#include "lv_i18n/lv_i18n.h"
#include <string>

namespace UI
{
	static const char* btnm_map[] = {
		"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_OK, ""};

	NumberPad::NumberPad(const std::string& name, lv_obj_t* parent, layout_t layout)
		: LvObj(lv_obj_create, name, parent, layout)
		, m_textCont(name + "_textcont", getCont())
		, m_textBox(name + "_textarea", m_textCont)
		, m_clearBtn("Clear", m_textCont, LV_SYMBOL_TRASH, layout_t{LV_PCT(75), 0, LV_PCT(20), LV_PCT(80)})
		, m_btnMatrix(name + "_btnmatrix", getCont())
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		// Overall layout
		m_textCont.setFlexGrow(0);			 // Don't grow the text container
		m_textCont.setSize(LV_PCT(100), 50); // LV_SIZE_CONTENT is bugged
		m_btnMatrix.setFlexGrow(1);
		m_btnMatrix.setWidth(LV_PCT(100));

		// Text Entry Layout
		m_textCont.addStyle(Themes::getLvglStyles().no_border);
		m_textBox.addStyle(Themes::getLvglStyles().no_border);
		m_textCont.addStyle(Themes::getLvglStyles().pad_tiny);
		m_textCont.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_textBox.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_textCont.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_textCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
		m_textBox.setFlexGrow(1);
		m_textBox.setHeight(LV_SIZE_CONTENT);
		m_clearBtn.setSize(LV_PCT(20), LV_PCT(100));
		m_clearBtn.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_textBox.setOneLine(true);
		m_textBox.setAcceptedChars("0123456789");

		// Button Matrix Layout
		m_btnMatrix.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_btnMatrix.setFlag(LV_OBJ_FLAG_CLICK_FOCUSABLE, false); // to keep the text area focused on button clicks
		m_btnMatrix.addEventCallback(btnmEventHandler, LV_EVENT_VALUE_CHANGED, &m_textBox);
		m_btnMatrix.setMap(btnm_map);

		m_textBox.setUserData(this);
		m_textBox.setText("");

		m_clearBtn.addClickedCallback(clearBtnEventHandler, this);

		validateInput();
	}
	NumberPad::NumberPad(const std::string& name, lv_obj_t* parent, layout_t layout, const NumberPadConfig& config)
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
			m_textBox.getTextArea().sendEvent(LV_EVENT_READY, this);
			if (m_closeOnConfirm)
			{
				close();
			}
		}
	}

	void NumberPad::setMinValue(int16_t value)
	{
		m_minValue = value;
		validateInput();
	}

	void NumberPad::setMaxValue(int16_t value)
	{
		m_maxValue = value;
		validateInput();
	}

	void NumberPad::setValue(int16_t value)
	{
		UI_LOCK();
		m_textBox.setText(std::to_string(value));
		validateInput();
	}

	int16_t NumberPad::getValue() const
	{
		return atoi(m_textBox.getText().c_str());
	}

	bool NumberPad::validateInput()
	{
		UI_LOCK();
		int16_t value = getValue();
		if (value < m_minValue || value > m_maxValue)
		{
			m_btnMatrix.setButtonCtrl(11, LV_BTNMATRIX_CTRL_DISABLED);
			return false;
		}
		else
		{
			m_btnMatrix.clearButtonCtrl(11, LV_BTNMATRIX_CTRL_DISABLED);
			return true;
		}
	}

	void NumberPad::setValueChangedCallback(lv_event_cb_t eventCb, void* userData)
	{
		m_textBox.getTextArea().addEventCallback(eventCb, LV_EVENT_VALUE_CHANGED, userData);
	}

	void NumberPad::setConfirmCallback(lv_event_cb_t eventCb, void* userData)
	{
		UI_LOCK();
		if (m_confirmCb != nullptr)
		{
			LOG_DBG("Removing previous confirm callback");
			m_textBox.getTextArea().removeEventCallback(m_confirmCb);
		}
		LOG_DBG("Setting new confirm callback");
		m_confirmCb = eventCb;
		m_textBox.getTextArea().addEventCallback(eventCb, LV_EVENT_READY, userData);
	}

	void NumberPad::clearBtnEventHandler(lv_event_t* e)
	{
		UI_LOCK();
		NumberPad* np = (NumberPad*)lv_event_get_user_data(e);
		np->clear();
	}

	void NumberPad::btnmEventHandler(lv_event_t* e)
	{
		UI_LOCK();
		lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
		TextBox& ta = *(TextBox*)lv_event_get_user_data(e);
		NumberPad* np = (NumberPad*)lv_obj_get_user_data(ta);
		const char* txt = lv_buttonmatrix_get_button_text(obj, lv_buttonmatrix_get_selected_button(obj));

		if (lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
		{
			ta.deleteChar();
			np->validateInput();
		}
		else if (lv_strcmp(txt, LV_SYMBOL_OK) == 0)
		{
			np->confirm();
		}
		else
		{
			ta.addText(txt);
			np->validateInput();
		}
	}
} // namespace UI
