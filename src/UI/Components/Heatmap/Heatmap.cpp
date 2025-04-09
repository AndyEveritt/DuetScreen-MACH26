/*
 * Heatmap.cpp
 *
 *  Created on: 2025-03-26
 *      Author: Andy Everitt
 */

#include "Heatmap.h"
#include "Debug.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace UI
{
	static constexpr lv_coord_t s_scaleSize = 30;

	static lv_color_t GetColorForPercent(double percent);

	Heatmap::Heatmap(const std::string& name, lv_obj_t* parent)
		: BaseView(name, parent)
		, m_columnDsc{s_scaleSize, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_rowDsc{LV_GRID_FR(1), s_scaleSize, LV_GRID_TEMPLATE_LAST}
		, m_canvas("heightmap_canvas", getCont())
		, m_colorBar("heightmap_color_bar", getCont())
	{
		init();
	}

	Heatmap::Heatmap(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_columnDsc{s_scaleSize, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST}
		, m_rowDsc{LV_GRID_FR(1), s_scaleSize, LV_GRID_TEMPLATE_LAST}
		, m_canvas("heightmap_canvas", getCont())
		, m_colorBar("heightmap_color_bar", getCont())
	{
		init();
	}

	Heatmap::~Heatmap() {}

	void Heatmap::init()
	{
		Lock lock;

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);

		lv_obj_set_flex_grow(m_canvas, 1);
		lv_obj_set_height(m_canvas, LV_PCT(100));
		lv_obj_set_size(m_colorBar, LV_SIZE_CONTENT, LV_PCT(100));

		lv_obj_set_style_pad_all(getCont(), 5, LV_PART_MAIN);

		m_canvas.setTitle("");
		m_canvas.setResolution(100, 100);
		m_canvas.setXRange({0, 100});
		m_canvas.setYRange({0, 100});

		// Color Bar
		m_colorBar.setTitle("Scale:");
		m_colorBar.setResolution(1, 100);
		m_colorBar.showXScale(false);
		m_colorBar.setYRange({-10, 10});

		// lv_obj_set_style
	}

	void Heatmap::showScale(const bool show)
	{
		lv_obj_set_flag(m_colorBar, LV_OBJ_FLAG_HIDDEN, !show);
		m_columnDsc[2] = show ? LV_GRID_CONTENT : 0;
	}

	Heatmap::range_t Heatmap::getXRange() const
	{
		return m_canvas.getXRange();
	}

	Heatmap::range_t Heatmap::getYRange() const
	{
		return m_canvas.getYRange();
	}

	void Heatmap::setXRange(Heatmap::range_t range)
	{
		m_canvas.setXRange(range);
	}

	void Heatmap::setYRange(Heatmap::range_t range)
	{
		m_canvas.setYRange(range);
	}

	void Heatmap::setTitle(const char* title)
	{
		m_canvas.setTitle(title);
	}

	void Heatmap::setRenderMode(HeatmapRenderMode mode)
	{
		m_renderMode = mode;
	}

	void Heatmap::setValueRange(float min, float max)
	{
		m_minValue = min;
		m_maxValue = max;

		// Update the color bar scale
		range_t colorRange = {static_cast<int32_t>(min), static_cast<int32_t>(max)};
		m_colorBar.setYRange(colorRange);
	}

	float Heatmap::normalizeValue(float value) const
	{
		if (m_renderMode == HeatmapRenderMode::Deviation)
		{
			// In deviation mode, center is 0, normalize around that
			float absMax = std::max(std::abs(m_minValue), std::abs(m_maxValue));
			return (value + absMax) / (2 * absMax);
		}
		else
		{
			// In fixed mode, simple min-max normalization
			return (value - m_minValue) / (m_maxValue - m_minValue);
		}
	}

	void Heatmap::ensureGridInitialized()
	{
		// If grid is not initialized yet, create a default grid
		if (m_gridRows == 0 || m_gridCols == 0)
		{
			// Default to 50x50 grid resolution
			setGridSize(50, 50);
		}
	}

	void Heatmap::addDataPoint(float x, float y, float value)
	{
		ensureGridInitialized();

		// Map to grid coordinates and store the value
		int row, col;
		if (mapToGrid(x, y, row, col))
		{
			setGridValue(row, col, value);
		}
	}

	void Heatmap::addDataPoints(const std::vector<DataPoint>& points)
	{
		for (const auto& point : points)
		{
			addDataPoint(point.x, point.y, point.value);
		}
	}

	void Heatmap::setGridSize(int rows, int cols)
	{
		if (rows <= 0 || cols <= 0)
		{
			error("Invalid grid size: (%d, %d)", rows, cols);
			return;
		}

		m_gridRows = rows;
		m_gridCols = cols;

		// Calculate spacing based on the current x/y ranges
		range_t xRange = getXRange();
		range_t yRange = getYRange();
		m_gridXSpacing = static_cast<float>(xRange.max - xRange.min) / (cols - 1);
		m_gridYSpacing = static_cast<float>(yRange.max - yRange.min) / (rows - 1);

		setupGrid();
	}

	void Heatmap::setGridSpacing(float xSpacing, float ySpacing)
	{
		if (xSpacing <= 0.0f || ySpacing <= 0.0f)
		{
			error("Invalid grid spacing: (%f, %f)", xSpacing, ySpacing);
			return;
		}

		m_gridXSpacing = xSpacing;
		m_gridYSpacing = ySpacing;

		// Calculate grid size based on the current x/y ranges
		range_t xRange = getXRange();
		range_t yRange = getYRange();
		m_gridCols = static_cast<int>((xRange.max - xRange.min) / xSpacing) + 1;
		m_gridRows = static_cast<int>((yRange.max - yRange.min) / ySpacing) + 1;

		setupGrid();
	}

	void Heatmap::setupGrid()
	{
		// Resize the grid and initialize with NaN to indicate empty cells
		m_gridData.resize(m_gridRows * m_gridCols, std::numeric_limits<float>::quiet_NaN());
	}

	int Heatmap::gridIndex(int row, int col) const
	{
		return row * m_gridCols + col;
	}

	float Heatmap::getGridValue(int row, int col) const
	{
		if (row < 0 || row >= m_gridRows || col < 0 || col >= m_gridCols)
		{
			warn("Grid coordinates out of bounds: (%d, %d)", row, col);
			return std::numeric_limits<float>::quiet_NaN();
		}

		int index = gridIndex(row, col);
		if (index >= m_gridData.size())
		{
			warn("Grid index out of bounds: %d", index);
			return std::numeric_limits<float>::quiet_NaN();
		}

		return m_gridData[index];
	}

	void Heatmap::setGridValue(int row, int col, float value)
	{
		if (row < 0 || row >= m_gridRows || col < 0 || col >= m_gridCols)
		{
			error("Grid coordinates out of bounds: (%d, %d)", row, col);
			return;
		}

		int index = gridIndex(row, col);
		if (index >= m_gridData.size())
		{
			error("Grid index out of bounds: %d", index);
			return;
		}

		m_gridData[index] = value;
	}

	bool Heatmap::mapToGrid(float x, float y, int& row, int& col) const
	{
		if (m_gridRows == 0 || m_gridCols == 0)
		{
			error("Grid not initialized");
			return false;
		}

		// Convert real-world coordinates to grid indices
		range_t xRange = getXRange();
		range_t yRange = getYRange();

		const float fEPSILON = 0.0f;
		col = static_cast<int>((x - xRange.min - fEPSILON) / m_gridXSpacing);
		row = static_cast<int>((y - yRange.min - fEPSILON) / m_gridYSpacing);

		return (row >= 0 && row < m_gridRows && col >= 0 && col < m_gridCols);
	}

	void Heatmap::mapFromGrid(int row, int col, float& x, float& y) const
	{
		// Convert grid indices to real-world coordinates
		range_t xRange = getXRange();
		range_t yRange = getYRange();

		x = xRange.min + col * m_gridXSpacing;
		y = yRange.min + row * m_gridYSpacing;
	}

	float Heatmap::interpolateValue(float x, float y) const
	{
		// Find the grid cell containing the point
		int row, col;
		bool inGrid = mapToGrid(x, y, row, col);

		if (!inGrid)
			return std::numeric_limits<float>::quiet_NaN();

		// Get the four corner values
		float q11 = getGridValue(row, col);
		float q12 = getGridValue(row, col + 1);
		float q21 = getGridValue(row + 1, col);
		float q22 = getGridValue(row + 1, col + 1);

		q12 = std::isnan(q12) ? q11 : q12;
		q21 = std::isnan(q21) ? q11 : q21;
		q22 = std::isnan(q22) ? q11 : q22;

		// If any corner is NaN, return NaN
		if (std::isnan(q11) || std::isnan(q12) || std::isnan(q21) || std::isnan(q22))
			return std::numeric_limits<float>::quiet_NaN();

		// Calculate the fractional position within the cell
		float x1, y1, x2, y2;
		mapFromGrid(row, col, x1, y1);
		mapFromGrid(row + 1, col + 1, x2, y2);

		float xd = (x - x1) / (x2 - x1);
		float yd = (y - y1) / (y2 - y1);

		// Bilinear interpolation formula
		float result =
			q11 * (1.0f - xd) * (1.0f - yd) + q21 * (1.0f - xd) * yd + q12 * xd * (1.0f - yd) + q22 * xd * yd;

		return result;
	}

	void Heatmap::render()
	{
		// Skip if grid is empty or not initialized
		if (m_gridRows == 0 || m_gridCols == 0 || m_gridData.empty())
			return;

		// Get the canvas dimensions
		lv_obj_update_layout(m_canvas);
		uint32_t width;
		uint32_t height;
		m_canvas.getResolution(width, height);

		// Get the data ranges
		range_t xRange = getXRange();
		range_t yRange = getYRange();

		float xScale = static_cast<float>(width) / (xRange.max - xRange.min);
		float yScale = static_cast<float>(height) / (yRange.max - yRange.min);

		// Clear the canvas before rendering
		m_canvas.clear();

		// Render the heatmap pixel by pixel
		for (uint32_t pixelY = 0; pixelY < height; pixelY++)
		{
			for (uint32_t pixelX = 0; pixelX < width; pixelX++)
			{
				// Convert pixel coordinates to real-world coordinates
				float x = xRange.min + pixelX / xScale;
				float y = yRange.min + pixelY / yScale;

				// Get interpolated value at this position
				float value = interpolateValue(x, y);

				// Skip if no valid data for this position
				if (std::isnan(value))
					continue;

				// Normalize value and get corresponding color
				float normalizedValue = normalizeValue(value);
				lv_color_t color = GetColorForPercent(normalizedValue);

				// Draw the pixel
				lv_canvas_set_px(m_canvas.getCanvas(), pixelX, height - pixelY - 1, color, LV_OPA_COVER);
			}
		}

		// Now render the color bar
		renderColorBar();
	}

	void Heatmap::renderColorBar()
	{
		// Also render a color scale on the color bar
		lv_obj_update_layout(m_colorBar);
		uint32_t barWidth;
		uint32_t barHeight;
		m_colorBar.getResolution(barWidth, barHeight);
		for (int y = 0; y < barHeight; y++)
		{
			float percent = 1.0f - (float)y / barHeight;
			lv_color_t color = GetColorForPercent(percent);
			lv_canvas_set_px(m_colorBar.getCanvas(), 0, y, color, LV_OPA_COVER);
		}
	}

	void Heatmap::clear()
	{
		m_canvas.clear();
		m_colorBar.clear();

		// Clear grid data
		std::fill(m_gridData.begin(), m_gridData.end(), std::numeric_limits<float>::quiet_NaN());
	}

	static lv_color_t GetColorForPercent(double percent)
	{
		// Convert the height to a color on a HSV colorbar from blue to red
		double hue = (1.0 - percent) * 240.0; // Map the percent to the hue range (blue to red)
		double saturation = 1.0;			  // Set the saturation to maximum
		double value = 1.0;					  // Set the value to maximum

		// Convert HSV to RGB
		double c = value * saturation;
		double x = c * (1.0 - std::abs(std::fmod(hue / 60.0, 2.0) - 1.0));
		double m = value - c;

		double r, g, b;
		if (hue >= 0 && hue < 60)
		{
			r = c;
			g = x;
			b = 0;
		}
		else if (hue >= 60 && hue < 120)
		{
			r = x;
			g = c;
			b = 0;
		}
		else if (hue >= 120 && hue < 180)
		{
			r = 0;
			g = c;
			b = x;
		}
		else if (hue >= 180 && hue < 240)
		{
			r = 0;
			g = x;
			b = c;
		}
		else if (hue >= 240 && hue < 300)
		{
			r = x;
			g = 0;
			b = c;
		}
		else
		{
			r = c;
			g = 0;
			b = x;
		}

		// Scale the RGB values to the range 0-255
		uint8_t red = static_cast<uint8_t>((r + m) * 255);
		uint8_t green = static_cast<uint8_t>((g + m) * 255);
		uint8_t blue = static_cast<uint8_t>((b + m) * 255);

		// Combine the RGB values into a single color
		lv_color_t color = lv_color_make(red, green, blue);

		return color;
	}
} // namespace UI
