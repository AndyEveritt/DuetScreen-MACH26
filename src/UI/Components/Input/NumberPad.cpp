/*
 * NumberPad.cpp
 *
 *  Created on: 2025-01-17
 *      Author: Andy Everitt
 */

#include "NumberPad.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
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
		, m_btnMatrix(lv_buttonmatrix_create(getCont()))
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		// Overall layout
		m_textCont.setFlexGrow(0); // Don't grow the text container
		m_textCont.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		lv_obj_set_flex_grow(m_btnMatrix, 1);		// Fill the remaining space with the button matrix
		lv_obj_set_width(m_btnMatrix, LV_PCT(100)); // Use 100% width

		// Text Entry Layout
		m_textCont.setStylePad(2, LV_PART_MAIN, Padding::ALL);
		m_textCont.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_textBox.setFlag(LV_OBJ_FLAG_SCROLLABLE, false);
		m_textCont.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_textBox.setFlexGrow(1);
		m_textBox.setHeight(LV_PCT(100));
		m_clearBtn.setSize(LV_PCT(20), LV_PCT(100));
		m_clearBtn.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_textBox.setOneLine(true);
		m_textBox.setAcceptedChars("0123456789");

		// Button Matrix Layout
		lv_obj_set_align(m_btnMatrix, LV_ALIGN_CENTER);
		lv_obj_remove_flag(m_btnMatrix, LV_OBJ_FLAG_CLICK_FOCUSABLE); // to keep the text area focused on button clicks
		lv_obj_add_event_cb(m_btnMatrix, btnmEventHandler, LV_EVENT_VALUE_CHANGED, &m_textBox);
		lv_buttonmatrix_set_map(m_btnMatrix, btnm_map);

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
			m_textBox.sendEvent(LV_EVENT_READY, this);
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

	bool NumberPad::validateInput() const
	{
		UI_LOCK();
		int16_t value = getValue();
		if (value < m_minValue || value > m_maxValue)
		{
			lv_buttonmatrix_set_button_ctrl(m_btnMatrix, 11, LV_BTNMATRIX_CTRL_DISABLED);
			return false;
		}
		else
		{
			lv_buttonmatrix_clear_button_ctrl(m_btnMatrix, 11, LV_BTNMATRIX_CTRL_DISABLED);
			return true;
		}
	}

	void NumberPad::setValueChangedCallback(lv_event_cb_t eventCb, void* userData)
	{
		m_textBox.addEventCallback(eventCb, LV_EVENT_VALUE_CHANGED, userData);
	}

	void NumberPad::setConfirmCallback(lv_event_cb_t eventCb, void* userData)
	{
		UI_LOCK();
		if (m_confirmCb != nullptr)
		{
			LOG_DBG("Removing previous confirm callback");
			m_textBox.removeEventCallback(m_confirmCb);
		}
		LOG_DBG("Setting new confirm callback");
		m_confirmCb = eventCb;
		lv_obj_add_event_cb(m_textBox, eventCb, LV_EVENT_READY, userData);
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
