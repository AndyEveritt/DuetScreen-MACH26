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
	static constexpr lv_coord_t s_legendSize = 100;
	static lv_color_t s_hiddenColor = lv_color_darken(lv_color_white(), 50);

	Graph::Graph(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_chart(lv_chart_create(getCont()))
		, m_vScale(lv_scale_create(getCont()))
		, m_hScale(lv_scale_create(getCont()))
		, m_legend(lv_obj_create(getCont()))
	{
		Lock lock;

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		m_columnDsc[0] = s_scaleSize;
		m_columnDsc[1] = LV_GRID_FR(1);
		m_columnDsc[2] = s_legendSize;
		m_columnDsc[3] = LV_GRID_TEMPLATE_LAST;

		m_rowDsc[0] = LV_GRID_FR(1);
		m_rowDsc[1] = s_scaleSize;
		m_rowDsc[2] = LV_GRID_TEMPLATE_LAST;
		lv_obj_set_grid_dsc_array(getCont(), m_columnDsc, m_rowDsc);
		lv_obj_set_grid_cell(m_vScale, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_hScale, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_chart, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_legend, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

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

		// Legend
		lv_obj_set_flex_flow(m_legend, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_style_pad_all(m_legend, 0, LV_PART_MAIN);
		lv_obj_set_style_pad_row(m_legend, 0, LV_PART_MAIN);
		// lv_obj_set_style

		showLegend(true);
	}

	void Graph::showLegend(const bool show)
	{
		m_columnDsc[2] = show ? s_legendSize : 0;
		if (show)
		{
			lv_obj_remove_flag(m_legend, LV_OBJ_FLAG_HIDDEN);
		}
		else
		{
			lv_obj_add_flag(m_legend, LV_OBJ_FLAG_HIDDEN);
		}
		lv_obj_set_grid_dsc_array(getCont(), m_columnDsc, m_rowDsc);
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

	void Graph::setXCount(int32_t count)
	{
		lv_chart_set_point_count(m_chart, count);
	}

	void Graph::setSeriesCount(size_t count)
	{
		if (count == m_series.size())
		{
			return;
		}
		if (count < m_series.size())
		{
			m_series.resize(count);
			return;
		}
		m_series.reserve(count);
	}

	const Graph::series_t* Graph::getSeries(const size_t index)
	{
		if (index >= m_series.size())
		{
			warn("Series index out of range");
			return nullptr;
		}
		return &m_series[index];
	}

	bool Graph::createSeries(lv_color_t color, const std::string& displayName)
	{

		lv_chart_series_t* series = lv_chart_add_series(m_chart, color, LV_CHART_AXIS_PRIMARY_Y);

		if (series == nullptr)
		{
			error("Failed to create series");
			return false;
		}

		size_t index = getSeriesCount();

		std::shared_ptr<legend_obj_t> legendObj =
			std::make_shared<legend_obj_t>(utils::format("%s_%u_legend_obj", getName(), index),
										   m_legend,
										   displayName.c_str(),
										   layout_t(0, 0, 100, 20));
		legendObj.get()->setBgColor(color, LV_STATE_CHECKED);
		legendObj.get()->setBgColor(s_hiddenColor, LV_STATE_DEFAULT);
		legendObj.get()->setCheckable(true);
		legendObj.get()->setChecked(true);
		legendObj.get()->setCallback(legendEvent, LV_EVENT_PRESSED, this);
		legendObj.get()->setUserData(new size_t(index));
		m_series.push_back(series_t(series, color, legendObj));
		return true;
	}

	bool Graph::updateSeriesColor(const size_t index, lv_color_t color)
	{
		series_t* series = (series_t*)getSeries(index);
		if (series == nullptr)
		{
			warn("Cannot update series, series not found");
			return false;
		}
		setSeriesColor(*series, color);
		return true;
	}

	bool Graph::updateSeriesName(const size_t index, const std::string& displayName)
	{
		series_t* series = (series_t*)getSeries(index);
		if (series == nullptr)
		{
			warn("Cannot update series, series not found");
			return false;
		}
		legend_obj_t* legendObj = series->legendObj.get();
		legendObj->setText(displayName.c_str());
		return true;
	}

	void Graph::showSeries(const size_t index, const bool show)
	{
		const series_t* series = getSeries(index);
		if (series == nullptr)
		{
			warn("Cannot show/hide series, series not found");
			return;
		}
		lv_chart_hide_series(m_chart, series->series, !show);
	}

	void Graph::clear()
	{
		for (auto& series : m_series)
		{
			lv_chart_remove_series(m_chart, series.series);
			delete (size_t*)series.legendObj.get()->getUserData();
		}
		m_series.clear();
	}

	void Graph::clear(const size_t index)
	{
		const series_t* series = getSeries(index);
		if (series == nullptr)
		{
			warn("Cannot clear series, series not found");
			return;
		}
		lv_chart_remove_series(m_chart, series->series);
		delete (std::string*)series->legendObj.get()->getUserData();
		m_series.erase(m_series.begin() + index);
	}

	void Graph::addData(const size_t index, int32_t value)
	{
		const series_t* series = getSeries(index);
		if (series == nullptr)
		{
			warn("Cannot add data to series, series not found");
			return;
		}
		lv_chart_set_next_value(m_chart, series->series, value);
	}

	void Graph::legendEvent(lv_event_t* e)
	{
		Graph* g = (Graph*)lv_event_get_user_data(e);
		lv_obj_t* btn = lv_event_get_target_obj(e);
		size_t* index = (size_t*)lv_obj_get_user_data(btn);

		// checked is inverted since this callback runs before the state is updated
		g->showSeries(*index, !lv_obj_has_state(btn, LV_STATE_CHECKED));
	}

	void Graph::setSeriesColor(series_t& series, lv_color_t color)
	{
		lv_chart_set_series_color(m_chart, series.series, color);
		series.color = color;

		lv_obj_t* legendObj = series.legendObj.get()->getCont();
		lv_obj_set_style_bg_color(legendObj, color, LV_STATE_CHECKED);
		lv_obj_set_style_bg_color(legendObj, s_hiddenColor, LV_STATE_DEFAULT);
	}
} // namespace UI
