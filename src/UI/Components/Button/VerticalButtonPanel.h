/*
 * VerticalButtonPanel.h
 *
 *  Created on: 2025-02-28
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/View.h"
#include "UI/Components/Button/Button.h"

namespace UI
{
	class VerticalButtonPanel : public LvObj
	{
      public:
		VerticalButtonPanel(const std::string& name, lv_obj_t* parent, layout_t layout);
		void setIncrementLabel(const char* label);
		void setDecrementLabel(const char* label);
		void setResetLabel(const char* label);
		void setValueLabelFmt(const std::string& fmt);
        void setIncrementValues(const std::array<float, 2>& values);
        
        float getSelectedValue() const;
        void setSelectedValueIndex(uint8_t index);

        void setValueChangeCallback(std::function<void(float)> callback);
        void setResetCallback(std::function<void()> callback);

	  private:
		Button m_reset;
		Button m_increment;
		Button m_decrement;

		lv_obj_t* m_valueCont;
		Button m_values[2];

        uint8_t m_selectedValueIndex = 0;

        std::string m_fmt;
        float m_incrementValues[2];

        std::function<void(float)> m_valueChangeCallback;
        std::function<void()> m_resetCallback;
	};
} // namespace UI
