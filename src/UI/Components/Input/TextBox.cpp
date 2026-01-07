/*
 * TextBox.cpp
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#include "TextBox.h"
#include "Debug.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Core/Navigation.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"

namespace UI
{
	TextBox::TextBox(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		init();
	}

	TextBox::TextBox(const std::string& name, LvObj& parent, layout_t layout)
		: LvContainer(name, parent, layout)
	{
		init();
	}

	void TextBox::init()
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Label
		m_label.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_label.setMaxWidth(LV_PCT(50));
		setLabel("");

		// Textarea
		m_textarea.setSize(LV_PCT(100), LV_PCT(100));
		m_textarea.setMinHeight(LV_SIZE_CONTENT);
		// m_textarea.setMinHeight(20);
		m_textarea.setFlexGrow(1);
		m_textarea.setCursorClickPos(true);
		m_textarea.addEventCallback(
			[this](lv_event_t*)
			{
				if (m_keyboard)
				{
					m_keyboard->setTextarea(&m_textarea);
				}
				else if (m_numberPad)
				{
					switch (m_mode)
					{
					case Mode::TEXT:
						m_numberPad->setConfirmCallback([this](std::string_view text) { setText(std::string(text)); });
						break;
					case Mode::NUMBER:
						m_numberPad->setConfirmCallback([this](float value) { setText(fmt::format("{:g}", value)); });
						break;
					}
					m_numberPad->setText(std::string(m_textarea.getText()));
					m_textarea.sendEvent(LV_EVENT_DEFOCUSED); // stop cursor blinking
					openModal(m_numberPad);
				}
			},
			LV_EVENT_CLICKED);

		// Show Password Button
		m_showPassword.setSize(LV_SIZE_CONTENT, LV_PCT(100));
		m_showPassword.setMinHeight(LV_SIZE_CONTENT);
		m_showPassword.setAlign(LV_ALIGN_RIGHT_MID, 0, 0);
		m_showPassword.hide();
		m_showPassword.setCheckable(true);
		m_showPassword.addClickedCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				TextBox* tb = static_cast<TextBox*>(lv_event_get_user_data(e));

				bool passwordMode = tb->m_textarea.getPasswordMode();
				tb->showPassword(passwordMode);
				// lv_group_focus_obj(tb->m_textarea);
			},
			this);
	}

	void TextBox::setLabel(const std::string& label)
	{
		m_label.setFlag(LV_OBJ_FLAG_HIDDEN, label.empty());
		m_label.setText(label);
	}
	void TextBox::setText(const std::string& text)
	{
		m_textarea.setText(text);
		m_textarea.setCursorPos(0);
		m_textarea.scrollToX(0, LV_ANIM_OFF);
		sendEvent(LV_EVENT_VALUE_CHANGED);
	}
	std::string_view TextBox::getText() const
	{
		return m_textarea.getText();
	}

	void TextBox::addChar(uint32_t c)
	{
		m_textarea.addChar(c);
	}

	void TextBox::addText(const std::string& text)
	{
		m_textarea.addText(text);
	}

	void TextBox::deleteChar()
	{
		m_textarea.deleteChar();
	}

	void TextBox::deleteCharForward()
	{
		m_textarea.deleteCharForward();
	}

	void TextBox::setPlaceholderText(const std::string& text)
	{
		m_textarea.setPlaceholderText(text);
	}

	void TextBox::setCursorPos(int32_t pos)
	{
		m_textarea.setCursorPos(pos);
	}

	void TextBox::setCursorClickPos(bool clickPos)
	{
		m_textarea.setCursorClickPos(clickPos);
	}

	void TextBox::setPasswordMode(bool passwordMode)
	{
		m_passwordMode = passwordMode;
		m_showPassword.setChecked(false);
		m_showPassword.setVisible(passwordMode);
		m_textarea.setPasswordMode(passwordMode);
	}

	void TextBox::setPasswordBullet(const char* bullet)
	{
		m_textarea.setPasswordBullet(bullet);
	}

	void TextBox::setPasswordShowTime(uint32_t time)
	{
		m_textarea.setPasswordShowTime(time);
	}

	void TextBox::showPassword(bool show)
	{
		UI_LOCK();
		if (!m_passwordMode)
			return;
		m_textarea.setPasswordMode(!show);
		m_showPassword.setChecked(show);
	}

	void TextBox::setOneLine(bool oneLine)
	{
		m_textarea.setOneLine(oneLine);
	}
	void TextBox::setAcceptedChars(const char* chars)
	{
		m_textarea.setAcceptedChars(chars);
	}
	void TextBox::setMaxLength(uint32_t length)
	{
		m_textarea.setMaxLength(length);
	}

	void TextBox::setTextSelection(bool enable)
	{
		m_textarea.setTextSelection(enable);
	}

	const char* TextBox::getPlaceholderText() const
	{
		return m_textarea.getPlaceholderText();
	}

	uint32_t TextBox::getCursorPos() const
	{
		return m_textarea.getCursorPos();
	}

	bool TextBox::getCursorClickPosEnabled() const
	{
		return m_textarea.getCursorClickPos();
	}

	bool TextBox::getPasswordModeEnabled() const
	{
		return m_textarea.getPasswordMode();
	}

	const char* TextBox::getPasswordBullet() const
	{
		return m_textarea.getPasswordBullet();
	}

	bool TextBox::getOneLineEnabled() const
	{
		return m_textarea.getOneLine();
	}

	const char* TextBox::getAcceptedChars() const
	{
		return m_textarea.getAcceptedChars();
	}

	uint32_t TextBox::getMaxLength() const
	{
		return m_textarea.getMaxLength();
	}

	bool TextBox::isTextSelected() const
	{
		// because of lvgl API naming this isn't marked as const by the generator so we const_cast
		return const_cast<TextBox*>(this)->m_textarea.textIsSelected();
	}

	bool TextBox::getTextSelectionEnabled() const
	{
		return m_textarea.getTextSelection();
	}

	uint32_t TextBox::getPasswordShowTime() const
	{
		return m_textarea.getPasswordShowTime();
	}

	uint32_t TextBox::getCurrentChar() const
	{
		return m_textarea.getCurrentChar();
	}

	void TextBox::clearSelection()
	{
		m_textarea.clearSelection();
	}

	void TextBox::cursorRight()
	{
		m_textarea.cursorRight();
	}

	void TextBox::cursorLeft()
	{
		m_textarea.cursorLeft();
	}

	void TextBox::cursorUp()
	{
		m_textarea.cursorUp();
	}

	void TextBox::cursorDown()
	{
		m_textarea.cursorDown();
	}

	void TextBox::addConfirmEventCallback(std::function<void(lv_event_t*)> cb)
	{
		UI_LOCK();
		m_textarea.addEventCallback(
			[cb](lv_event_t* e)
			{
				UI_LOCK();
				lv_event_code_t code = lv_event_get_code(e);
				if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED)
				{
					std::invoke(cb, e);
				}
			},
			LV_EVENT_ALL);
	}
} // namespace UI
