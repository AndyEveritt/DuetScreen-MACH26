/*
 * Graph.cpp
 *
 *  Created on: 2025-01-20
 *      Author: Andy Everitt
 */

#include "Graph.h"
#include "Debug.h"

namespace UI
{
	static constexpr lv_coord_t s_scaleSize = 30;

	Graph::Graph(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_chart(lv_chart_create(getCont()))
		, m_vCont(lv_obj_create(getCont()))
		, m_hCont(lv_obj_create(getCont()))
		, m_vScale(lv_scale_create(m_vCont))
		, m_hScale(lv_scale_create(m_hCont))
	{
		Lock lock;

		// Chart
		// Horizontal scale
		lv_obj_set_style_pad_ver(m_hCont, 0, 0);
		lv_obj_set_style_pad_hor(m_hCont, 10, 0);
		lv_obj_set_style_border_opa(m_hCont, LV_OPA_0, LV_PART_MAIN);
		lv_obj_set_flex_flow(m_hCont, LV_FLEX_FLOW_ROW_REVERSE);
		lv_obj_set_flex_grow(m_hScale, 1);
		lv_obj_t* hPad = lv_obj_create(m_hCont);
		lv_obj_set_size(hPad, s_scaleSize - 2 * 10, LV_SIZE_CONTENT);
		lv_obj_set_style_pad_all(hPad, 0, 0);
		lv_obj_remove_flag(hPad, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_style_border_opa(hPad, LV_OPA_0, LV_PART_MAIN);
		lv_obj_set_style_bg_opa(m_hCont, LV_OPA_0, LV_PART_MAIN);
		lv_obj_set_style_opa(hPad, LV_OPA_0, LV_PART_MAIN);
		lv_obj_align(m_hCont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
		lv_obj_set_size(m_hCont, LV_PCT(100), LV_SIZE_CONTENT);
		lv_scale_set_mode(m_hScale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
		lv_obj_set_height(m_hScale, s_scaleSize);
		lv_obj_align(m_hScale, LV_ALIGN_BOTTOM_LEFT, s_scaleSize, 0);
		lv_scale_set_label_show(m_hScale, true);
		// lv_scale_set_range(m_hScale, 0, 300);
		// lv_scale_set_total_tick_count(m_hScale, 21);
		// lv_scale_set_major_tick_every(m_hScale, 4);

		// Vertical scale
		lv_obj_set_style_pad_hor(m_vCont, 0, 0);
		lv_obj_set_style_pad_ver(m_vCont, 10, 0);
		lv_obj_set_style_border_opa(m_vCont, LV_OPA_0, LV_PART_MAIN);
		lv_obj_set_flex_flow(m_vCont, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_grow(m_vScale, 1);
		lv_obj_t* vPad = lv_obj_create(m_vCont);
		lv_obj_set_size(vPad, LV_SIZE_CONTENT, s_scaleSize - 2 * 10);
		lv_obj_set_style_pad_all(vPad, 0, 0);
		lv_obj_remove_flag(vPad, LV_OBJ_FLAG_SCROLLABLE);
		lv_obj_set_style_border_opa(vPad, LV_OPA_0, LV_PART_MAIN);
		lv_obj_set_style_bg_opa(m_vCont, LV_OPA_0, LV_PART_MAIN);
		lv_obj_set_style_opa(vPad, LV_OPA_0, LV_PART_MAIN);
		lv_obj_align(m_vCont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
		lv_obj_set_size(m_vCont, LV_SIZE_CONTENT, LV_PCT(100));
		lv_scale_set_mode(m_vScale, LV_SCALE_MODE_VERTICAL_LEFT);
		lv_obj_set_width(m_vScale, s_scaleSize);
		lv_obj_align(m_vScale, LV_ALIGN_BOTTOM_LEFT, 0, -s_scaleSize);
		lv_scale_set_label_show(m_vScale, true);
		lv_scale_set_range(m_vScale, 0, 300);
		lv_scale_set_total_tick_count(m_vScale, 21);
		lv_scale_set_major_tick_every(m_vScale, 4);

		// Chart
		lv_obj_align(m_chart, LV_ALIGN_TOP_RIGHT, 0, 0);
		lv_obj_set_size(m_chart,
						lv_obj_get_width(getCont()) - lv_obj_get_width(m_vScale),
						lv_obj_get_height(getCont()) - lv_obj_get_height(m_hScale));
	}
} // namespace UI
