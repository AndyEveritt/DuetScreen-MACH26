/*
 * TextBox.cpp
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#include "TextBox.h"
#include "Debug.h"

namespace UI
{
	TextBox::TextBox(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_label(lv_label_create(getCont()))
		, m_textArea(lv_textarea_create(getCont()))
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Label
		lv_obj_set_size(m_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		// TextArea
		lv_obj_set_size(m_textArea, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_flex_grow(m_textArea, 1);
		lv_textarea_set_cursor_click_pos(m_textArea, true);
	}

	void TextBox::setLabel(const std::string& label)
	{
		UI_LOCK();
		lv_label_set_text(m_label, label.c_str());
	}
	void TextBox::setText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_set_text(m_textArea, text.c_str());
		lv_textarea_set_cursor_pos(m_textArea, 0);
		lv_obj_scroll_to_x(m_textArea, 0, LV_ANIM_OFF);
	}
	std::string TextBox::getText() const
	{
		UI_LOCK();
		return lv_textarea_get_text(m_textArea);
	}

	void TextBox::setOneLine(bool oneLine)
	{
		UI_LOCK();
		lv_textarea_set_one_line(m_textArea, oneLine);
	}
	void TextBox::setPlaceholderText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_set_placeholder_text(m_textArea, text.c_str());
	}
	void TextBox::setAcceptedChars(const char* chars)
	{
		UI_LOCK();
		lv_textarea_set_accepted_chars(m_textArea, chars);
	}
	void TextBox::setMaxLength(uint32_t length)
	{
		UI_LOCK();
		lv_textarea_set_max_length(m_textArea, length);
	}
	void TextBox::setPasswordMode(bool passwordMode)
	{
		UI_LOCK();
		lv_textarea_set_password_mode(m_textArea, passwordMode);
	}

	void TextBox::addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData)
	{
		UI_LOCK();
		lv_obj_add_event_cb(m_textArea, cb, code, userData);
	}

	void TextBox::addConfirmEventCallback(lv_event_cb_t cb, void* userData)
	{
		UI_LOCK();
		lv_obj_set_user_data(m_textArea, reinterpret_cast<void*>(cb));
		addEventCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				auto callback =
					reinterpret_cast<lv_event_cb_t>(lv_obj_get_user_data((lv_obj_t*)lv_event_get_target(e)));
				lv_event_code_t code = lv_event_get_code(e);
				if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED)
				{
					callback(e);
				}
			},
			LV_EVENT_ALL,
			userData);
	}
} // namespace UI
