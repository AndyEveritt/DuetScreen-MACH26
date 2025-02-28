/*
 * FineTune.cpp
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#include "FineTune.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"
#include <algorithm>

namespace UI
{
	FineTune::FineTune(lv_obj_t* parent)
		: View("fine_tune", parent, layout_t(0, 0, 100, 100))
		, m_babystep(getCont())
		, m_sliderCont(lv_obj_create(getCont()))
		, m_speed("fine_tune_speed", m_sliderCont, layout_t(0, 0, 100, LV_SIZE_CONTENT))
		, m_extruderLabel(lv_label_create(m_sliderCont))
		, m_extruderCont(lv_obj_create(m_sliderCont))
		, m_fanLabel(lv_label_create(m_sliderCont))
		, m_fanCont(lv_obj_create(m_sliderCont))
		, m_keyboard(lv_keyboard_create(getCont()))
	{
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_height(child, LV_PCT(100));
		}
		lv_obj_set_flex_grow(m_babystep.getCont(), 2);
		lv_obj_set_flex_grow(m_sliderCont, 5);
		lv_obj_set_flex_grow(m_keyboard, 6);

		lv_obj_set_style_max_width(m_babystep.getCont(), 200, 0);

		lv_obj_set_layout(m_sliderCont, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_sliderCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_sliderCont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (size_t i = 0; i < lv_obj_get_child_cnt(m_sliderCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_sliderCont, i);
			lv_obj_set_size(child, LV_PCT(100), LV_SIZE_CONTENT);
		}

		lv_keyboard_set_mode(m_keyboard, LV_KEYBOARD_MODE_NUMBER);
		lv_obj_add_flag(m_keyboard, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_event_cb(
			m_keyboard,
			[](lv_event_t* e)
			{
				FineTune* view = static_cast<FineTune*>(lv_event_get_user_data(e));
				view->showKeyboard(false);
			},
			LV_EVENT_CANCEL,
			this);

		m_babystep.setIncrementLabel(_("fine_tune_babystep_increment"));
		m_babystep.setDecrementLabel(_("fine_tune_babystep_decrement"));
		m_babystep.setResetLabel(utils::format(_("fine_tune_babystep_reset"), 0).c_str());
		m_babystep.setValueLabels({"0.01", "0.05"});

		m_speed.setLabel(_("fine_tune_speed_factor"));
		m_speed.setKeyboard(m_keyboard);
		m_speed.setFocusedCallback([this](bool focused) { showKeyboard(focused); });
		m_speed.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_speed.setRange(1, 200);
		m_speed.setValueChangedCallback([this](int32_t value) { m_presenter.setSpeedFactor(value); });

		lv_obj_add_event_cb(
			m_sliderCont,
			[](lv_event_t* e)
			{
				FineTune* view = static_cast<FineTune*>(lv_event_get_user_data(e));
				view->showKeyboard(false);
			},
			LV_EVENT_SCROLL,
			this);

		lv_label_set_text(m_extruderLabel, _("fine_tune_extruder_header"));
		lv_obj_set_layout(m_extruderCont, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_extruderCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_extruderCont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		lv_label_set_text(m_fanLabel, _("fine_tune_fan_header"));
		lv_obj_set_layout(m_fanCont, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_fanCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(m_fanCont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	}

	void FineTune::setBabyStepValue(float value)
	{
		m_babystep.setResetLabel(utils::format(_("fine_tune_babystep_reset"), value).c_str());
	}

	void FineTune::setSpeedValue(uint32_t value)
	{
		m_speed.setValue(value);
	}

	/**
	 * @brief
	 * @param count
	 */
	void FineTune::setExtruderCount(size_t count)
	{
		if (count == getExtruderCount())
		{
			return;
		}

		if (count < getExtruderCount())
		{
			m_extruders.resize(count);
			return;
		}

		m_extruders.reserve(count);
		for (size_t i = getExtruderCount(); i < count; ++i)
		{
			m_extruders.emplace_back(
				std::make_shared<Slider>("extruder_slider", m_extruderCont, layout_t(0, 0, 100, LV_SIZE_CONTENT)));
			Slider& slider = *m_extruders.back();

			slider.setKeyboard(m_keyboard);
			slider.setFocusedCallback([this](bool focused) { showKeyboard(focused); });
			slider.setRange(0, 200);
			slider.setOutOfRangeMode(Slider::OutOfRange::UPPER);
			slider.setValueChangedCallback([this, i](int32_t value) { m_presenter.setExtruderFactor(i, value); });
		}
	}

	void FineTune::setFanCount(size_t count)
	{
		if (count == getFanCount())
		{
			return;
		}

		if (count < getFanCount())
		{
			m_fans.resize(count);
			return;
		}

		m_fans.reserve(count);
		for (size_t i = getFanCount(); i < count; ++i)
		{
			m_fans.emplace_back(
				std::make_shared<Slider>("fan_slider", m_fanCont, layout_t(0, 0, 100, LV_SIZE_CONTENT)));
			Slider& slider = *m_fans.back();

			slider.setKeyboard(m_keyboard);
			slider.setFocusedCallback([this](bool focused) { showKeyboard(focused); });
			slider.setValueChangedCallback([this, i](int32_t value) { m_presenter.setFanValue(i, value); });
		}
	}

	void FineTune::setExtruderLabel(size_t index, const char* label)
	{
		if (index >= m_extruders.size())
		{
			return;
		}
		m_extruders[index]->setLabel(label);
	}

	void FineTune::setExtruderValue(size_t index, uint32_t value)
	{
		if (index >= m_extruders.size() || m_extruders[index]->isFocused())
		{
			return;
		}
		m_extruders[index]->setValue(value);
	}

	void FineTune::setFanLabel(size_t index, const char* label)
	{
		if (index >= m_fans.size())
		{
			return;
		}
		m_fans[index]->setLabel(label);
	}

	void FineTune::setFanValue(size_t index, uint32_t value)
	{
		if (index >= m_fans.size() || m_fans[index]->isFocused())
		{
			return;
		}

		m_fans[index]->setValue(value);
	}

	void FineTune::showKeyboard(bool show)
	{
		lv_obj_set_flag(m_keyboard, LV_OBJ_FLAG_HIDDEN, !show);
	}

	FineTune::Item::Item(lv_obj_t* parent)
		: BaseView("fine_tune_item", parent)
		, m_reset("fine_tune_item_reset", getCont(), "")
		, m_increment("fine_tune_item_increment", getCont(), "")
		, m_decrement("fine_tune_item_decrement", getCont(), "")
		, m_valueCont(lv_obj_create(getCont()))
		, m_values{Button("fine_tune_item_value1", m_valueCont, ""), Button("fine_tune_item_value2", m_valueCont, "")}
	{
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_width(child, LV_PCT(100));
		}

		lv_obj_set_flex_grow(m_reset.getCont(), 1);
		lv_obj_set_flex_grow(m_increment.getCont(), 3);
		lv_obj_set_flex_grow(m_decrement.getCont(), 3);
		lv_obj_set_flex_grow(m_valueCont, 2);

		lv_obj_set_layout(m_valueCont, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(m_valueCont, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(m_valueCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_all(m_valueCont, 2, 0);

		for (size_t i = 0; i < lv_obj_get_child_cnt(m_valueCont); i++)
		{
			lv_obj_t* child = lv_obj_get_child(m_valueCont, i);
			lv_obj_set_height(child, LV_PCT(100));
			lv_obj_set_flex_grow(child, 1);
		}
	}

	void FineTune::Item::setIncrementLabel(const char* label)
	{
		m_increment.setText(label);
	}

	void FineTune::Item::setDecrementLabel(const char* label)
	{
		m_decrement.setText(label);
	}

	void FineTune::Item::setResetLabel(const char* label)
	{
		m_reset.setText(label);
	}

	void FineTune::Item::setValueLabels(const std::array<const char*, 2>& labels)
	{
		m_values[0].setText(labels[0]);
		m_values[1].setText(labels[1]);
	}
} // namespace UI
