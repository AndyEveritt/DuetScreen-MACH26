/*
 * Color.cpp
 *
 *  Created on: 2025-08-06
 *      Author: Andy Everitt
 */

#include "Color.h"
#include "Debug.h"
#include "colorm.h"

namespace UI
{
	Color::Color(float l, float c, float h)
	{
		setL(l);
		setC(c);
		setH(h);
	}

	Color::Color(lv_color_t rgb)
	{
		auto oklch = colorm::Oklch(colorm::Rgb(rgb.red, rgb.green, rgb.blue));
		m_l = oklch.lightness();
		m_c = oklch.chroma();
		m_h = oklch.hue();
	}

	Color& Color::operator=(lv_color_t rgb)
	{
		*this = Color(rgb);
		return *this;
	}

	Color::operator lv_color_t() const
	{
		auto rgb = colorm::Rgb(colorm::Oklch(m_l, m_c, m_h));
		auto rgb_fit = rgb.fit();
		lv_color_t lv_color;
		lv_color.red = static_cast<uint8_t>(rgb.red8());
		lv_color.green = static_cast<uint8_t>(rgb.green8());
		lv_color.blue = static_cast<uint8_t>(rgb.blue8());
		return lv_color;
	}

	void Color::setL(float l)
	{
		m_l = std::clamp(l, 0.0f, 1.0f);
	}

	void Color::setC(float c)
	{
		m_c = std::clamp(c, 0.0f, 0.4f);
	}

	void Color::setH(float h)
	{
		m_h = std::clamp(h, 0.0f, 360.0f);
	}
} // namespace UI
