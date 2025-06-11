/*
 * GenericAxisControl.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "GenericAxisControl.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{

	GenericAxisControl::GenericAxisControl(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_obj_create, name, parent)
		, m_label(name + "_label", getCont())
		, m_incrementButton(name + "_increment", getCont(), LV_SYMBOL_PLUS)
		, m_homeButton(name + "_home", getCont(), LV_SYMBOL_HOME)
		, m_decrementButton(name + "_decrement", getCont(), LV_SYMBOL_MINUS)
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_label.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_label.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		m_incrementButton.setWidth(LV_PCT(100));
		m_homeButton.setWidth(LV_PCT(100));
		m_decrementButton.setWidth(LV_PCT(100));

		m_incrementButton.setFlexGrow(1);
		m_homeButton.setFlexGrow(1);
		m_decrementButton.setFlexGrow(1);

		m_incrementButton.addClickedCallback(onIncrementBtn, this);
		m_homeButton.addClickedCallback(onHomeBtn, this);
		m_decrementButton.addClickedCallback(onDecrementBtn, this);

		m_incrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_decrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);

		updateLabel();
	}

	void GenericAxisControl::setAxisLetter(std::string_view letter)
	{
		UI_LOCK();
		m_axisLetter = letter;

		m_homeButton.setText(LV_SYMBOL_HOME " " + m_axisLetter);
		updateLabel();
	}

	void GenericAxisControl::setAxisPosition(float value)
	{
		UI_LOCK();
		m_axisPosition = value;
		updateLabel();
	}

	void GenericAxisControl::setPositionCallback(position_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_positionCallback = std::move(cb);
		m_positionUserData = user_data;
	}

	void GenericAxisControl::setHomeCallback(home_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_homeCallback = std::move(cb);
		m_homeUserData = user_data;
	}

	void GenericAxisControl::onIncrementBtn(lv_event_t* event)
	{
		UI_LOCK();
		auto* control = static_cast<GenericAxisControl*>(lv_event_get_user_data(event));
		if (control && control->m_positionCallback)
		{
			control->m_positionCallback(true, control->m_positionUserData);
		}
	}

	void GenericAxisControl::onDecrementBtn(lv_event_t* event)
	{
		UI_LOCK();
		auto* control = static_cast<GenericAxisControl*>(lv_event_get_user_data(event));
		if (control && control->m_positionCallback)
		{
			control->m_positionCallback(false, control->m_positionUserData);
		}
	}

	void GenericAxisControl::onHomeBtn(lv_event_t* event)
	{
		UI_LOCK();
		auto* control = static_cast<GenericAxisControl*>(lv_event_get_user_data(event));
		if (control && control->m_homeCallback)
		{
			control->m_homeCallback(control->m_homeUserData);
		}
	}

	void GenericAxisControl::updateLabel()
	{
		UI_LOCK();
		std::string labelText = fmt::format("{}: {:g}", m_axisLetter, m_axisPosition);
		m_label.setText(labelText);
		lv_obj_set_style_text_align(m_label, LV_TEXT_ALIGN_CENTER, 0);
	}
} // namespace UI
