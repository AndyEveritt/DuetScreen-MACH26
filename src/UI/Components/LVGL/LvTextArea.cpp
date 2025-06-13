/*
 * LvTextArea.cpp
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#include "LvTextArea.h"
#include "Debug.h"

namespace UI
{
	LvTextArea::LvTextArea(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_textarea_create, name, parent)
	{
		UI_LOCK();

		setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		setCursorClickPos(true);
	}

	void LvTextArea::setText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_set_text(getTextArea(), text.c_str());
	}
	std::string LvTextArea::getText() const
	{
		UI_LOCK();
		return lv_textarea_get_text(getTextArea());
	}

	void LvTextArea::addChar(uint32_t c)
	{
		UI_LOCK();
		lv_textarea_add_char(getTextArea(), c);
	}

	void LvTextArea::addText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_add_text(getTextArea(), text.c_str());
	}

	void LvTextArea::deleteChar()
	{
		UI_LOCK();
		lv_textarea_delete_char(getTextArea());
	}

	void LvTextArea::deleteCharForward()
	{
		UI_LOCK();
		lv_textarea_delete_char_forward(getTextArea());
	}

	void LvTextArea::setPlaceholderText(const std::string& text)
	{
		UI_LOCK();
		lv_textarea_set_placeholder_text(getTextArea(), text.c_str());
	}

	void LvTextArea::setCursorPos(uint32_t pos)
	{
		UI_LOCK();
		lv_textarea_set_cursor_pos(getTextArea(), pos);
	}

	void LvTextArea::setCursorClickPos(bool clickPos)
	{
		UI_LOCK();
		lv_textarea_set_cursor_click_pos(getTextArea(), clickPos);
	}

	void LvTextArea::setPasswordMode(bool passwordMode)
	{
		UI_LOCK();
		lv_textarea_set_password_mode(getTextArea(), passwordMode);
	}

	void LvTextArea::setPasswordBullet(const char* bullet)
	{
		UI_LOCK();
		lv_textarea_set_password_bullet(getTextArea(), bullet);
	}

	void LvTextArea::setPasswordShowTime(uint32_t time)
	{
		UI_LOCK();
		lv_textarea_set_password_show_time(getTextArea(), time);
	}

	void LvTextArea::setOneLine(bool oneLine)
	{
		UI_LOCK();
		lv_textarea_set_one_line(getTextArea(), oneLine);
	}
	void LvTextArea::setAcceptedChars(const char* chars)
	{
		UI_LOCK();
		lv_textarea_set_accepted_chars(getTextArea(), chars);
	}
	void LvTextArea::setMaxLength(uint32_t length)
	{
		UI_LOCK();
		lv_textarea_set_max_length(getTextArea(), length);
	}

	void LvTextArea::setTextSelection(bool enable)
	{
		UI_LOCK();
		lv_textarea_set_text_selection(getTextArea(), enable);
	}

	bool LvTextArea::isTextSelected() const
	{
		UI_LOCK();
		return lv_textarea_text_is_selected(getTextArea());
	}

	void LvTextArea::clearSelection()
	{
		UI_LOCK();
		lv_textarea_clear_selection(getTextArea());
	}

	void LvTextArea::cursorRight()
	{
		UI_LOCK();
		lv_textarea_cursor_right(getTextArea());
	}

	void LvTextArea::cursorLeft()
	{
		UI_LOCK();
		lv_textarea_cursor_left(getTextArea());
	}

	void LvTextArea::cursorUp()
	{
		UI_LOCK();
		lv_textarea_cursor_up(getTextArea());
	}

	void LvTextArea::cursorDown()
	{
		UI_LOCK();
		lv_textarea_cursor_down(getTextArea());
	}
} // namespace UI
