/*
 * FineTune.cpp
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#include "FineTune.h"
#include "Debug.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	FineTune::FineTune(lv_obj_t* parent)
		: View("fine_tune", parent, layout_t(0, 0, 100, 100))
		, m_babystep(getCont())
		, m_speed(getCont())
		, m_flow(getCont())
	{
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(getCont(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_height(child, LV_PCT(100));
			lv_obj_set_flex_grow(child, 1);
		}

		m_babystep.setIncrementLabel(_("fine_tune_babystep_increment"));
		m_babystep.setDecrementLabel(_("fine_tune_babystep_decrement"));
		m_babystep.setResetLabel(utils::format(_("fine_tune_babystep_reset"), 0).c_str());
		m_babystep.setValueLabels({"0.01", "0.05"});

		m_speed.setIncrementLabel(_("fine_tune_speed_increment"));
		m_speed.setDecrementLabel(_("fine_tune_speed_decrement"));
		m_speed.setResetLabel(utils::format(_("fine_tune_speed_reset"), 100).c_str());
		m_speed.setValueLabels({"5%", "25%"});

		m_flow.setIncrementLabel(_("fine_tune_flow_increment"));
		m_flow.setDecrementLabel(_("fine_tune_flow_decrement"));
		m_flow.setResetLabel(utils::format(_("fine_tune_flow_reset"), 100).c_str());
		m_flow.setValueLabels({"1%", "2%"});
	}

	void FineTune::setBabyStepValue(float value)
	{
		m_babystep.setResetLabel(utils::format(_("fine_tune_babystep_reset"), value).c_str());
	}

	void FineTune::setSpeedValue(float value)
	{
		m_speed.setResetLabel(utils::format(_("fine_tune_speed_reset"), value).c_str());
	}

	void FineTune::setFlowValue(float value)
	{
		m_flow.setResetLabel(utils::format(_("fine_tune_flow_reset"), value).c_str());
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
