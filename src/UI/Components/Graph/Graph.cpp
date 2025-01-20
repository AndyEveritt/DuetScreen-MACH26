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
		, m_vScale(lv_scale_create(getCont()))
		, m_hScale(lv_scale_create(getCont()))
	{
		Lock lock;

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		m_columnDsc[0] = s_scaleSize;
		m_columnDsc[1] = LV_GRID_FR(1);
		m_columnDsc[2] = LV_GRID_TEMPLATE_LAST;

		m_rowDsc[0] = LV_GRID_FR(1);
		m_rowDsc[1] = s_scaleSize;
		m_rowDsc[2] = LV_GRID_TEMPLATE_LAST;
		lv_obj_set_grid_dsc_array(getCont(), m_columnDsc, m_rowDsc);
		lv_obj_set_grid_cell(m_vScale, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_hScale, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_chart, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		lv_obj_set_style_pad_top(getCont(), 20, LV_PART_MAIN);
		lv_obj_set_style_pad_right(getCont(), 20, LV_PART_MAIN);
		lv_obj_set_style_pad_left(getCont(), 10, LV_PART_MAIN);
		lv_obj_set_style_pad_bottom(getCont(), 5, LV_PART_MAIN);

		// Chart
		// Horizontal scale
		lv_scale_set_mode(m_hScale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
		lv_scale_set_label_show(m_hScale, true);

		// Vertical scale
		lv_scale_set_mode(m_vScale, LV_SCALE_MODE_VERTICAL_LEFT);
		lv_scale_set_label_show(m_vScale, true);
		lv_scale_set_total_tick_count(m_vScale, 21);
		lv_scale_set_major_tick_every(m_vScale, 4);

		// Chart
		lv_obj_set_style_border_width(m_chart, 2, LV_PART_MAIN);
		lv_obj_set_style_border_color(m_chart, lv_color_hex(0x000000), LV_PART_MAIN);
		lv_obj_set_style_border_opa(m_chart, LV_OPA_100, LV_PART_MAIN);
		lv_chart_set_type(m_chart, LV_CHART_TYPE_LINE);
		lv_chart_set_update_mode(m_chart, LV_CHART_UPDATE_MODE_SHIFT);
	}

	Graph::range_t Graph::getXRange() const
	{
		range_t range;
		range.min = lv_scale_get_range_min_value(m_hScale);
		range.max = lv_scale_get_range_max_value(m_hScale);
		return range;
	}

	Graph::range_t Graph::getYRange() const
	{
		range_t range;
		range.min = lv_scale_get_range_min_value(m_vScale);
		range.max = lv_scale_get_range_max_value(m_vScale);
		return range;
	}

	void Graph::setXRange(Graph::range_t range)
	{
		lv_scale_set_range(m_hScale, range.min, range.max);
		lv_chart_set_range(m_chart, LV_CHART_AXIS_PRIMARY_X, range.min, range.max);
	}

	void Graph::setYRange(Graph::range_t range)
	{
		lv_scale_set_range(m_vScale, range.min, range.max);
		lv_chart_set_range(m_chart, LV_CHART_AXIS_PRIMARY_Y, range.min, range.max);
	}

	lv_chart_series_t* Graph::getSeries(const std::string& name)
	{
		auto it = m_series.find(name);
		if (it != m_series.end())
		{
			return it->second;
		}
		return nullptr;
	}

	bool Graph::createSeries(const std::string& name, lv_color_t color)
	{
		if (m_series.find(name) != m_series.end())
		{
			return false; // Series with this name already exists
		}

		lv_chart_series_t* series = lv_chart_add_series(m_chart, color, LV_CHART_AXIS_PRIMARY_Y);
		if (series)
		{
			m_series[name] = series;
			return true;
		}
		return false;
	}

	void Graph::clear()
	{
		for (auto& pair : m_series)
		{
			lv_chart_remove_series(m_chart, pair.second);
		}
		m_series.clear();
	}

	void Graph::clear(const std::string& seriesName)
	{
		lv_chart_series_t* series = getSeries(seriesName);
		if (series)
		{
			lv_chart_remove_series(m_chart, series);
			m_series.erase(seriesName);
		}
	}

	void Graph::addData(const std::string& seriesName, int32_t value)
	{
		lv_chart_series_t* series = getSeries(seriesName);
		if (series)
		{
			lv_chart_set_next_value(m_chart, series, value);
		}
	}
} // namespace UI
