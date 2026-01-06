/*
 * Graph.cpp
 *
 *  Created on: 2025-01-20
 *      Author: Andy Everitt
 */

#include "Graph.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	static constexpr lv_coord_t s_scaleSize = 30;
	static constexpr lv_coord_t s_legendSize = 100;
	static lv_color_t s_hiddenColor = lv_color_darken(lv_color_white(), 50);

	Graph::Graph(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		init();
	}

	void Graph::init()
	{
		UI_LOCK();

		// Layout
		m_columnDsc[0] = s_scaleSize;
		m_columnDsc[1] = LV_GRID_FR(1);
		m_columnDsc[2] = s_legendSize;
		m_columnDsc[3] = LV_GRID_TEMPLATE_LAST;

		m_rowDsc[0] = LV_GRID_FR(1);
		m_rowDsc[1] = s_scaleSize;
		m_rowDsc[2] = LV_GRID_TEMPLATE_LAST;
		setGridDsc(m_columnDsc, m_rowDsc);
		setGridCell(m_vScale, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_hScale, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		setGridCell(m_chart, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		setGridCell(m_legend, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

		lv_obj_set_style_pad_top(getRootPtr(), 20, LV_PART_MAIN);
		lv_obj_set_style_pad_right(getRootPtr(), 20, LV_PART_MAIN);
		lv_obj_set_style_pad_left(getRootPtr(), 10, LV_PART_MAIN);
		lv_obj_set_style_pad_bottom(getRootPtr(), 5, LV_PART_MAIN);

		// Chart
		// Horizontal scale
		m_hScale.setMode(LV_SCALE_MODE_HORIZONTAL_BOTTOM);
		m_hScale.setLabelShow(true);

		// Vertical scale
		m_vScale.setMode(LV_SCALE_MODE_VERTICAL_LEFT);
		m_vScale.setLabelShow(true);
		m_vScale.setTotalTickCount(21);
		m_vScale.setMajorTickEvery(4);

		// Chart
		m_chart.setType(LV_CHART_TYPE_LINE);
		m_chart.setUpdateMode(LV_CHART_UPDATE_MODE_SHIFT);

		// Legend
		m_legend.setFlexFlow(LV_FLEX_FLOW_COLUMN);

		showLegend(true);
	}

	void Graph::showLegend(const bool show)
	{
		UI_LOCK();
		m_columnDsc[2] = show ? s_legendSize : 0;
		m_legend.setVisible(show);
		setGridDsc(m_columnDsc, m_rowDsc);
	}

	Graph::range_t Graph::getXRange() const
	{
		UI_LOCK();
		range_t range;
		range.min = m_hScale.getRangeMinValue();
		range.max = m_hScale.getRangeMaxValue();
		return range;
	}

	Graph::range_t Graph::getYRange() const
	{
		UI_LOCK();
		range_t range;
		range.min = m_vScale.getRangeMinValue();
		range.max = m_vScale.getRangeMaxValue();
		return range;
	}

	void Graph::setXRange(Graph::range_t range)
	{
		UI_LOCK();
		m_hScale.setRange(range.min, range.max);
		m_chart.setAxisRange(LV_CHART_AXIS_PRIMARY_X, range.min, range.max);
	}

	void Graph::setYRange(Graph::range_t range)
	{
		UI_LOCK();
		m_vScale.setRange(range.min, range.max);
		m_chart.setAxisRange(LV_CHART_AXIS_PRIMARY_Y, range.min, range.max);
	}

	void Graph::setXCount(int32_t count)
	{
		UI_LOCK();
		m_chart.setPointCount(count);
	}

	void Graph::setSeriesCount(size_t count)
	{
		UI_LOCK();
		if (count == m_series.size())
		{
			return;
		}
		if (count < m_series.size())
		{
			for (size_t i = count; i < m_series.size(); ++i)
			{
				m_chart.removeSeries(m_series[i].series);
			}
			m_series.resize(count);
			return;
		}
		m_series.reserve(count);
	}

	const Graph::series_t* Graph::getSeries(const size_t index)
	{
		if (index >= m_series.size())
		{
			LOG_DBG("Series index out of range");
			return nullptr;
		}
		return &m_series[index];
	}

	bool Graph::createSeries(lv_color_t color, const std::string& displayName)
	{
		UI_LOCK();
		lv_chart_series_t* series = m_chart.addSeries(color, LV_CHART_AXIS_PRIMARY_Y);

		if (series == nullptr)
		{
			LOG_ERROR("Failed to create series");
			return false;
		}

		size_t index = getSeriesCount();

		auto legendObj = std::make_shared<legend_obj_t>(fmt::format("legend_obj_{}", index), m_legend);
		legendObj->setSize(LV_PCT(100), LV_SIZE_CONTENT);
		legendObj->setText(displayName);
		legendObj->setStyleBgColor(color, LV_STATE_CHECKED);
		legendObj->addStyle(Themes::getLvglStyles().border);
		legendObj->addStyle(Themes::getLvglStyles().bg_light, LV_STATE_DEFAULT);
		legendObj->setCheckable(true);
		legendObj->setChecked(true);
		legendObj->addClickedCallback(legendEvent, this);
		legendObj->setUserData((void*)(uintptr_t)index);
		m_series.push_back(series_t(series, color, legendObj));
		return true;
	}

	bool Graph::updateSeriesColor(const size_t index, lv_color_t color)
	{
		UI_LOCK();
		series_t* series = (series_t*)getSeries(index);
		if (series == nullptr)
		{
			LOG_WARN("Cannot update series, series not found");
			return false;
		}
		setSeriesColor(*series, color);
		return true;
	}

	bool Graph::updateSeriesName(const size_t index, const std::string& displayName)
	{
		UI_LOCK();
		series_t* series = (series_t*)getSeries(index);
		if (series == nullptr)
		{
			LOG_WARN("Cannot update series, series not found");
			return false;
		}
		auto legendObj = series->legendObj;
		legendObj->setText(displayName.c_str());
		return true;
	}

	void Graph::showSeries(const size_t index, const bool show)
	{
		UI_LOCK();
		const series_t* series = getSeries(index);
		if (series == nullptr)
		{
			LOG_WARN("Cannot show/hide series, series not found");
			return;
		}
		m_chart.hideSeries(series->series, !show);
		series->legendObj->setChecked(show);
	}

	void Graph::clear()
	{
		UI_LOCK();
		for (auto& series : m_series)
		{
			m_chart.removeSeries(series.series);
		}
		m_chart.refresh();
		m_series.clear();
	}

	void Graph::clear(const size_t index)
	{
		UI_LOCK();
		const series_t* series = getSeries(index);
		if (series == nullptr)
		{
			LOG_WARN("Cannot clear series, series not found");
			return;
		}
		m_chart.removeSeries(series->series);
		m_chart.refresh();
		m_series.erase(m_series.begin() + index);
	}

	void Graph::addData(const size_t index, int32_t value)
	{
		UI_LOCK();
		const series_t* series = getSeries(index);
		if (series == nullptr)
		{
			LOG_WARN("Cannot add data to series, series not found");
			return;
		}
		m_chart.setNextValue(series->series, value);
	}

	void Graph::legendEvent(lv_event_t* e)
	{
		UI_LOCK();
		Graph* g = (Graph*)lv_event_get_user_data(e);
		LvObj* btn = LvObj::fromPtr(lv_event_get_target_obj(e));
		size_t index = (uintptr_t)btn->getUserData();

		// checked is inverted since this callback runs before the state is updated
		g->showSeries(index, btn->hasState(LV_STATE_CHECKED));
	}

	void Graph::setSeriesColor(series_t& series, lv_color_t color)
	{
		UI_LOCK();
		m_chart.setSeriesColor(series.series, color);
		series.color = color;

		series.legendObj->setStyleBgColor(color, LV_STATE_CHECKED);
		series.legendObj->setStyleBgColor(s_hiddenColor, LV_STATE_DEFAULT);
	}
} // namespace UI
