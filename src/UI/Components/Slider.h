/*
 * Slider.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
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

		Slider(const std::string& name, lv_obj_t* parent, layout_t layout)
			: LvObj(lv_obj_create, name, parent, layout)
			, m_label(lv_label_create(getCont()))
			, m_sliderCont(lv_obj_create(getCont()))
			, m_decrement("slider_decrement", m_sliderCont, LV_SYMBOL_MINUS)
			, m_slider(lv_slider_create(m_sliderCont))
			, m_increment("slider_increment", m_sliderCont, LV_SYMBOL_PLUS)
			, m_input(lv_textarea_create(m_sliderCont))
			, m_incrementValue(1)
			, m_keyboard(nullptr)
		{
			UI_LOCK();
			lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
			lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
			lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

			lv_obj_set_size(m_label, LV_PCT(100), LV_SIZE_CONTENT);
			lv_obj_set_size(m_sliderCont, LV_PCT(100), LV_SIZE_CONTENT);

			lv_obj_set_layout(m_sliderCont, LV_LAYOUT_FLEX);
			lv_obj_set_flex_flow(m_sliderCont, LV_FLEX_FLOW_ROW);
			lv_obj_set_flex_align(m_sliderCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

			for (size_t i = 0; i < lv_obj_get_child_cnt(m_sliderCont); i++)
			{
				lv_obj_t* child = lv_obj_get_child(m_sliderCont, i);
				lv_obj_set_height(child, LV_SIZE_CONTENT);
				lv_obj_set_style_pad_all(child, 2, 0);
			}
			lv_obj_set_width(m_decrement.getCont(), LV_SIZE_CONTENT);
			lv_obj_set_width(m_increment.getCont(), LV_SIZE_CONTENT);
			lv_obj_set_width(m_input, 50);
			lv_obj_set_flex_grow(m_slider, 1);

			m_decrement.addClickedCallback(
				[](lv_event_t* e)
				{
					UI_LOCK();
					lv_event_code_t code = lv_event_get_code(e);
					Slider* slider = static_cast<Slider*>(lv_event_get_user_data(e));

					if (code == LV_EVENT_PRESSED ||
						(code == LV_EVENT_LONG_PRESSED_REPEAT && slider->m_longPressEnabled))
					{
						slider->setValue(slider->getValue() - slider->m_incrementValue);
					}
				},
				this);

			m_increment.addClickedCallback(
				[](lv_event_t* e)
				{
					UI_LOCK();
					lv_event_code_t code = lv_event_get_code(e);
					Slider* slider = static_cast<Slider*>(lv_event_get_user_data(e));

					if (code == LV_EVENT_PRESSED ||
						(code == LV_EVENT_LONG_PRESSED_REPEAT && slider->m_longPressEnabled))
					{
						slider->setValue(slider->getValue() + slider->m_incrementValue);
					}
				},
				this);

			lv_textarea_set_one_line(m_input, true);
			lv_textarea_set_accepted_chars(m_input, "0123456789");
			lv_textarea_set_max_length(m_input, 4);
			lv_textarea_set_cursor_click_pos(m_input, false);
			lv_obj_set_style_text_align(m_input, LV_TEXT_ALIGN_CENTER, 0);
			updateText();
			lv_obj_add_event_cb(m_slider, onValueChanged, LV_EVENT_ALL, this);
			lv_obj_add_event_cb(m_input, onInputEvent, LV_EVENT_ALL, this);
		}

		int32_t getValue() const { return m_value; }
		int32_t getMin() const
		{
			UI_LOCK();
			return lv_slider_get_min_value(m_slider);
		}
		int32_t getMax() const
		{
			UI_LOCK();
			return lv_slider_get_max_value(m_slider);
		}
		lv_obj_t* getInput() const { return m_input; }

		bool isFocused() const { return m_focused; }

		void setOutOfRangeMode(OutOfRange mode)
		{
			UI_LOCK();
			switch (mode)
			{
			case OutOfRange::NONE:
			case OutOfRange::UPPER:
				lv_textarea_set_accepted_chars(m_input, getMin() < 0 ? "-0123456789" : "0123456789");
				break;
			case OutOfRange::LOWER:
			case OutOfRange::BOTH:
				lv_textarea_set_accepted_chars(m_input, "-0123456789");
				break;
			}
			m_outOfRangeMode = mode;
		}
		void setLabel(const char* text)
		{
			UI_LOCK();
			lv_obj_set_flag(m_label, LV_OBJ_FLAG_HIDDEN, text == nullptr);
			lv_label_set_text(m_label, text);
		}
		void setIncrementValue(int32_t value) {}
		void setRange(int32_t min, int32_t max)
		{
			UI_LOCK();
			lv_slider_set_range(m_slider, min, max);
		}
		void setValue(int32_t value)
		{
			UI_LOCK();
			boundValue(value);
			m_value = value;
			lv_slider_set_value(m_slider, value, LV_ANIM_ON);

			if (!lv_obj_has_state(m_input, LV_STATE_FOCUSED))
			{
				updateText();
			}

			if (m_valueChangedCallback)
			{
				m_valueChangedCallback(getValue());
			}
		}
		void setSendMode(SendMode mode) { m_sendMode = mode; }
		void setLongPressedEnabled(bool enabled) { m_longPressEnabled = enabled; }
		void setKeyboard(lv_obj_t* keyboard) { m_keyboard = keyboard; }
		void setValueChangedCallback(std::function<void(int32_t)> callback) { m_valueChangedCallback = callback; }
		void setFocusedCallback(std::function<void(bool)> callback) { m_focusedCallback = callback; }

	  protected:
		static void onValueChanged(lv_event_t* e)
		{
			UI_LOCK();
			lv_event_code_t code = lv_event_get_code(e);
			Slider* slider = static_cast<Slider*>(lv_event_get_user_data(e));

			switch (code)
			{
			case LV_EVENT_FOCUSED:
				slider->m_focused = true;
				break;
			case LV_EVENT_VALUE_CHANGED:
				slider->m_value = lv_slider_get_value(slider->m_slider);
				if (slider->boundValue(slider->m_value))
				{
					// Should never reach here
					slider->setValue(slider->m_value);
				}
				if (slider->m_sendMode == SendMode::VALUE_CHANGED && slider->m_valueChangedCallback)
				{
					slider->m_valueChangedCallback(slider->getValue());
				}
				if (!lv_obj_has_state(slider->m_input, LV_STATE_FOCUSED))
				{
					slider->updateText();
				}
				break;

			case LV_EVENT_RELEASED:
				slider->m_focused = false;
				if (slider->m_valueChangedCallback)
				{
					slider->m_valueChangedCallback(slider->getValue());
				}
				slider->updateText();
				break;
			}
		}

		static void onInputEvent(lv_event_t* e)
		{
			UI_LOCK();
			lv_event_code_t code = lv_event_get_code(e);
			Slider* slider = static_cast<Slider*>(lv_event_get_user_data(e));
			switch (code)
			{
			case LV_EVENT_PRESSED:
			case LV_EVENT_FOCUSED:
			{
				slider->m_focused = true;
				if (slider->m_keyboard)
				{
					lv_keyboard_set_textarea(slider->m_keyboard, slider->m_input);
					if (slider->m_focusedCallback)
					{
						slider->m_focusedCallback(true);
					}
				}
				break;
			}
			case LV_EVENT_DEFOCUSED:
			{
				slider->m_focused = false;
				if (slider->m_keyboard)
				{
					lv_keyboard_set_textarea(slider->m_keyboard, nullptr);
					if (slider->m_focusedCallback)
					{
						slider->m_focusedCallback(false);
					}
					slider->updateText();
				}
				break;
			}
			case LV_EVENT_READY:
			{
				int32_t value = atoi(lv_textarea_get_text(slider->m_input));
				slider->setValue(value);
				break;
			}
			}
		}

		bool boundValue(int32_t& value)
		{
			UI_LOCK();
			bool outOfRange = false;
			switch (m_outOfRangeMode)
			{
			case OutOfRange::NONE:
				outOfRange = value < getMin() || value > getMax();
				value = std::clamp(value, getMin(), getMax());
				m_decrement.setInvalid(value == getMin());
				m_increment.setInvalid(value == getMax());
				break;
			case OutOfRange::BOTH:
				m_decrement.setInvalid(false);
				m_increment.setInvalid(false);
				break;
			case OutOfRange::UPPER:
				outOfRange = value < getMin();
				value = std::max(value, getMin());
				m_decrement.setInvalid(value == getMin());
				m_increment.setInvalid(false);
				break;
			case OutOfRange::LOWER:
				outOfRange = value > getMax();
				value = std::min(value, getMax());
				m_decrement.setInvalid(false);
				m_increment.setInvalid(value == getMax());
				break;
			}
			return outOfRange;
		}

		void updateText()
		{
			UI_LOCK();
			lv_textarea_set_text(m_input, std::to_string(getValue()).c_str());
		}

		lv_obj_t* m_label;
		lv_obj_t* m_sliderCont;

		Button m_decrement;
		lv_obj_t* m_slider;
		Button m_increment;
		lv_obj_t* m_input;

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
