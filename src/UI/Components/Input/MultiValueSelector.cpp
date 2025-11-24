/*
 * MultiValueSelector.cpp
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#include "MultiValueSelector.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"
#include "utils/UnitSystem.h"

#define ID_MVS_SELECTED_SUFFIX ":selected"
#define ID_MVS_VALUES_SUFFIX ":values"

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
		m_valueDisplay.addEventCallback(
			[](lv_event_t* e)
			{
				auto& mvs = *static_cast<MultiValueSelector*>(lv_event_get_user_data(e));
				auto np = mvs.getNumberPad();
				if (np == nullptr)
				{
					LOG_DBG("No numberpad set");
					return;
				}

				openModal(np);
				np->setHeader(_("multi_value_selector.numberpad_input_header"));
				np->setValue(mvs.getValue());
				np->setMinValue(mvs.getMinValue());
				np->setMaxValue(mvs.getMaxValue());
				np->setConfirmCallback([mvs_ptr = &mvs](float value) { mvs_ptr->setValue(value); });
			},
			LV_EVENT_CLICKED,
			this);

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
		value = std::clamp(value, getMinValue(), getMaxValue());
		if (value == m_value)
			return;
		m_value = value;
		m_valueDisplay.setText(fmt::format("{:g}", m_value));

		saveSelected();
	}

	float MultiValueSelector::getValue() const
	{
		return m_value;
	}

	void MultiValueSelector::setIncrement(float increment)
	{
		m_incrementValue = increment;
	}

	void MultiValueSelector::setValueBtn(size_t index, float value)
	{
		UI_LOCK();
		if (index >= m_currentValues.size())
		{
			LOG_ERROR("Index {:d} is out of bounds", index);
			return;
		}

		m_currentValues[index] = value;
		if (auto btn = m_valueBtns.getItem(index))
		{
			btn->setText(fmt::format("{:g}", value));
		}
		saveValues();
	}

	void MultiValueSelector::setValueBtns(const std::vector<float>& values)
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
									 btn->addStyle(Themes::getLvglStyles().long_press, 0);
									 btn->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(index)));
									 btn->addClickedCallback(
										 [](lv_event_t* e)
										 {
											 auto& mvs = *static_cast<MultiValueSelector*>(lv_event_get_user_data(e));
											 auto btn = LvObj::fromPtr(lv_event_get_target_obj(e));
											 assert(btn);
											 size_t index =
												 static_cast<size_t>(reinterpret_cast<uintptr_t>(btn->getUserData()));
											 mvs.setValue(mvs.m_currentValues.at(index));
										 },
										 this);
									 btn->addEventCallback(
										 [](lv_event_t* e)
										 {
											 auto& mvs = *static_cast<MultiValueSelector*>(lv_event_get_user_data(e));

											 auto np = mvs.getNumberPad();
											 if (np == nullptr)
											 {
												 LOG_DBG("No numberpad set");
												 return;
											 }

											 auto btn = LvObj::fromPtr(lv_event_get_target_obj(e));
											 assert(btn);
											 size_t index =
												 static_cast<size_t>(reinterpret_cast<uintptr_t>(btn->getUserData()));

											 if (index >= mvs.m_currentValues.size())
												 return;

											 float value = mvs.m_currentValues.at(index);
											 openModal(np);
											 np->setHeader(_("multi_value_selector.numberpad_new_value_header"));
											 np->setValue(value);
											 np->setMinValue(mvs.getMinValue());
											 np->setMaxValue(mvs.getMaxValue());
											 np->setConfirmCallback(
												 [mvs_ptr = &mvs, index](float value)
												 {
													 mvs_ptr->setValueBtn(index, value);
													 mvs_ptr->setValue(value);
												 });
										 },
										 LV_EVENT_LONG_PRESSED,
										 this);
									 //  btn->setMinWidth(LV_SIZE_CONTENT);
									 return btn;
								 });

		m_valueBtns.iterateListItems([&values](size_t index, Button& btn)
									 { btn.setText(fmt::format("{:g}", values.at(index))); });
		saveValues();
	}

	void MultiValueSelector::setMinValue(float value)
	{
		m_minValue = value;
		setValue(getValue()); // refresh with new limit
	}

	void MultiValueSelector::setMaxValue(float value)
	{
		m_maxValue = value;
		setValue(getValue()); // refresh with new limit
	}

	void MultiValueSelector::setStorageKey(std::string_view key)
	{
		if (key.empty())
		{
			m_storageKey.clear();
			return;
		}

		m_storageKey = fmt::format("ui:mvs:{:s}", key);

		/* Load values */
		auto values = StorageHelper::getData<std::vector<float>>(m_storageKey + ID_MVS_VALUES_SUFFIX, {});
		if (!values.empty())
		{
			setValueBtns(values);
		}
		float selected = StorageHelper::getData<float>(m_storageKey + ID_MVS_SELECTED_SUFFIX, 0.0f);
		if (selected != 0.0f)
		{
			setValue(selected);
		}
	}

	void MultiValueSelector::saveValues() const
	{
		if (m_storageKey.empty())
			return;

		StorageHelper::setData<std::vector<float>>(m_storageKey + ID_MVS_VALUES_SUFFIX, m_currentValues);
	}

	void MultiValueSelector::saveSelected() const
	{
		if (m_storageKey.empty())
			return;

		StorageHelper::setData<float>(m_storageKey + ID_MVS_SELECTED_SUFFIX, m_value);
	}
} // namespace UI
