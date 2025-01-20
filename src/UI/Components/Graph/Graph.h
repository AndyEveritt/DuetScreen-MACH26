/*
 * Graph.h
 *
 *  Created on: 2025-01-20
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/View.h"
#include <map>

namespace UI
{
	typedef std::map<std::string, lv_chart_series_t*> series_map_t;

	class Graph : public BaseView
	{
	  public:
		struct range_t
		{
			int32_t min;
			int32_t max;
		};

		Graph(const std::string& name, lv_obj_t* parent, layout_t layout);

		range_t getXRange() const;
		range_t getYRange() const;
		void setXRange(range_t range);
		void setYRange(range_t range);

		const series_map_t& getSeriesMap() const { return m_series; }
		size_t getSeriesCount() const { return m_series.size(); }
		lv_chart_series_t* getSeries(const std::string& name);
		bool createSeries(const std::string& name, lv_color_t color);

		void clear();
		void clear(const std::string& series);
		void addData(const std::string& series, int32_t value);

	  private:
		lv_obj_t* m_chart;
		lv_obj_t* m_vScale;
		lv_obj_t* m_hScale;

		int32_t m_columnDsc[3];
		int32_t m_rowDsc[3];

		series_map_t m_series;
	};
} // namespace UI
