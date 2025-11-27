/*
 * Slider.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/Input/NumberPad.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvKeyboard.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvSlider.h"
#include "UI/Components/LVGL/LvTextArea.h"
#include "UI/Core/View.h"
#include <functional>

namespace UI
{
	class Slider : public LvContainer
	{
	  public:
		using value_changed_callback_t = std::function<void(float)>;
		using focused_callback_t = std::function<void(bool)>;

		enum class SendMode
		{
			VALUE_CONFIRMED, // Only run callback once the slider has been released or the buttons are pressed
			VALUE_CHANGED,	 // Run callback as soon as the slider is moved or the buttons are pressed
			DISABLED		 // Do not run the callback
		};

		enum OutOfRange
		{
			NONE = (1 << 0),  // Do not allow the value to go out of range
			UPPER = (1 << 1), // Allow the value to go out of range in the +ve direction
			LOWER = (1 << 2), // Allow the value to go out of range in the -ve direction
			BOTH = UPPER | LOWER
		};

		Slider(const std::string& name, LvObj& parent);

		std::string_view getLabel() const { return m_label.getText(); }
		float getValue() const { return m_value; }
		float getMin() const { return m_min; }
		float getMax() const { return m_max; }
		LvTextArea& getInput() { return m_input; }
		NumberPad* getNumberPad() const { return m_numberPad; }

		bool isFocused() const { return m_focused; }

		void setOutOfRangeMode(OutOfRange mode);
		void setLabel(std::string_view text);
		void setIncrementValue(float value);
		void setRange(float min, float max);
		void setValue(float value);
		void setDefaultValue(float value);
		void setSendMode(SendMode mode) { m_sendMode = mode; }
		void setLongPressedEnabled(bool enabled) { m_longPressEnabled = enabled; }
		void setNumberPad(NumberPad* numberPad) { m_numberPad = numberPad; }
		void setValueChangedCallback(value_changed_callback_t callback) { m_valueChangedCallback = callback; }

		/**
		 * @brief Set a callback to run when the focus state of the text input changes
		 */
		void setFocusedCallback(focused_callback_t callback) { m_focusedCallback = callback; }

	  protected:
		static void onValueChanged(lv_event_t* e);
		static void onInputEvent(lv_event_t* e);

		bool boundValue(float& value);
		int32_t normaliseValue(float value) const;
		void updateText();
		bool hasDefaultValue() const { return !std::isnan(m_defaultValue); }

	  private:
		LvLabel m_label{"label", getRoot()};
		LvContainer m_sliderCont{"slider_cont", getRoot()};

		Button m_decrement{"decrement", m_sliderCont};
		LvSlider m_slider{"slider", m_sliderCont};
		Button m_increment{"increment", m_sliderCont};
		LvTextArea m_input{"input", m_sliderCont};
		Button m_reset{"reset", getRoot()};

		float m_incrementValue = 1;
		NumberPad* m_numberPad = nullptr;

		float m_min;
		float m_max;
		float m_value;
		float m_defaultValue = std::numeric_limits<float>::quiet_NaN();
		bool m_focused = false;
		bool m_longPressEnabled = true;
		SendMode m_sendMode = SendMode::VALUE_CONFIRMED;
		OutOfRange m_outOfRangeMode = OutOfRange::NONE;
		value_changed_callback_t m_valueChangedCallback;
		focused_callback_t m_focusedCallback;
	};
} // namespace UI
