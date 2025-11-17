/*
 * MultiValueSelector.cpp
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#include "MultiValueSelector.h"
#include "Debug.h"

namespace UI
{
	MultiValueSelector::MultiValueSelector(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_topRow.setFlexFlow(LV_FLEX_FLOW_ROW);
		m_topRow.setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_topRow.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_topRow.setMinHeight(LV_SIZE_CONTENT);

		m_decrementBtn.setIcon("decrement.png");
		m_incrementBtn.setIcon("increment.png");
		m_decrementBtn.setHeight(LV_PCT(100));
		m_decrementBtn.setMinHeight(LV_SIZE_CONTENT);
		m_incrementBtn.setHeight(LV_PCT(100));
		m_incrementBtn.setMinHeight(LV_SIZE_CONTENT);

		m_valueDisplay.setHeight(LV_SIZE_CONTENT);
		// m_valueDisplay.setMinHeight(LV_SIZE_CONTENT);
		m_valueDisplay.setFlexGrow(1);
		m_valueDisplay.setOneLine(true);
		m_valueDisplay.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		m_valueBtns.setWidth(LV_PCT(100));
		m_valueBtns.setFlexGrow(1);
		m_valueBtns.setMinHeight(LV_SIZE_CONTENT);
		m_valueBtns.setListFlow(LV_FLEX_FLOW_ROW);
		m_valueBtns.getListContainer().setWidth(LV_PCT(100));
		m_valueBtns.getListContainer().setFlexGrow(1);
		m_valueBtns.getListContainer().setMinHeight(LV_SIZE_CONTENT);

		m_incrementBtn.addClickedCallback(
			[](lv_event_t* e)
			{
				auto& mvs = *static_cast<MultiValueSelector*>(lv_event_get_user_data(e));
				mvs.setValue(mvs.getValue() + mvs.m_incrementValue);
			},
			this);

		m_decrementBtn.addClickedCallback(
			[](lv_event_t* e)
			{
				auto& mvs = *static_cast<MultiValueSelector*>(lv_event_get_user_data(e));
				mvs.setValue(mvs.getValue() - mvs.m_incrementValue);
			},
			this);

		setValue(0.0f);
		setLabel("");
	}

	void MultiValueSelector::setLabel(const std::string& label)
	{
		m_label.setVisible(!label.empty());
		m_label.setText(label);
	}

	void MultiValueSelector::setValue(float value)
	{
		m_value = value;
		m_valueDisplay.setText(fmt::format("{:g}", m_value));
	}

	float MultiValueSelector::getValue() const
	{
		return m_value;
	}

	void MultiValueSelector::setIncrement(float increment)
	{
		m_incrementValue = increment;
	}

	void MultiValueSelector::setValues(const std::vector<float>& values)
	{
		UI_LOCK();
		m_currentValues = values;

		m_valueBtns.setItemCount(values.size(),
								 [this](size_t index, LvObj& parent)
								 {
									 auto btn = std::make_unique<Button>(std::to_string(index), parent);
									 btn->setHeight(LV_PCT(100));
									 btn->setMinHeight(LV_SIZE_CONTENT);
									 btn->setFlexGrow(1);
									 btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(index)));
									 btn->addClickedCallback(
										 [](lv_event_t* e)
										 {
											 auto& mvs = *static_cast<MultiValueSelector*>(lv_event_get_user_data(e));
											 auto btn = LvObj::fromPtr(lv_event_get_target_obj(e));
											 assert(btn);
											 mvs.setValue(mvs.m_currentValues.at(
												 static_cast<size_t>(reinterpret_cast<uintptr_t>(btn->getUserData()))));
										 },
										 this);
									 //  btn->setMinWidth(LV_SIZE_CONTENT);
									 return btn;
								 });

		m_valueBtns.iterateListItems([&values](size_t index, Button& btn)
									 { btn.setText(fmt::format("{:g}", values.at(index))); });
	}
} // namespace UI
