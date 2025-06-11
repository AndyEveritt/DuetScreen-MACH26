/*
 * TextBox.cpp
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#include "TextBox.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	TextBox::TextBox(const std::string& name, lv_obj_t* parent, layout_t layout)
		: LvObj(lv_obj_create, name, parent, layout)
		, m_label(lv_label_create(getCont()))
		, m_textArea(lv_textarea_create(getCont()))
		, m_showPassword("show_password", m_textArea, LV_SYMBOL_EYE_OPEN)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Label
		lv_obj_set_size(m_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		setLabel("");
		lv_obj_set_style_max_width(m_label, LV_PCT(50), 0);

		// TextArea
		lv_obj_set_size(m_textArea, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_flex_grow(m_textArea, 1);
		lv_textarea_set_cursor_click_pos(m_textArea, true);

		// Show Password Button
		m_showPassword.setSize(LV_SIZE_CONTENT, LV_PCT(100));
		m_showPassword.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_showPassword.hide();
		m_showPassword.setCheckable(true);
		m_showPassword.setCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				TextBox* tb = (TextBox*)lv_event_get_user_data(e);

				bool passwordMode = lv_textarea_get_password_mode(tb->m_textArea);
				tb->showPassword(passwordMode);
				// lv_group_focus_obj(tb->m_textArea);
			},
			LV_EVENT_CLICKED,
			this);
	}

	void TextBox::setLabel(const std::string& label)
	{
		UI_LOCK();
		lv_obj_set_flag(m_label, LV_OBJ_FLAG_HIDDEN, label.empty());
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

	void TextBox::addChar(uint32_t c)
	{
		UI_LOCK();
		lv_textarea_add_char(m_textArea, c);
	}

	void TextBox::addText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_add_text(m_textArea, text.c_str());
	}

	void TextBox::deleteChar()
	{
		UI_LOCK();
		lv_textarea_delete_char(m_textArea);
	}

	void TextBox::deleteCharForward()
	{
		UI_LOCK();
		lv_textarea_delete_char_forward(m_textArea);
	}

	void TextBox::setPlaceholderText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_set_placeholder_text(m_textArea, text.c_str());
	}

	void TextBox::setCursorPos(uint32_t pos)
	{
		UI_LOCK();
		lv_textarea_set_cursor_pos(m_textArea, pos);
	}

	void TextBox::setCursorClickPos(bool clickPos)
	{
		UI_LOCK();
		lv_textarea_set_cursor_click_pos(m_textArea, clickPos);
	}

	void TextBox::setPasswordMode(bool passwordMode)
	{
		UI_LOCK();
		m_passwordMode = passwordMode;
		m_showPassword.setChecked(false);
		m_showPassword.setVisibility(passwordMode);
		lv_textarea_set_password_mode(m_textArea, passwordMode);
	}

	void TextBox::setPasswordBullet(const char* bullet)
	{
		UI_LOCK();
		lv_textarea_set_password_bullet(m_textArea, bullet);
	}

	void TextBox::setPasswordShowTime(uint32_t time)
	{
		UI_LOCK();
		lv_textarea_set_password_show_time(m_textArea, time);
	}

	void TextBox::showPassword(bool show)
	{
		UI_LOCK();
		if (!m_passwordMode)
			return;

		lv_textarea_set_password_mode(m_textArea, !show);
		m_showPassword.setChecked(show);
	}

	void TextBox::setOneLine(bool oneLine)
	{
		UI_LOCK();
		lv_textarea_set_one_line(m_textArea, oneLine);
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

	void TextBox::setTextSelection(bool enable)
	{
		UI_LOCK();
		lv_textarea_set_text_selection(m_textArea, enable);
	}

	bool TextBox::isTextSelected() const
	{
		UI_LOCK();
		return lv_textarea_text_is_selected(m_textArea);
	}

	void TextBox::clearSelection()
	{
		UI_LOCK();
		lv_textarea_clear_selection(m_textArea);
	}

	void TextBox::cursorRight()
	{
		UI_LOCK();
		lv_textarea_cursor_right(m_textArea);
	}

	void TextBox::cursorLeft()
	{
		UI_LOCK();
		lv_textarea_cursor_left(m_textArea);
	}

	void TextBox::cursorUp()
	{
		UI_LOCK();
		lv_textarea_cursor_up(m_textArea);
	}

	void TextBox::cursorDown()
	{
		UI_LOCK();
		lv_textarea_cursor_down(m_textArea);
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
