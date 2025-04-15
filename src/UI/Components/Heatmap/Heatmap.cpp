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
		UI_LOCK();

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(getCont(), LV_FLEX_FLOW_ROW);

		lv_obj_set_flex_grow(m_canvas, 1);
		lv_obj_set_height(m_canvas, LV_PCT(100));
		lv_obj_set_size(m_colorBar, LV_SIZE_CONTENT, LV_PCT(100));

		lv_obj_set_style_pad_all(getCont(), 5, LV_PART_MAIN);

		m_canvas.setTitle("");
		m_canvas.setResolution(100, 100);
		m_canvas.setXRange(range_t{0, 100});
		m_canvas.setYRange(range_t{0, 100});

		// Color Bar
		m_colorBar.setTitle("Scale:");
		m_colorBar.setResolution(1, 100);
		m_colorBar.showXScale(false);
		m_colorBar.setYRange(range_float_t{-1.0f, 1.0f});
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

	void Heatmap::setPx(size_t px, size_t py, float value)
	{
		m_minValue = std::min(m_minValue, value);
		m_maxValue = std::max(m_maxValue, value);

		lv_color_t color = GetColorForPercent(normalizeValue(value));
		m_canvas.drawPx(px, py, color, LV_OPA_COVER);
	}

	void Heatmap::setPos(float x, float y, float value)
	{
		size_t px, py;
		if (!m_canvas.posToPx(x, y, px, py))
		{
			warn("Invalid position (%f, %f) for heatmap", x, y);
			return;
		}
		setPx(px, py, value);
	}

	void Heatmap::render()
	{
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
