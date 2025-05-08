/*
 * TextBox.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

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

		void setOneLine(bool oneLine);
		void setPlaceholderText(const std::string& text);
		void setAcceptedChars(const char* chars);
		void setMaxLength(uint32_t length);
        void setPasswordMode(bool passwordMode);
		void addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData);
        void addConfirmEventCallback(lv_event_cb_t cb, void* userData);

	  private:
		lv_obj_t* m_label;
		lv_obj_t* m_textArea;
	};
} // namespace UI
