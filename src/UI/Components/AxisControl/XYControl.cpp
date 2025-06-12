/*
 * XYControl.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "XYControl.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	const char XYControl::sm_xAxisLetter = 'X';
	const char XYControl::sm_yAxisLetter = 'Y';

	XYControl::XYControl(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_obj_create, name, parent)
		, m_xLabel(name + "_x_label", getCont())
		, m_yLabel(name + "_y_label", getCont())
		, m_xIncrementButton(name + "_x_increment", getCont(), LV_SYMBOL_RIGHT)
		, m_xDecrementButton(name + "_x_decrement", getCont(), LV_SYMBOL_LEFT)
		, m_yIncrementButton(name + "_y_increment", getCont(), LV_SYMBOL_UP)
		, m_yDecrementButton(name + "_y_decrement", getCont(), LV_SYMBOL_DOWN)
		, m_homeXYButton(
			  name + "_home_xy", getCont(), fmt::format(LV_SYMBOL_HOME " {}{}", sm_xAxisLetter, sm_yAxisLetter))
		, m_homeXButton(name + "_home_x", getCont(), fmt::format(LV_SYMBOL_HOME " {}", sm_xAxisLetter))
		, m_homeYButton(name + "_home_y", getCont(), fmt::format(LV_SYMBOL_HOME " {}", sm_yAxisLetter))
	{
		UI_LOCK();
		setGridDsc(m_colDsc, m_rowDsc);
		setGridCell(m_xLabel, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_yLabel, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_xDecrementButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_xIncrementButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_yDecrementButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 3, 1);
		setGridCell(m_yIncrementButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_homeXYButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_homeXButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_homeYButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 3, 1);

		m_xLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_yLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		m_xLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
		m_yLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		m_xIncrementButton.addClickedCallback(onJogBtn, this);
		m_xDecrementButton.addClickedCallback(onJogBtn, this);
		m_yIncrementButton.addClickedCallback(onJogBtn, this);
		m_yDecrementButton.addClickedCallback(onJogBtn, this);
		m_homeXYButton.addClickedCallback(onHomeBtn, this);
		m_homeXButton.addClickedCallback(onHomeBtn, this);
		m_homeYButton.addClickedCallback(onHomeBtn, this);

		m_xIncrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_xDecrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_yIncrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_yDecrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);

		m_homeXYButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeXButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeYButton.addStyle(Themes::getLvglStyles().actionBtn, 0);

		m_homeXYButton.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);
		m_homeXButton.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);
		m_homeYButton.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);

		updateXLabel();
		updateYLabel();
	}

	void XYControl::setXPosition(float position)
	{
		UI_LOCK();
		m_xPosition = position;
		updateXLabel();
	}

	void XYControl::setYPosition(float position)
	{
		UI_LOCK();
		m_yPosition = position;
		updateYLabel();
	}

	void XYControl::setXHomed(bool homed)
	{
		UI_LOCK();
		m_homeXButton.setChecked(!homed);
		m_homeXYButton.setChecked(!homed || m_homeYButton.hasState(LV_STATE_CHECKED));
	}

	void XYControl::setYHomed(bool homed)
	{
		UI_LOCK();
		m_homeYButton.setChecked(!homed);
		m_homeXYButton.setChecked(!homed || m_homeXButton.hasState(LV_STATE_CHECKED));
	}

	void XYControl::setXDisabled(bool disabled)
	{
		UI_LOCK();
		setXJogDisabled(disabled);
		setXHomeDisabled(disabled);
	}

	void XYControl::setYDisabled(bool disabled)
	{
		UI_LOCK();
		setYJogDisabled(disabled);
		setYHomeDisabled(disabled);
	}

	void XYControl::setXJogDisabled(bool disabled)
	{
		UI_LOCK();
		m_xIncrementButton.setDisabled(disabled);
		m_xDecrementButton.setDisabled(disabled);
	}

	void XYControl::setYJogDisabled(bool disabled)
	{
		UI_LOCK();
		m_yIncrementButton.setDisabled(disabled);
		m_yDecrementButton.setDisabled(disabled);
	}

	void XYControl::setXHomeDisabled(bool disabled)
	{
		UI_LOCK();
		m_homeXButton.setDisabled(disabled);
		m_homeXYButton.setDisabled(disabled || m_homeYButton.hasState(LV_STATE_DISABLED));
	}

	void XYControl::setYHomeDisabled(bool disabled)
	{
		UI_LOCK();
		m_homeYButton.setDisabled(disabled);
		m_homeXYButton.setDisabled(disabled || m_homeXButton.hasState(LV_STATE_DISABLED));
	}

	void XYControl::setJogCallback(jog_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_jogCallback = std::move(cb);
		m_jogUserData = user_data;
	}

	void XYControl::setHomeXYCallback(home_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_homeXYCallback = std::move(cb);
		m_homeXYUserData = user_data;
	}

	void XYControl::setHomeXCallback(home_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_homeXCallback = std::move(cb);
		m_homeXUserData = user_data;
	}

	void XYControl::setHomeYCallback(home_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_homeYCallback = std::move(cb);
		m_homeYUserData = user_data;
	}

	void XYControl::onJogBtn(lv_event_t* event)
	{
		UI_LOCK();
		XYControl* control = static_cast<XYControl*>(lv_event_get_user_data(event));

		lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(event));

		char axisLetter = '\0';
		bool forward = true;

		if (target == control->m_xDecrementButton.getButton() || target == control->m_yDecrementButton.getButton())
		{
			forward = false;
		}

		if (target == control->m_xIncrementButton.getButton() || target == control->m_xDecrementButton.getButton())
		{
			axisLetter = control->sm_xAxisLetter;
		}
		else if (target == control->m_yIncrementButton.getButton() || target == control->m_yDecrementButton.getButton())
		{
			axisLetter = control->sm_yAxisLetter;
		}
		else
		{
			LOG_ERROR("Unknown increment button pressed");
			return;
		}

		if (control->m_jogCallback)
		{
			control->m_jogCallback(axisLetter, forward, control->m_jogUserData);
		}
	}

	void XYControl::onHomeBtn(lv_event_t* event)
	{
		UI_LOCK();
		XYControl* control = static_cast<XYControl*>(lv_event_get_user_data(event));

		lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(event));

		if (target == control->m_homeXButton.getButton())
		{
			if (control->m_homeXCallback)
			{
				control->m_homeXCallback(control->m_homeXUserData);
			}
		}
		else if (target == control->m_homeYButton.getButton())
		{
			if (control->m_homeYCallback)
			{
				control->m_homeYCallback(control->m_homeYUserData);
			}
		}
		else if (target == control->m_homeXYButton.getButton())
		{
			if (control->m_homeXYCallback)
			{
				control->m_homeXYCallback(control->m_homeXYUserData);
			}
		}
		else
		{
			LOG_ERROR("Unknown home button pressed");
			return;
		}
	}

	void XYControl::updateXLabel()
	{
		UI_LOCK();
		updateLabel(m_xLabel, sm_xAxisLetter, m_xPosition);
	}

	void XYControl::updateYLabel()
	{
		UI_LOCK();
		updateLabel(m_yLabel, sm_yAxisLetter, m_yPosition);
	}

	void XYControl::updateLabel(Label& label, const char axisLetter, const float position)
	{
		UI_LOCK();
		std::string labelText = fmt::format("{}: {:g}", axisLetter, position);
		label.setText(labelText);
	}
} // namespace UI
