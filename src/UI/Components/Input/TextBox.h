/*
 * TextBox.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvObj.h"
#include "UI/Components/LVGL/LvTextArea.h"

namespace UI
{
	class TextBox : public LvObj
	{
	  public:
		TextBox(const std::string& name, lv_obj_t* parent);
		TextBox(const std::string& name, lv_obj_t* parent, layout_t layout);

        void setLabel(const std::string& label);
		void setText(const std::string& text);
		std::string getText() const;
		LvTextArea& getTextArea() { return m_textArea; }

		void addChar(uint32_t c);
		void addText(const std::string& text);
		void deleteChar();
		void deleteCharForward();
		void setPlaceholderText(const std::string& text);
		void setCursorPos(uint32_t pos);
		void setCursorClickPos(bool clickPos);
		void setPasswordMode(bool passwordMode);
		void setPasswordBullet(const char* bullet);
		void setPasswordShowTime(uint32_t time);
		void setOneLine(bool oneLine);
		void setAcceptedChars(const char* chars);
		void setMaxLength(uint32_t length);
		void setTextSelection(bool enable);
		bool isTextSelected() const;
		void clearSelection();
		void cursorRight();
		void cursorLeft();
		void cursorUp();
		void cursorDown();
		void addConfirmEventCallback(lv_event_cb_t cb, void* userData);

		void showPassword(bool show);

	  private:
		void init();

		LvLabel m_label;
		LvTextArea m_textArea;
		Button m_showPassword;

		bool m_passwordMode = false;
	};
} // namespace UI
