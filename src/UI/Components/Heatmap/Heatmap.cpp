/*
 * Heatmap.cpp
 *
 *  Created on: 2025-03-26
 *      Author: Andy Everitt
 */

#include "Heatmap.h"
#include "Debug.h"
#include <cmath>

namespace UI
{
	static constexpr lv_coord_t s_scaleSize = 30;

	static uint32_t GetColorForPercent(double percent);

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

		m_canvas.setTitle("Heatmap");
		m_colorBar.setTitle("Scale:");

		// Color Bar
		m_colorBar.showXScale(false);

		// lv_obj_set_style
	}

	void Heatmap::showScale(const bool show)
	{
		lv_obj_set_flag(m_colorBar, LV_OBJ_FLAG_HIDDEN, !show);
		m_columnDsc[2] = show ? LV_GRID_CONTENT : 0;
	}

	Heatmap::range_t Heatmap::getXRange() const
	{
		range_t range;
		return range;
	}

	Heatmap::range_t Heatmap::getYRange() const
	{
		range_t range;
		return range;
	}

	void Heatmap::setXRange(Heatmap::range_t range)
	{
		m_canvas.setXRange(range);
	}

	void Heatmap::setYRange(Heatmap::range_t range)
	{
		m_colorBar.setYRange(range);
	}

	void Heatmap::clear()
	{
		lv_canvas_fill_bg(m_canvas, lv_color_hex(0xFFFFFF), LV_OPA_COVER);
	}

	static uint32_t GetColorForPercent(double percent)
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
		uint32_t color = (0xFF << 24) | (red << 16) | (green << 8) | blue;

		return color;
	}
} // namespace UI
