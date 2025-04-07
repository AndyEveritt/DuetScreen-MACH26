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

		// Grid configuration methods - setting one parameter infers the other
		void setGridSize(int rows, int cols);
		void setGridSpacing(float xSpacing, float ySpacing);

		// Get a grid value at specific grid coordinates
		float getGridValue(int row, int col) const;

		// Add a data point to the grid at specific grid coordinates
		void setGridValue(int row, int col, float value);

		void render();
		void renderColorBar();

		void clear();

	  private:
		void init();
		float normalizeValue(float value) const;

		// Bilinear interpolation for grid values
		float interpolateValue(float x, float y) const;

		// Map real coordinates to grid indices
		bool mapToGrid(float x, float y, int& row, int& col) const;

		// Map grid indices to real coordinates
		void mapFromGrid(int row, int col, float& x, float& y) const;

		// Convert 2D indices to 1D index
		int gridIndex(int row, int col) const;

		// Initialize or resize the grid
		void setupGrid();

		// Ensure grid is initialized
		void ensureGridInitialized();

		int32_t m_columnDsc[4];
		int32_t m_rowDsc[3];

		Canvas m_canvas;
		Canvas m_colorBar;

		// Grid data storage (single 1D vector)
		std::vector<float> m_gridData;
		int m_gridRows = 0;
		int m_gridCols = 0;
		float m_gridXSpacing = 1.0f;
		float m_gridYSpacing = 1.0f;

		HeatmapRenderMode m_renderMode = HeatmapRenderMode::Fixed;
		float m_minValue = 0.0f;
		float m_maxValue = 1.0f;
	};
} // namespace UI
