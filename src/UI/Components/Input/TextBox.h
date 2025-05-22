/*
 * TextBox.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Core/View.h"

namespace UI
{
	class TextBox : public BaseView
	{
	  public:
		TextBox(const std::string& name, lv_obj_t* parent, layout_t layout);

        void setLabel(const std::string& label);
		void setText(const std::string& text);
		std::string getText() const;
		lv_obj_t* getTextArea() const { return m_textArea; }

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
		void addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData);
        void addConfirmEventCallback(lv_event_cb_t cb, void* userData);

		void showPassword(bool show);

	  private:
		lv_obj_t* m_label;
		lv_obj_t* m_textArea;
		Button m_showPassword;

		bool m_passwordMode = false;
	};
} // namespace UI
