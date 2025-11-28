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

	GenericAxisControl::GenericAxisControl(const std::string& name, LvObj& parent)
		: LvObj(lv_obj_create, name, parent)
		, m_label("label", getRoot())
		, m_incrementButton("increment", getRoot())
		, m_homeButton("home", getRoot())
		, m_decrementButton("decrement", getRoot())
	{
		UI_LOCK();

		addStyle(Themes::getLvglStyles().pad_large);

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_label.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_label.setMinWidth(LV_SIZE_CONTENT);
		m_label.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
		m_label.addClickedCallback(onLabelClick, this);

		m_incrementButton.setWidth(LV_PCT(100));
		m_homeButton.setWidth(LV_PCT(100));
		m_decrementButton.setWidth(LV_PCT(100));

		m_incrementButton.setFlexGrow(1);
		m_homeButton.setFlexGrow(1);
		m_decrementButton.setFlexGrow(1);

		m_incrementButton.setIcon("increment.png");
		m_homeButton.setIcon("home_axis.png");
		m_decrementButton.setIcon("decrement.png");

		m_incrementButton.addClickedCallback(onJogBtn, this);
		m_homeButton.addClickedCallback(onHomeBtn, this);
		m_decrementButton.addClickedCallback(onJogBtn, this);

		m_label.addStyle(Themes::getLvglStyles().input);
		m_label.addStyle(Themes::getLvglStyles().pad_base);

		m_incrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_decrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);

		m_homeButton.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);

		updateLabel();
	}

	void GenericAxisControl::setAxisLetter(const char letter)
	{
		UI_LOCK();
		m_axisLetter = letter;

		m_homeButton.setText(std::string(1, m_axisLetter));
		updateLabel();
	}

	void GenericAxisControl::setAxisPosition(float value)
	{
		UI_LOCK();
		m_axisPosition = value;
		updateLabel();
	}

	void GenericAxisControl::setDisabled(bool disabled)
	{
		UI_LOCK();
		setJogDisabled(disabled);
		setHomeDisabled(disabled);
	}

	void GenericAxisControl::setJogDisabled(bool disabled)
	{
		UI_LOCK();
		m_label.setState(LV_STATE_DISABLED, disabled);
		m_incrementButton.setDisabled(disabled);
		m_decrementButton.setDisabled(disabled);
	}

	void GenericAxisControl::setHomeDisabled(bool disabled)
	{
		UI_LOCK();
		m_homeButton.setDisabled(disabled);
	}

	void GenericAxisControl::setJogCallback(jog_cb_t cb)
	{
		UI_LOCK();
		m_positionCallback = std::move(cb);
	}

	void GenericAxisControl::setHomeCallback(home_cb_t cb)
	{
		UI_LOCK();
		m_homeCallback = std::move(cb);
	}

	void GenericAxisControl::setLabelCallback(label_cb_t cb)
	{
		UI_LOCK();
		m_labelCallback = std::move(cb);
	}

	void GenericAxisControl::onJogBtn(lv_event_t* event)
	{
		UI_LOCK();
		auto* control = static_cast<GenericAxisControl*>(lv_event_get_user_data(event));

		lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(event));
		bool forward = target == control->m_decrementButton.getButton() ? false : true;
		if (control && control->m_positionCallback && control->m_axisLetter != '\0')
		{
			control->m_positionCallback(control->m_axisLetter, forward);
		}
	}

	void GenericAxisControl::onHomeBtn(lv_event_t* event)
	{
		UI_LOCK();
		auto* control = static_cast<GenericAxisControl*>(lv_event_get_user_data(event));
		if (control && control->m_homeCallback && control->m_axisLetter != '\0')
		{
			control->m_homeCallback(control->m_axisLetter);
		}
	}

	void GenericAxisControl::onLabelClick(lv_event_t* event)
	{
		UI_LOCK();
		auto control = static_cast<GenericAxisControl*>(lv_event_get_user_data(event));
		if (control && control->m_labelCallback && control->m_axisLetter != '\0')
		{
			control->m_labelCallback(control->m_axisLetter, control->m_axisPosition);
		}
	}

	void GenericAxisControl::updateLabel()
	{
		UI_LOCK();
		std::string labelText = fmt::format("{}: {:g}", m_axisLetter, m_axisPosition);
		m_label.setText(labelText);
	}
} // namespace UI
