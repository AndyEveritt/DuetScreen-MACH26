/*
 * StatusSubViews.cpp
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#include "PrintInfo.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	PrintInfo::PrintInfo(lv_obj_t* parent)
		: LvObj(lv_obj_create, "status_print_info", parent, layout_t(0, 0, 100, 100))
		, m_toolTemp(lv_label_create(getCont()))
		, m_bedTemp(lv_label_create(getCont()))
		, m_speed(lv_label_create(getCont()))
		, m_speedMultiplier(lv_label_create(getCont()))
		, m_flowRate(lv_label_create(getCont()))
		, m_flowMultiplier(lv_label_create(getCont()))
		, m_elapsedTime(lv_label_create(getCont()))
		, m_remainingTime(lv_label_create(getCont()))
		, m_layer(lv_label_create(getCont()))
		, m_fanSpeed(lv_label_create(getCont()))
		, m_speedInfo(getParent())
	{
		UI_LOCK();
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_align(getCont(), LV_GRID_ALIGN_SPACE_AROUND, LV_GRID_ALIGN_SPACE_AROUND);
		static int32_t printInfoColDsc[] = {LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_FR(5), LV_GRID_TEMPLATE_LAST};
		static int32_t printInfoRowDsc[] = {
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		lv_obj_set_grid_dsc_array(getCont(), printInfoColDsc, printInfoRowDsc);
		lv_obj_set_grid_cell(m_toolTemp, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
		lv_obj_set_grid_cell(m_bedTemp, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
		lv_obj_set_grid_cell(m_speedMultiplier, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_speed, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_flowMultiplier, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
		lv_obj_set_grid_cell(m_flowRate, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
		lv_obj_set_grid_cell(m_elapsedTime, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 3, 1);
		lv_obj_set_grid_cell(m_remainingTime, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 4, 1);
		lv_obj_set_grid_cell(m_layer, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_fanSpeed, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_align(child, LV_ALIGN_LEFT_MID);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_style_min_height(child, 20, 0);
			lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
			// lv_obj_set_style_text_align(child, LV_TEXT_ALIGN_LEFT, 0);
			// lv_obj_set_height(child, LV_SIZE_CONTENT);
		}

		m_speedInfo.hide();

		lv_obj_add_event_cb(m_toolTemp, openExtrudeView, LV_EVENT_CLICKED, nullptr);
		lv_obj_add_event_cb(m_speed, openSubView, LV_EVENT_CLICKED, &m_speedInfo);
		lv_obj_add_event_cb(m_speedMultiplier, openSubView, LV_EVENT_CLICKED, &m_speedInfo);

		lv_obj_add_style(m_toolTemp, Themes::getLvglStyles().input, 0);
		lv_obj_add_style(m_speed, Themes::getLvglStyles().input, 0);
		lv_obj_add_style(m_speedMultiplier, Themes::getLvglStyles().input, 0);
	}

	bool PrintInfo::back()
	{
		UI_LOCK();
		for (LvObj* subView : {&m_speedInfo})
		{
			if (subView->isVisible())
			{
				subView->hide();
				return true;
			}
		}
		return false;
	}

	void PrintInfo::openExtrudeView(lv_event_t* e)
	{
		UI_LOCK();
		auto& view = HomeView::instance().getExtrudeView();
		openScreen(&view, true);
	}

	void PrintInfo::openSubView(lv_event_t* e)
	{
		UI_LOCK();
		LvObj* view = static_cast<LvObj*>(lv_event_get_user_data(e));
		view->show();
	}

	void PrintInfo::updateToolTemp(float temp, int32_t target)
	{
		UI_LOCK();
		lv_label_set_text(m_toolTemp, utils::format(_("status_tool_temp"), temp, target).c_str());
	}

	void PrintInfo::updateBedTemp(float temp, int32_t target)
	{
		UI_LOCK();
		lv_label_set_text(m_bedTemp, utils::format(_("status_bed_temp"), temp, target).c_str());
	}

	void PrintInfo::updateExtrusionRate(float feedrate, float volumetric)
	{
		UI_LOCK();
		lv_label_set_text(m_speed, utils::format(_("status_speed"), feedrate).c_str());
		lv_label_set_text(m_flowRate, utils::format(_("status_flow_rate"), volumetric).c_str());
	}

	void PrintInfo::updateSpeed(float topSpeed, float requestedSpeed)
	{
		UI_LOCK();
		lv_label_set_text(m_speed, utils::format(_("status_speed"), topSpeed, requestedSpeed).c_str());
		m_speedInfo.updateSpeed(topSpeed, requestedSpeed);
	}

	void PrintInfo::updateFlowMultiplier(uint32_t multiplier)
	{
		UI_LOCK();
		lv_label_set_text(m_flowMultiplier, utils::format(_("status_flow_multiplier"), multiplier).c_str());
	}

	void PrintInfo::updateSpeedMultiplier(uint32_t multiplier)
	{
		UI_LOCK();
		lv_label_set_text(m_speedMultiplier, utils::format(_("status_speed_multiplier"), multiplier).c_str());
		m_speedInfo.updateSpeedMultiplier(multiplier);
	}

	void PrintInfo::updateElapsedTime(uint32_t elapsed)
	{
		UI_LOCK();
		int32_t hours = elapsed / 3600;
		int32_t minutes = (elapsed % 3600) / 60;
		int32_t seconds = elapsed % 60;
		std::string elapsedStr = utils::format("%02d:%02d:%02d", hours, minutes, seconds);
		lv_label_set_text(m_elapsedTime, utils::format(_("status_elapsed_time"), elapsedStr.c_str()).c_str());
	}

	void PrintInfo::updateRemainingTime(uint32_t remaining)
	{
		UI_LOCK();
		int32_t hours = remaining / 3600;
		int32_t minutes = (remaining % 3600) / 60;
		int32_t seconds = remaining % 60;
		std::string remainingStr = utils::format("%02d:%02d:%02d", hours, minutes, seconds);
		lv_label_set_text(m_remainingTime, utils::format(_("status_remaining_time"), remainingStr.c_str()).c_str());
	}

	void PrintInfo::updateLayer(float height, float maxHeight)
	{
		UI_LOCK();
		lv_label_set_text(m_layer, utils::format(_("status_layer"), height, maxHeight).c_str());
		m_speedInfo.updatePrintHeight(maxHeight);
		m_speedInfo.updatePrintHeight(height);
	}

	void PrintInfo::updateFanSpeed(uint32_t speed)
	{
		UI_LOCK();
		lv_label_set_text(m_fanSpeed, utils::format(_("status_fan_speed"), speed).c_str());
	}

	void PrintInfo::updateAcceleration(uint32_t acceleration)
	{
		UI_LOCK();
		m_speedInfo.updateAcceleration(acceleration);
	}

	void PrintInfo::updatePosition(float x, float y, float z)
	{
		UI_LOCK();
		m_speedInfo.updatePosition(x, y, z);
	}

	void PrintInfo::updateZOffset(float offset)
	{
		UI_LOCK();
		m_speedInfo.updateZOffset(offset);
	}

	void PrintInfo::updateLayerNumber(uint32_t layer)
	{
		UI_LOCK();
		m_speedInfo.updateLayerNumber(layer);
	}

	PrintInfo::SpeedInfo::SpeedInfo(lv_obj_t* parent)
		: LvObj(lv_obj_create, "status_speed_info", parent, layout_t(0, 0, 100, 100))
		, m_speed(lv_label_create(getCont()))
		, m_speedMultiplier(lv_label_create(getCont()))
		, m_acceleration(lv_label_create(getCont()))
		, m_position(lv_label_create(getCont()))
		, m_z_offset(lv_label_create(getCont()))
		, m_z_height(lv_label_create(getCont()))
		, m_layer(lv_label_create(getCont()))
	{
		UI_LOCK();
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_align(child, LV_ALIGN_LEFT_MID);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_height(child, LV_SIZE_CONTENT);
		}

		updateSpeed(0, 0);
		updateSpeedMultiplier(0);
		updateAcceleration(0);
		updatePosition(0, 0, 0);
		updateZOffset(0);
		updatePrintHeight(0);
		updateLayerNumber(0);
	}

	void PrintInfo::SpeedInfo::updateSpeed(float topSpeed, float requestedSpeed)
	{
		UI_LOCK();
		lv_label_set_text(m_speed, utils::format(_("status_speed_detailed"), topSpeed, requestedSpeed).c_str());
	}

	void PrintInfo::SpeedInfo::updateSpeedMultiplier(uint32_t multiplier)
	{
		UI_LOCK();
		lv_label_set_text(m_speedMultiplier, utils::format(_("status_speed_multiplier"), multiplier).c_str());
	}

	void PrintInfo::SpeedInfo::updateAcceleration(uint32_t acceleration)
	{
		UI_LOCK();
		lv_label_set_text(m_acceleration, utils::format(_("status_acceleration"), acceleration).c_str());
	}

	void PrintInfo::SpeedInfo::updatePosition(float x, float y, float z)
	{
		UI_LOCK();
		lv_label_set_text(m_position, utils::format(_("status_position"), x, y, z).c_str());
	}

	void PrintInfo::SpeedInfo::updateZOffset(float offset)
	{
		UI_LOCK();
		lv_label_set_text(m_z_offset, utils::format(_("status_z_offset"), offset).c_str());
	}

	void PrintInfo::SpeedInfo::updatePrintHeight(float height)
	{
		UI_LOCK();
		lv_label_set_text(m_z_height, utils::format(_("status_print_height"), height).c_str());
	}

	void PrintInfo::SpeedInfo::updateLayerNumber(uint32_t layer)
	{
		UI_LOCK();
		lv_label_set_text(m_layer, utils::format(_("status_layer_number"), layer).c_str());
	}

} // namespace UI
