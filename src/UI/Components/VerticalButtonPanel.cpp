/*
 * VerticalButtonPanel.cpp
 *
 *  Created on: 2025-02-28
 *      Author: Andy Everitt
 */

#include "VerticalButtonPanel.h"
#include "Debug.h"

namespace UI
{
	VerticalButtonPanel::VerticalButtonPanel(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_reset(name + "_reset", getCont(), "")
		, m_increment(name + "_increment", getCont(), "")
		, m_decrement(name + "_decrement", getCont(), "")
		, m_valueCont(lv_obj_create(getCont()))
		, m_values{Button(name + "_value1", m_valueCont, ""), Button(name + "_value2", m_valueCont, "")}
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

		for (auto& v : m_values)
		{
			v.setCheckable(true);
			v.setChecked(false);
			v.setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(&v - m_values)));
			v.setCallback(
				[](lv_event_t* e)
				{
					VerticalButtonPanel* panel = static_cast<VerticalButtonPanel*>(lv_event_get_user_data(e));
					lv_obj_t* btn = static_cast<lv_obj_t*>(lv_event_get_target(e));
					panel->setSelectedValueIndex(
						static_cast<uint8_t>(reinterpret_cast<uintptr_t>(lv_obj_get_user_data(btn))));
				},
				LV_EVENT_CLICKED,
				this);
		}
		m_values[m_selectedValueIndex].setChecked(true);

		m_decrement.setCallback(
			[](lv_event_t* e)
			{
				VerticalButtonPanel* panel = static_cast<VerticalButtonPanel*>(lv_event_get_user_data(e));
				if (panel->m_valueChangeCallback)
				{
					panel->m_valueChangeCallback(-panel->getSelectedValue());
				}
			},
			LV_EVENT_CLICKED,
			this);

		m_increment.setCallback(
			[](lv_event_t* e)
			{
				VerticalButtonPanel* panel = static_cast<VerticalButtonPanel*>(lv_event_get_user_data(e));
				if (panel->m_valueChangeCallback)
				{
					panel->m_valueChangeCallback(panel->getSelectedValue());
				}
			},
			LV_EVENT_CLICKED,
			this);

		m_reset.setCallback(
			[](lv_event_t* e)
			{
				VerticalButtonPanel* panel = static_cast<VerticalButtonPanel*>(lv_event_get_user_data(e));
				if (panel->m_resetCallback)
				{
					panel->m_resetCallback();
				}
			},
			LV_EVENT_CLICKED,
			this);
	}

	void VerticalButtonPanel::setIncrementLabel(const char* label)
	{
		m_increment.setText(label);
	}

	void VerticalButtonPanel::setDecrementLabel(const char* label)
	{
		m_decrement.setText(label);
	}

	void VerticalButtonPanel::setResetLabel(const char* label)
	{
		m_reset.setText(label);
	}

	void VerticalButtonPanel::setValueLabelFmt(const std::string& fmt)
	{
		m_fmt = fmt;
		m_values[0].setText(utils::format(m_fmt.c_str(), m_incrementValues[0]).c_str());
		m_values[1].setText(utils::format(m_fmt.c_str(), m_incrementValues[1]).c_str());
	}

	void VerticalButtonPanel::setIncrementValues(const std::array<float, 2>& values)
	{
		m_incrementValues[0] = values[0];
		m_incrementValues[1] = values[1];

		m_values[0].setText(utils::format(m_fmt.c_str(), m_incrementValues[0]).c_str());
		m_values[1].setText(utils::format(m_fmt.c_str(), m_incrementValues[1]).c_str());
	}

	float VerticalButtonPanel::getSelectedValue() const
	{
		return m_incrementValues[m_selectedValueIndex];
	}

	void VerticalButtonPanel::setSelectedValueIndex(uint8_t index)
	{
		if (index >= 2)
		{
			return;
		}
		m_values[m_selectedValueIndex].setChecked(false); // uncheck the current value
		m_selectedValueIndex = index;
		m_values[m_selectedValueIndex].setChecked(true); // check the new value
	}

	void VerticalButtonPanel::setValueChangeCallback(std::function<void(float)> callback)
	{
		m_valueChangeCallback = callback;
	}

	void VerticalButtonPanel::setResetCallback(std::function<void()> callback)
	{
		m_resetCallback = callback;
	}
} // namespace UI
