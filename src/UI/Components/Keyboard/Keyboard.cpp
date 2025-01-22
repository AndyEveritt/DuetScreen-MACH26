/*
 * Keyboard.cpp
 *
 *  Created on: 2025-01-17
 *      Author: Andy Everitt
 */

#include "Keyboard.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"
#include <string>

namespace UI
{
	static const char* btnm_map[] = {
		"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", "\n", LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_OK, ""};

	Keyboard::Keyboard(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_textCont(lv_obj_create(getCont()))
		, m_textArea(lv_textarea_create(m_textCont))
		, m_clearBtn("Clear", m_textCont, LV_SYMBOL_TRASH, layout_t{LV_PCT(75), 0, LV_PCT(20), LV_PCT(80)})
		, m_btnMatrix(lv_buttonmatrix_create(getCont()))
	{
		setLayoutStyle(LV_LAYOUT_FLEX, LV_FLEX_FLOW_COLUMN);

		// Overall layout
		lv_obj_set_flex_grow(m_textCont, 0);		  // Don't grow the text area
		lv_obj_set_flex_grow(m_btnMatrix, 1);		  // Fill the remaining space with the button matrix
		lv_obj_set_size(m_textCont, LV_PCT(100), 50); // Use 100% width and fix height to 50px
		lv_obj_set_width(m_btnMatrix, LV_PCT(100));	  // Use 100% width

		// Text Entry Layout
		lv_obj_set_style_pad_all(m_textCont, 2, 0);
		lv_obj_remove_flag(m_textCont, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_remove_flag(m_textArea, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_flex_flow(m_textCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_grow(m_textArea, 1);
		lv_obj_set_flex_grow(m_clearBtn.getCont(), 0);
		lv_obj_set_height(m_textArea, LV_PCT(100));
		lv_obj_set_size(m_clearBtn.getCont(), LV_PCT(20), LV_PCT(100));
		lv_obj_set_style_pad_all(m_clearBtn.getCont(), 0, 0);
		lv_obj_set_align(m_clearBtn.getCont(), LV_ALIGN_RIGHT_MID);
		lv_textarea_set_one_line(m_textArea, true);
		lv_textarea_set_accepted_chars(m_textArea, "0123456789");

		lv_obj_set_align(m_btnMatrix, LV_ALIGN_CENTER);
		lv_obj_remove_flag(m_btnMatrix, LV_OBJ_FLAG_CLICK_FOCUSABLE); // to keep the text area focused on button clicks
		lv_obj_add_event_cb(m_btnMatrix, btnmEventHandler, LV_EVENT_VALUE_CHANGED, m_textArea);
		lv_buttonmatrix_set_map(m_btnMatrix, btnm_map);

		lv_obj_set_user_data(m_textArea, this);
		lv_textarea_set_text(m_textArea, "");

		m_clearBtn.setCallback(clearBtnEventHandler, LV_EVENT_PRESSED, this);

		validateInput();
	}
	Keyboard::Keyboard(const std::string& name, lv_obj_t* parent, layout_t layout, const KeyboardConfig& config)
		: Keyboard(name, parent, layout)
	{
		// Configure
		setMinValue(config.minValue);
		setMaxValue(config.maxValue);
		validateInput();
	}

	bool Keyboard::back()
	{
		// This custom back method prevents the previous screen from reopening
		close();
		return true;
	}

	void Keyboard::clear()
	{
		{
			Lock lock;
			lv_textarea_set_text(m_textArea, "");
		}
		validateInput();
	}

	void Keyboard::close()
	{
		closeScreen(this, false);
	}

	void Keyboard::confirm()
	{
		if (validateInput())
		{
			Lock lock;
			// Call the confirm callback
			lv_obj_send_event(m_textArea, LV_EVENT_READY, this);
			if (m_closeOnConfirm)
			{
				close();
			}
		}
	}

	void Keyboard::setMinValue(int16_t value)
	{
		m_minValue = value;
		validateInput();
	}

	void Keyboard::setMaxValue(int16_t value)
	{
		m_maxValue = value;
		validateInput();
	}

	void Keyboard::setValue(int16_t value)
	{
		{
			Lock lock;
			lv_textarea_set_text(m_textArea, std::to_string(value).c_str());
		}
		validateInput();
	}

	int16_t Keyboard::getValue() const
	{
		Lock lock;
		return atoi(lv_textarea_get_text(m_textArea));
	}

	bool Keyboard::validateInput() const
	{
		Lock lock;
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

	void Keyboard::setValueChangedCallback(lv_event_cb_t eventCb, void* userData)
	{
		Lock lock;
		lv_obj_add_event_cb(m_textArea, eventCb, LV_EVENT_VALUE_CHANGED, userData);
	}

	void Keyboard::setConfirmCallback(lv_event_cb_t eventCb, void* userData)
	{
		Lock lock;
		uint32_t event_cnt = lv_obj_get_event_count(m_textArea);
		for (uint32_t i = 0; i < event_cnt; i++)
		{
			lv_obj_remove_event(m_textArea, i);
		}
		lv_obj_add_event_cb(m_textArea, eventCb, LV_EVENT_READY, userData);
	}

	void Keyboard::clearBtnEventHandler(lv_event_t* e)
	{
		Keyboard* np = (Keyboard*)lv_event_get_user_data(e);
		np->clear();
	}

	void Keyboard::btnmEventHandler(lv_event_t* e)
	{
		lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
		lv_obj_t* ta = (lv_obj_t*)lv_event_get_user_data(e);
		Keyboard* np = (Keyboard*)lv_obj_get_user_data(ta);
		const char* txt = lv_buttonmatrix_get_button_text(obj, lv_buttonmatrix_get_selected_button(obj));

		if (lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
		{
			lv_textarea_delete_char(ta);
			np->validateInput();
		}
		else if (lv_strcmp(txt, LV_SYMBOL_OK) == 0)
		{
			np->confirm();
		}
		else
		{
			lv_textarea_add_text(ta, txt);
			np->validateInput();
		}
	}
} // namespace UI
