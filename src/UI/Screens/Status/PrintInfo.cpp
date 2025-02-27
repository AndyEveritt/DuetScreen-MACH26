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
		: BaseView("status_print_info", parent, layout_t(0, 0, 100, 100))
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
	}

	bool PrintInfo::back()
	{
		for (BaseView* subView : {&m_speedInfo})
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
		Lock lock;
		auto& view = HomeView::instance().getExtrudeView();
		openScreen(&view, true);
	}

	void PrintInfo::openSubView(lv_event_t* e)
	{
		Lock lock;
		BaseView* view = static_cast<BaseView*>(lv_event_get_user_data(e));
		view->show();
	}

	void PrintInfo::updateToolTemp(float temp, int32_t target)
	{
		Lock lock;
		lv_label_set_text(m_toolTemp, utils::format(_("status_tool_temp"), temp, target).c_str());
	}

	void PrintInfo::updateBedTemp(float temp, int32_t target)
	{
		Lock lock;
		lv_label_set_text(m_bedTemp, utils::format(_("status_bed_temp"), temp, target).c_str());
	}

	void PrintInfo::updateExtrusionRate(float feedrate, float volumetric)
	{
		Lock lock;
		lv_label_set_text(m_speed, utils::format(_("status_speed"), feedrate).c_str());
		lv_label_set_text(m_flowRate, utils::format(_("status_flow_rate"), volumetric).c_str());
	}

	void PrintInfo::updateSpeed(float topSpeed, float requestedSpeed)
	{
		Lock lock;
		lv_label_set_text(m_speed, utils::format(_("status_speed"), topSpeed, requestedSpeed).c_str());
	}

	void PrintInfo::updateFlowMultiplier(uint32_t multiplier)
	{
		Lock lock;
		lv_label_set_text(m_flowMultiplier, utils::format(_("status_flow_multiplier"), multiplier).c_str());
	}

	void PrintInfo::updateSpeedMultiplier(uint32_t multiplier)
	{
		Lock lock;
		lv_label_set_text(m_speedMultiplier, utils::format(_("status_speed_multiplier"), multiplier).c_str());
	}

	void PrintInfo::updateElapsedTime(uint32_t elapsed)
	{
		Lock lock;
		int32_t hours = elapsed / 3600;
		int32_t minutes = (elapsed % 3600) / 60;
		int32_t seconds = elapsed % 60;
		std::string elapsedStr = utils::format("%02d:%02d:%02d", hours, minutes, seconds);
		lv_label_set_text(m_elapsedTime, utils::format(_("status_elapsed_time"), elapsedStr.c_str()).c_str());
	}

	void PrintInfo::updateRemainingTime(uint32_t remaining)
	{
		Lock lock;
		int32_t hours = remaining / 3600;
		int32_t minutes = (remaining % 3600) / 60;
		int32_t seconds = remaining % 60;
		std::string remainingStr = utils::format("%02d:%02d:%02d", hours, minutes, seconds);
		lv_label_set_text(m_remainingTime, utils::format(_("status_remaining_time"), remainingStr.c_str()).c_str());
	}

	void PrintInfo::updateLayer(float height, float maxHeight)
	{
		Lock lock;
		lv_label_set_text(m_layer, utils::format(_("status_layer"), height, maxHeight).c_str());
	}

	void PrintInfo::updateFanSpeed(uint32_t speed)
	{
		Lock lock;
		lv_label_set_text(m_fanSpeed, utils::format(_("status_fan_speed"), speed).c_str());
	}

	PrintInfo::SpeedInfo::SpeedInfo(lv_obj_t* parent)
		: BaseView("status_speed_info", parent, layout_t(0, 0, 100, 100))
		, m_speed(lv_label_create(getCont()))
		, m_speedMultiplier(lv_label_create(getCont()))
		, m_acceleration(lv_label_create(getCont()))
		, m_position(lv_label_create(getCont()))
		, m_z_offset(lv_label_create(getCont()))
		, m_z_height(lv_label_create(getCont()))
		, m_layer(lv_label_create(getCont()))
	{
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_COLUMN);

		for (size_t i = 0; i < lv_obj_get_child_cnt(getCont()); i++)
		{
			lv_obj_t* child = lv_obj_get_child(getCont(), i);
			lv_obj_set_align(child, LV_ALIGN_LEFT_MID);
			lv_obj_set_width(child, LV_PCT(100));
			lv_obj_set_height(child, LV_SIZE_CONTENT);
		}
	}
} // namespace UI
