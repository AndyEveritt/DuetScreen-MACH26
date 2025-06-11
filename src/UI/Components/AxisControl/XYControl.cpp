/*
 * XYControl.cpp
 *
 *  Created on: 2025-06-10
 *      Author: Andy Everitt
 */

#include "XYControl.h"
#include "Debug.h"

namespace UI
{
	XYControl::XYControl(const std::string& name, lv_obj_t* parent, layout_t layout)
		: LvObj(lv_obj_create, name, parent, layout)
		, m_xLabel(name + "_x_label", getCont())
		, m_yLabel(name + "_y_label", getCont())
		, m_xIncrementButton(name + "_x_increment", getCont(), LV_SYMBOL_RIGHT)
		, m_xDecrementButton(name + "_x_decrement", getCont(), LV_SYMBOL_LEFT)
		, m_yIncrementButton(name + "_y_increment", getCont(), LV_SYMBOL_UP)
		, m_yDecrementButton(name + "_y_decrement", getCont(), LV_SYMBOL_DOWN)
		, m_homeXYButton(name + "_home_xy", getCont(), LV_SYMBOL_HOME)
		, m_homeXButton(name + "_home_x", getCont(), LV_SYMBOL_HOME)
		, m_homeYButton(name + "_home_y", getCont(), LV_SYMBOL_HOME)
	{
		UI_LOCK();
		setGridDsc(m_colDsc, m_rowDsc);
		setGridCell(m_xLabel, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_yLabel, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_xDecrementButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_xIncrementButton, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		setGridCell(m_yDecrementButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 3, 1);
		setGridCell(m_yIncrementButton, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 1, 1);

		m_xLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_yLabel.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		lv_obj_set_style_text_align(m_xLabel, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_yLabel, LV_TEXT_ALIGN_CENTER, 0);

		m_xIncrementButton.setCallback(onIncrementBtn, LV_EVENT_CLICKED, this);
		m_xDecrementButton.setCallback(onDecrementBtn, LV_EVENT_CLICKED, this);
		m_yIncrementButton.setCallback(onIncrementBtn, LV_EVENT_CLICKED, this);
		m_yDecrementButton.setCallback(onDecrementBtn, LV_EVENT_CLICKED, this);
		m_homeXYButton.setCallback(onHomeXYBtn, LV_EVENT_CLICKED, this);
		m_homeXButton.setCallback(onHomeXBtn, LV_EVENT_CLICKED, this);
		m_homeYButton.setCallback(onHomeYBtn, LV_EVENT_CLICKED, this);
	}

	void XYControl::onIncrementBtn(lv_event_t* event) {}

	void XYControl::onDecrementBtn(lv_event_t* event) {}

	void XYControl::onHomeXYBtn(lv_event_t* event) {}

	void XYControl::onHomeXBtn(lv_event_t* event) {}

	void XYControl::onHomeYBtn(lv_event_t* event) {}
} // namespace UI
