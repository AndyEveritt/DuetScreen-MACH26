/*
 * Heatmap.h
 *
 *  Created on: 2025-01-20
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Canvas/Canvas.h"
#include <map>
#include <memory>
#include <vector>

namespace UI
{
	enum class HeatmapRenderMode
	{
		Fixed = 0,
		Deviation,
	};

	class Heatmap : public BaseView
	{
	  public:
		using range_t = Canvas::range_t;

		struct DataPoint
		{
			float x;
			float y;
			float value;
		};

		Heatmap(const std::string& name, lv_obj_t* parent);
		Heatmap(const std::string& name, lv_obj_t* parent, layout_t layout);
		virtual ~Heatmap();

		range_t getXRange() const;
		range_t getYRange() const;
		void setXRange(range_t range);
		void setYRange(range_t range);

		void showScale(const bool show);

		void setRenderMode(HeatmapRenderMode mode);
		void setValueRange(float min, float max);

		void addDataPoint(float x, float y, float value);
		void addDataPoints(const std::vector<DataPoint>& points);

		void render();
		void renderColorBar();

		void clear();

	  private:
		void init();
		float normalizeValue(float value) const;

		int32_t m_columnDsc[4];
		int32_t m_rowDsc[3];

		Canvas m_canvas;
		Canvas m_colorBar;

		std::vector<DataPoint> m_dataPoints;
		HeatmapRenderMode m_renderMode = HeatmapRenderMode::Fixed;
		float m_minValue = 0.0f;
		float m_maxValue = 1.0f;
	};
} // namespace UI
