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
	const std::string XYControl::sm_xAxisLetter = "X";
	const std::string XYControl::sm_yAxisLetter = "Y";

	XYControl::XYControl(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_obj_create, name, parent)
		, m_xLabel(name + "_x_label", getCont())
		, m_yLabel(name + "_y_label", getCont())
		, m_xIncrementButton(name + "_x_increment", getCont(), LV_SYMBOL_RIGHT)
		, m_xDecrementButton(name + "_x_decrement", getCont(), LV_SYMBOL_LEFT)
		, m_yIncrementButton(name + "_y_increment", getCont(), LV_SYMBOL_UP)
		, m_yDecrementButton(name + "_y_decrement", getCont(), LV_SYMBOL_DOWN)
		, m_homeXYButton(name + "_home_xy", getCont(), LV_SYMBOL_HOME " " + sm_xAxisLetter + sm_yAxisLetter)
		, m_homeXButton(name + "_home_x", getCont(), LV_SYMBOL_HOME " " + sm_xAxisLetter)
		, m_homeYButton(name + "_home_y", getCont(), LV_SYMBOL_HOME " " + sm_yAxisLetter)
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

		m_xIncrementButton.addClickedCallback(onIncrementBtn, this);
		m_xDecrementButton.addClickedCallback(onDecrementBtn, this);
		m_yIncrementButton.addClickedCallback(onIncrementBtn, this);
		m_yDecrementButton.addClickedCallback(onDecrementBtn, this);
		m_homeXYButton.addClickedCallback(onHomeXYBtn, this);
		m_homeXButton.addClickedCallback(onHomeXBtn, this);
		m_homeYButton.addClickedCallback(onHomeYBtn, this);

		m_xIncrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_xDecrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_yIncrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_yDecrementButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeXYButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeXButton.addStyle(Themes::getLvglStyles().actionBtn, 0);
		m_homeYButton.addStyle(Themes::getLvglStyles().actionBtn, 0);

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

	void XYControl::setXPositionCallback(position_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_xPositionCallback = std::move(cb);
		m_xPositionUserData = user_data;
	}

	void XYControl::setYPositionCallback(position_cb_t cb, void* user_data)
	{
		UI_LOCK();
		m_yPositionCallback = std::move(cb);
		m_yPositionUserData = user_data;
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

	void XYControl::onIncrementBtn(lv_event_t* event) {}

	void XYControl::onDecrementBtn(lv_event_t* event) {}

	void XYControl::onHomeXYBtn(lv_event_t* event) {}

	void XYControl::onHomeXBtn(lv_event_t* event) {}

	void XYControl::onHomeYBtn(lv_event_t* event) {}

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

	void XYControl::updateLabel(Label& label, const std::string& axisLetter, float position)
	{
		UI_LOCK();
		std::string labelText = fmt::format("{}: {:g}", axisLetter, position);
		label.setText(labelText);
	}
} // namespace UI
