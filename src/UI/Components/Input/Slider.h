/*
 * Slider.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvSlider.h"
#include "UI/Components/LVGL/LvTextArea.h"
#include "UI/Core/View.h"
#include <functional>

namespace UI
{
	class Slider : public LvObj
	{
	  public:
		enum class SendMode
		{
			VALUE_CONFIRMED, // Only run callback once the slider has been released or the buttons are pressed
			VALUE_CHANGED,	 // Run callback as soon as the slider is moved or the buttons are pressed
		};

		enum class OutOfRange
		{
			NONE,  // Do not allow the value to go out of range
			BOTH,  // Allow the value to go out of range in both directions
			UPPER, // Allow the value to go out of range in the +ve direction
			LOWER, // Allow the value to go out of range in the -ve direction
		};

		Slider(const std::string& name, lv_obj_t* parent, layout_t layout);

		int32_t getValue() const { return m_value; }
		int32_t getMin() const { return m_slider.getMinValue(); }
		int32_t getMax() const { return m_slider.getMaxValue(); }
		lv_obj_t* getInput() const { return m_input; }

		bool isFocused() const { return m_focused; }

		void setOutOfRangeMode(OutOfRange mode);
		void setLabel(const std::string& text);
		void setIncrementValue(int32_t value) { m_incrementValue = value; }
		void setRange(int32_t min, int32_t max) { m_slider.setRange(min, max); }
		void setValue(int32_t value);
		void setSendMode(SendMode mode) { m_sendMode = mode; }
		void setLongPressedEnabled(bool enabled) { m_longPressEnabled = enabled; }
		void setKeyboard(lv_obj_t* keyboard) { m_keyboard = keyboard; }
		void setValueChangedCallback(std::function<void(int32_t)> callback) { m_valueChangedCallback = callback; }
		void setFocusedCallback(std::function<void(bool)> callback) { m_focusedCallback = callback; }

	  protected:
		static void onValueChanged(lv_event_t* e);
		static void onInputEvent(lv_event_t* e);

		bool boundValue(int32_t& value);
		void updateText();

		LvLabel m_label;
		LvContainer m_sliderCont;

		Button m_decrement;
		LvSlider m_slider;
		Button m_increment;
		LvTextArea m_input;

		int32_t m_incrementValue;
		lv_obj_t* m_keyboard;

		int32_t m_value;
		bool m_focused = false;
		bool m_longPressEnabled = true;
		SendMode m_sendMode = SendMode::VALUE_CONFIRMED;
		OutOfRange m_outOfRangeMode = OutOfRange::NONE;
		std::function<void(int32_t)> m_valueChangedCallback;
		std::function<void(int32_t)> m_focusedCallback;
	};
} // namespace UI
