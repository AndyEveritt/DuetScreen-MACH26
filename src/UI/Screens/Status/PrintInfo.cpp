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
		: LvContainer("status_print_info", parent, layout_t(0, 0, 100, 100))
		, m_speed("speed", getRoot())
		, m_speedMultiplier("speed_multiplier", getRoot())
		, m_flowRate("flow_rate", getRoot())
		, m_flowMultiplier("flow_multiplier", getRoot())
		, m_elapsedTime("elapsed_time", getRoot())
		, m_remainingTime("remaining_time", getRoot())
		, m_layer("layer", getRoot())
		, m_fanSpeed("fan_speed", getRoot())
		, m_speedInfo(getParent())
	{
		UI_LOCK();
		lv_obj_set_layout(getRoot(), LV_LAYOUT_GRID);
		lv_obj_set_grid_align(getRoot(), LV_GRID_ALIGN_SPACE_AROUND, LV_GRID_ALIGN_SPACE_AROUND);
		static int32_t printInfoColDsc[] = {LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_FR(5), LV_GRID_TEMPLATE_LAST};
		static int32_t printInfoRowDsc[] = {
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		lv_obj_set_grid_dsc_array(getRoot(), printInfoColDsc, printInfoRowDsc);
		lv_obj_set_grid_cell(m_speedMultiplier, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_speed, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_flowMultiplier, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
		lv_obj_set_grid_cell(m_flowRate, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
		lv_obj_set_grid_cell(m_elapsedTime, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 3, 1);
		lv_obj_set_grid_cell(m_remainingTime, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 4, 1);
		lv_obj_set_grid_cell(m_layer, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
		lv_obj_set_grid_cell(m_fanSpeed, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getRoot()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getRoot(), i);
			lv_obj_set_align(child, LV_ALIGN_LEFT_MID);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_style_min_height(child, 20, 0);
			lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
			// lv_obj_set_style_text_align(child, LV_TEXT_ALIGN_LEFT, 0);
			// lv_obj_set_height(child, LV_SIZE_CONTENT);
		}

		m_speedInfo.hide();

		lv_obj_add_event_cb(m_speed, openSubView, LV_EVENT_CLICKED, &m_speedInfo);
		lv_obj_add_event_cb(m_speedMultiplier, openSubView, LV_EVENT_CLICKED, &m_speedInfo);

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

	void PrintInfo::openSubView(lv_event_t* e)
	{
		UI_LOCK();
		LvObj* view = static_cast<LvObj*>(lv_event_get_user_data(e));
		view->show(true);
	}

	void PrintInfo::updateExtrusionRate(float feedrate, float volumetric)
	{
		UI_LOCK();
		m_flowRate.setText(fmt::format(fmt::runtime(_("status_extrusion_speed")), feedrate));
		m_flowRate.setText(fmt::format(fmt::runtime(_("status_flow_rate")), volumetric));
	}

	void PrintInfo::updateSpeed(float topSpeed, float requestedSpeed)
	{
		m_speed.setText(fmt::format(fmt::runtime(_("status_speed")), topSpeed, requestedSpeed));
		m_speedInfo.updateSpeed(topSpeed, requestedSpeed);
	}

	void PrintInfo::updateFlowMultiplier(uint32_t multiplier)
	{
		m_flowMultiplier.setText(fmt::format(fmt::runtime(_("status_flow_multiplier")), multiplier));
	}

	void PrintInfo::updateSpeedMultiplier(uint32_t multiplier)
	{
		m_speedMultiplier.setText(fmt::format(fmt::runtime(_("status_speed_multiplier")), multiplier));
		m_speedInfo.updateSpeedMultiplier(multiplier);
	}

	void PrintInfo::updateElapsedTime(uint32_t elapsed)
	{
		int32_t hours = elapsed / 3600;
		int32_t minutes = (elapsed % 3600) / 60;
		int32_t seconds = elapsed % 60;
		std::string elapsedStr = fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, seconds);
		m_elapsedTime.setText(fmt::format(fmt::runtime(_("status_elapsed_time")), elapsedStr));
	}

	void PrintInfo::updateRemainingTime(uint32_t remaining)
	{
		UI_LOCK();
		int32_t hours = remaining / 3600;
		int32_t minutes = (remaining % 3600) / 60;
		int32_t seconds = remaining % 60;
		std::string remainingStr = fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, seconds);
		m_remainingTime.setText(fmt::format(fmt::runtime(_("status_remaining_time")), remainingStr));
	}

	void PrintInfo::updateLayer(float height, float maxHeight)
	{
		UI_LOCK();
		m_layer.setText(fmt::format(fmt::runtime(_("status_layer")), height, maxHeight));
		m_speedInfo.updatePrintHeight(maxHeight);
		m_speedInfo.updatePrintHeight(height);
	}

	void PrintInfo::updateFanSpeed(uint32_t speed)
	{
		m_fanSpeed.setText(fmt::format(fmt::runtime(_("status_fan_speed")), speed));
	}

	void PrintInfo::updateAcceleration(uint32_t acceleration)
	{
		m_speedInfo.updateAcceleration(acceleration);
	}

	void PrintInfo::updatePosition(float x, float y, float z)
	{
		m_speedInfo.updatePosition(x, y, z);
	}

	void PrintInfo::updateZOffset(float offset)
	{
		m_speedInfo.updateZOffset(offset);
	}

	void PrintInfo::updateLayerNumber(uint32_t layer)
	{
		m_speedInfo.updateLayerNumber(layer);
	}

	PrintInfo::SpeedInfo::SpeedInfo(lv_obj_t* parent)
		: LvObj(lv_obj_create, "status_speed_info", parent, layout_t(0, 0, 100, 100))
		, m_speed("speed", getRoot())
		, m_speedMultiplier("speed_multiplier", getRoot())
		, m_acceleration("acceleration", getRoot())
		, m_position("position", getRoot())
		, m_z_offset("z_offset", getRoot())
		, m_z_height("z_height", getRoot())
		, m_layer("layer", getRoot())
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_COLUMN);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getRoot()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getRoot(), i);
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
		m_speed.setText(fmt::format(fmt::runtime(_("status_speed_detailed")), topSpeed, requestedSpeed));
	}

	void PrintInfo::SpeedInfo::updateSpeedMultiplier(uint32_t multiplier)
	{
		m_speedMultiplier.setText(fmt::format(fmt::runtime(_("status_speed_multiplier")), multiplier));
	}

	void PrintInfo::SpeedInfo::updateAcceleration(uint32_t acceleration)
	{
		m_acceleration.setText(fmt::format(fmt::runtime(_("status_acceleration")), acceleration));
	}

	void PrintInfo::SpeedInfo::updatePosition(float x, float y, float z)
	{
		m_position.setText(fmt::format(fmt::runtime(_("status_position")), x, y, z));
	}

	void PrintInfo::SpeedInfo::updateZOffset(float offset)
	{
		m_z_offset.setText(fmt::format(fmt::runtime(_("status_z_offset")), offset));
	}

	void PrintInfo::SpeedInfo::updatePrintHeight(float height)
	{
		m_z_height.setText(fmt::format(fmt::runtime(_("status_print_height")), height));
	}

	void PrintInfo::SpeedInfo::updateLayerNumber(uint32_t layer)
	{
		m_layer.setText(fmt::format(fmt::runtime(_("status_layer_number")), layer));
	}

} // namespace UI
