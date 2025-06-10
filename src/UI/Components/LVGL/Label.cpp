/*
 * Label.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "Label.h"
#include "Debug.h"

namespace UI
{
	Label::Label(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_label_create, name, parent)
	{
		UI_LOCK();
	}

	void Label::setText(const std::string& text)
	{
		UI_LOCK();
		lv_label_set_text(getCont(), text.c_str());
	}

	void Label::setTextF(const char* fmt, ...)
	{
		UI_LOCK();
		va_list args;
		va_start(args, fmt);
		lv_label_set_text_fmt(getCont(), fmt, args);
		va_end(args);
	}

	void Label::setTextStatic(const char* text)
	{
		UI_LOCK();
		lv_label_set_text_static(getCont(), text);
	}

	void Label::setLongMode(lv_label_long_mode_t mode)
	{
		UI_LOCK();
		lv_label_set_long_mode(getCont(), mode);
	}

	void Label::setTextSelectionStart(uint32_t index)
	{
		UI_LOCK();
		lv_label_set_text_selection_start(getCont(), index);
	}

	void Label::setTextSelectionEnd(uint32_t index)
	{
		UI_LOCK();
		lv_label_set_text_selection_end(getCont(), index);
	}

	void Label::setRecolor(bool enable)
	{
		UI_LOCK();
		lv_label_set_recolor(getCont(), enable);
	}

	std::string Label::getText() const
	{
		UI_LOCK();
		return lv_label_get_text(getCont());
	}

	lv_label_long_mode_t Label::getLongMode() const
	{
		UI_LOCK();
		return lv_label_get_long_mode(getCont());
	}

	void Label::getLetterPos(uint32_t char_id, lv_point_t* pos) const
	{
		UI_LOCK();
		lv_label_get_letter_pos(getCont(), char_id, pos);
	}

	uint32_t Label::getLetterOn(lv_point_t* pos_in, bool bidi) const
	{
		UI_LOCK();
		return lv_label_get_letter_on(getCont(), pos_in, bidi);
	}

	bool Label::isCharUnderPos(lv_point_t* pos) const
	{
		UI_LOCK();
		return lv_label_is_char_under_pos(getCont(), pos);
	}

	uint32_t Label::getTextSelectionStart() const
	{
		UI_LOCK();

		return lv_label_get_text_selection_start(getCont());
	}

	uint32_t Label::getTextSelectionEnd() const
	{
		UI_LOCK();
		return lv_label_get_text_selection_end(getCont());
	}

	bool Label::isRecolorEnabled() const
	{
		UI_LOCK();
		return lv_label_get_recolor(getCont());
	}

	void Label::insertText(uint32_t pos, const std::string& text)
	{
		UI_LOCK();
		lv_label_ins_text(getCont(), pos, text.c_str());
	}

	void Label::cutText(uint32_t pos, uint32_t len)
	{
		UI_LOCK();
		lv_label_cut_text(getCont(), pos, len);
	}

} // namespace UI
