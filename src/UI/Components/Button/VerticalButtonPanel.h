/*
 * VerticalButtonPanel.h
 *
 *  Created on: 2025-02-28
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/List/List.h"
#include "UI/Core/View.h"
#include <span>

namespace UI
{
	class VerticalButtonPanel : public LvObj
	{
	  public:
		VerticalButtonPanel(const std::string& name, LvObj& parent);
		void setIncrementIcon(std::string_view icon);
		void setDecrementIcon(std::string_view icon);
		void setIncrementLabel(std::string_view label);
		void setDecrementLabel(std::string_view label);
		void setResetLabel(std::string_view label);
		void setValueLabelFmt(std::string_view fmt);
		void setIncrementValues(std::span<const float> values);

		float getSelectedValue() const;
		void setSelectedValueIndex(size_t index);

		void setValueChangeCallback(std::function<void(float)> callback);
		void setResetCallback(std::function<void()> callback);

		Button& getResetButton() { return m_reset; }
		Button& getIncrementButton() { return m_increment; }
		Button& getDecrementButton() { return m_decrement; }

		void setNumberPad(ModalNumberPad* numberPad) { m_numberPad = numberPad; }
		ModalNumberPad* getNumberPad() const { return m_numberPad; }

	  private:
		std::unique_ptr<Button> createValueButton(size_t index, LvObj& parent);

		void updateValueLabels();

		Button m_reset;
		Button m_increment;
		Button m_decrement;

		List<Button> m_values;

		ModalNumberPad* m_numberPad = nullptr;

		size_t m_selectedValueIndex = 0;

		std::string m_fmt;
		std::vector<float> m_incrementValues;

		std::function<void(float)> m_valueChangeCallback;
		std::function<void()> m_resetCallback;
	};
} // namespace UI
