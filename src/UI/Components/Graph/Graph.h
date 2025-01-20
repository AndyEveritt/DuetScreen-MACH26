/*
 * Graph.h
 *
 *  Created on: 2025-01-20
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button.h"
#include "UI/Core/View.h"
#include <map>
#include <memory>

namespace UI
{

	class Graph : public BaseView
	{
	  public:
		typedef Button legend_obj_t;
		struct series_t
		{
			lv_chart_series_t* series;
			lv_color_t color;
			std::shared_ptr<legend_obj_t> legendObj;
		};
		typedef std::map<std::string, series_t> series_map_t;
		struct range_t
		{
			int32_t min;
			int32_t max;
		};

		Graph(const std::string& name, lv_obj_t* parent, layout_t layout);

		void showLegend(const bool show);

		range_t getXRange() const;
		range_t getYRange() const;
		void setXRange(range_t range);
		void setYRange(range_t range);
		void setXCount(int32_t count);

		size_t getSeriesCount() const { return m_series.size(); }
		const series_t* getSeries(const std::string& id);
		bool createSeries(const std::string& id, lv_color_t color, const std::string& displayName);
		void showSeries(const std::string& id, const bool show);

		void clear();
		void clear(const std::string& id);
		void addData(const std::string& id, int32_t value);

	  private:
		static void legendEvent(lv_event_t* e);

		lv_obj_t* m_chart;
		lv_obj_t* m_vScale;
		lv_obj_t* m_hScale;
		lv_obj_t* m_legend;

		int32_t m_columnDsc[4];
		int32_t m_rowDsc[3];

		series_map_t m_series;
	};
} // namespace UI
