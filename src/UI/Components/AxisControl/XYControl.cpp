/*
 * XYControl.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "XYControl.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"

namespace UI
{
	const char XYControl::sm_xAxisLetter = 'X';
	const char XYControl::sm_yAxisLetter = 'Y';

	XYControl::XYControl(const std::string& name, LvObj& parent)
		: LvObj(lv_obj_create, name, parent)
	{
		UI_LOCK();
		setGridDsc(m_colDsc, m_rowDsc);
		setGridCell(m_xLabel, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_yLabel, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_xDecrementButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_xIncrementButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_yDecrementButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 3, 1);
		setGridCell(m_yIncrementButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_homeAllButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_homeXYButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_homeXButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_homeYButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
		setGridCell(m_disableMotorsButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);

		m_xLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_yLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		m_xLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
		m_yLabel.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		m_homeXYButton.setText(fmt::format("{}{}", sm_xAxisLetter, sm_yAxisLetter));
		m_homeXButton.setText(fmt::format("{}", sm_xAxisLetter));
		m_homeYButton.setText(fmt::format("{}", sm_yAxisLetter));
		m_disableMotorsButton.setText(_("move.disable_motors"));

		m_xIncrementButton.setIcon("arrow_right.png");
		m_xDecrementButton.setIcon("arrow_left.png");
		m_yIncrementButton.setIcon("arrow_up.png");
		m_yDecrementButton.setIcon("arrow_down.png");

		m_homeAllButton.setText(_("move.home_all"));
		m_homeAllButton.setIcon("home_axis.png");
		m_homeXButton.setIcon("home_axis.png");
		m_homeYButton.setIcon("home_axis.png");
		m_homeXYButton.setIcon("home_axis.png");
		m_disableMotorsButton.setIcon("disable_motors.png");

		m_xLabel.addClickedCallback(onLabelEvent, this);
		m_yLabel.addClickedCallback(onLabelEvent, this);
		m_xIncrementButton.addClickedCallback(onJogBtn, this);
		m_xDecrementButton.addClickedCallback(onJogBtn, this);
		m_yIncrementButton.addClickedCallback(onJogBtn, this);
		m_yDecrementButton.addClickedCallback(onJogBtn, this);
		m_homeAllButton.addClickedCallback(onHomeBtn, this);
		m_homeXYButton.addClickedCallback(onHomeBtn, this);
		m_homeXButton.addClickedCallback(onHomeBtn, this);
		m_homeYButton.addClickedCallback(onHomeBtn, this);
		m_disableMotorsButton.addClickedCallback(
			[](lv_event_t* e)
			{
				XYControl& control = *static_cast<XYControl*>(lv_event_get_user_data(e));
				if (control.m_disableMotorsCallback)
				{
					control.m_disableMotorsCallback();
				}
			},
			this);

		m_xLabel.addStyle(Themes::getLvglStyles().input);
		m_yLabel.addStyle(Themes::getLvglStyles().input);
		m_xLabel.addStyle(Themes::getLvglStyles().pad_base);
		m_yLabel.addStyle(Themes::getLvglStyles().pad_base);

		m_xIncrementButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_xDecrementButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_yIncrementButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_yDecrementButton.addStyle(Themes::getLvglStyles().actionBtn);

		m_homeAllButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_homeXYButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_homeXButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_homeYButton.addStyle(Themes::getLvglStyles().actionBtn);
		m_disableMotorsButton.addStyle(Themes::getLvglStyles().actionBtn);

		m_homeAllButton.addStyle(Themes::getComponentStyles().unhomed, LV_STATE_CHECKED);
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
		m_xLabel.setState(LV_STATE_DISABLED, disabled);
		m_xIncrementButton.setDisabled(disabled);
		m_xDecrementButton.setDisabled(disabled);
	}

	void XYControl::setYJogDisabled(bool disabled)
	{
		UI_LOCK();
		m_yLabel.setState(LV_STATE_DISABLED, disabled);
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

	void XYControl::setJogCallback(jog_cb_t cb)
	{
		UI_LOCK();
		m_jogCallback = std::move(cb);
	}

	void XYControl::setHomeAllCallback(home_cb_t cb)
	{
		UI_LOCK();
		m_homeAllCallback = std::move(cb);
	}

	void XYControl::setHomeXYCallback(home_cb_t cb)
	{
		UI_LOCK();
		m_homeXYCallback = std::move(cb);
	}

	void XYControl::setHomeXCallback(home_cb_t cb)
	{
		UI_LOCK();
		m_homeXCallback = std::move(cb);
	}

	void XYControl::setHomeYCallback(home_cb_t cb)
	{
		UI_LOCK();
		m_homeYCallback = std::move(cb);
	}

	void XYControl::setDisableMotorsCallback(disable_cb_t cb)
	{
		UI_LOCK();
		m_disableMotorsCallback = std::move(cb);
	}

	void XYControl::setXLabelCallback(label_cb_t cb)
	{
		UI_LOCK();
		m_xLabelCallback = std::move(cb);
	}

	void XYControl::setYLabelCallback(label_cb_t cb)
	{
		UI_LOCK();
		m_yLabelCallback = std::move(cb);
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
			control->m_jogCallback(axisLetter, forward);
		}
	}

	void XYControl::onHomeBtn(lv_event_t* event)
	{
		UI_LOCK();
		XYControl* control = static_cast<XYControl*>(lv_event_get_user_data(event));

		LvObj* target = LvObj::fromPtr(lv_event_get_target_obj(event));

		if (target == &control->m_homeAllButton)
		{
			if (control->m_homeAllCallback)
			{
				control->m_homeAllCallback();
			}
		}
		else if (target == &control->m_homeXButton)
		{
			if (control->m_homeXCallback)
			{
				control->m_homeXCallback();
			}
		}
		else if (target == &control->m_homeYButton)
		{
			if (control->m_homeYCallback)
			{
				control->m_homeYCallback();
			}
		}
		else if (target == &control->m_homeXYButton)
		{
			if (control->m_homeXYCallback)
			{
				control->m_homeXYCallback();
			}
		}
		else
		{
			LOG_ERROR("Unknown home button pressed");
			return;
		}
	}

	void XYControl::onLabelEvent(lv_event_t* event)
	{
		UI_LOCK();
		XYControl* control = static_cast<XYControl*>(lv_event_get_user_data(event));

		auto* target = LvObj::fromPtr(lv_event_get_target_obj(event));

		if (target == &control->m_xLabel)
		{
			if (control->m_xLabelCallback)
			{
				control->m_xLabelCallback(control->m_xPosition);
			}
		}
		else if (target == &control->m_yLabel)
		{
			if (control->m_yLabelCallback)
			{
				control->m_yLabelCallback(control->m_yPosition);
			}
		}
		else
		{
			LOG_ERROR("Unknown label event");
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

	void XYControl::updateLabel(Button& label, const char axisLetter, const float position)
	{
		UI_LOCK();
		std::string labelText = fmt::format("{}: {:g}", axisLetter, position);
		label.setText(labelText);
	}
} // namespace UI
