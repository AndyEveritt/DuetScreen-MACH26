/*
 * MultiValueSelector.h
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/ModalNumberPad.h"
#include "UI/Components/Input/TextBox.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/List/List.h"

namespace UI
{
	class MultiValueSelector : public LvContainer
	{
	  public:
		MultiValueSelector(const std::string& name, LvObj& parent);

		void setLabel(const std::string& label);

		void setValue(float value);
		float getValue() const;

		void setIncrement(float increment);
		void setValueBtn(size_t index, float value);
		void setValueBtns(const std::vector<float>& values);
		const std::vector<float>& getValues() const { return m_currentValues; }

		void setMinValue(float value);
		void setMaxValue(float value);

		float getMinValue() const { return m_minValue; }
		float getMaxValue() const { return m_maxValue; }

		void setStorageKey(std::string_view key);

		void setNumberPad(ModalNumberPad* np) { m_numberPad = np; }
		ModalNumberPad* getNumberPad() const { return m_numberPad; }

	  private:
		void saveValues() const;
		void saveSelected() const;

		LvLabel m_label{"label", getRoot()};
		LvContainer m_topRow{"topRow", getRoot()};
		Button m_decrementBtn{"decrement", m_topRow};
		LvTextarea m_valueDisplay{"valueDisplay", m_topRow};
		Button m_incrementBtn{"increment", m_topRow};
		List<Button> m_valueBtns{"values", getRoot()};

		ModalNumberPad* m_numberPad = nullptr;

		float m_incrementValue = 1.0f;
		float m_value = std::numeric_limits<float>::max(); // this is set to 0.0f in constructor to update label
		float m_minValue = 0.0f;
		float m_maxValue = std::numeric_limits<float>::max();
		std::vector<float> m_currentValues;
		std::string m_storageKeyPrefix;
	};
} // namespace UI
