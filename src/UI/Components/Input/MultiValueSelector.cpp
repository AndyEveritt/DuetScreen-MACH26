/*
 * MultiValueSelector.cpp
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#include "MultiValueSelector.h"
#include "Debug.h"
#include "Storage.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"
#include "utils/StorageHelper.h"
#include "utils/UnitSystem.h"

#define ID_MVS_SELECTED_SUFFIX ":selected"
#define ID_MVS_VALUES_SUFFIX ":values"

namespace UI
{
	static StorageKeyRunTime<std::vector<float>> makeValuesStorageKey(const std::string& prefix)
	{
		return StorageKeyRunTime<std::vector<float>>(prefix + ID_MVS_VALUES_SUFFIX, {});
	}

	static StorageKeyRunTime<float> makeSelectedStorageKey(const std::string& prefix)
	{
		return StorageKeyRunTime<float>(prefix + ID_MVS_SELECTED_SUFFIX, std::numeric_limits<float>::quiet_NaN());
	}

	MultiValueSelector::MultiValueSelector(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		ZoneScoped;
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		m_label.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_label.setMinWidth(LV_SIZE_CONTENT);
		m_label.addStyle(Themes::getLvglStyles().bg_color_header);
		m_label.addStyle(Themes::getComponentStyles().list_header);
		m_label.addStyle(Themes::getComponentStyles().list_title);

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
		ZoneScoped;
		m_label.setVisible(!label.empty());
		m_label.setText(label);
	}

	void MultiValueSelector::setValue(float value)
	{
		ZoneScoped;
		value = std::clamp(value, getMinValue(), getMaxValue());
		if (value == m_value)
			return;
		m_value = value;
		m_valueDisplay.setText(fmt::format("{:g}", m_value));

		saveSelected();
	}

	float MultiValueSelector::getValue() const
	{
		ZoneScoped;
		return m_value;
	}

	void MultiValueSelector::setIncrement(float increment)
	{
		ZoneScoped;
		m_incrementValue = increment;
	}

	void MultiValueSelector::setValueBtn(size_t index, float value)
	{
		ZoneScoped;
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
		ZoneScoped;
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
		ZoneScoped;
		m_minValue = value;
		setValue(getValue()); // refresh with new limit
	}

	void MultiValueSelector::setMaxValue(float value)
	{
		ZoneScoped;
		m_maxValue = value;
		setValue(getValue()); // refresh with new limit
	}

	void MultiValueSelector::setStorageKey(std::string_view key)
	{
		ZoneScoped;
		if (key.empty())
		{
			m_storageKeyPrefix.clear();
			return;
		}

		m_storageKeyPrefix = key;

		/* Load values */
		auto values = StorageHelper::getData(makeValuesStorageKey(m_storageKeyPrefix));
		if (!values.empty())
		{
			setValueBtns(values);
		}
		float selected = StorageHelper::getData(makeSelectedStorageKey(m_storageKeyPrefix));
		if (!std::isnan(selected))
		{
			setValue(selected);
		}
	}

	void MultiValueSelector::saveValues() const
	{
		ZoneScoped;
		if (m_storageKeyPrefix.empty())
			return;

		StorageHelper::setData(makeValuesStorageKey(m_storageKeyPrefix), m_currentValues);
	}

	void MultiValueSelector::saveSelected() const
	{
		ZoneScoped;
		if (m_storageKeyPrefix.empty())
			return;

		StorageHelper::setData(makeSelectedStorageKey(m_storageKeyPrefix), m_value);
	}
} // namespace UI
