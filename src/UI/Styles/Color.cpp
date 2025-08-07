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
	Color::Color(float l_, float c_, float h_)
		: m_l(l_)
		, m_c(c_)
		, m_h(h_)
	{
		m_l = std::clamp(m_l, 0.0f, 1.0f);
		m_c = std::clamp(m_c, 0.0f, 0.2f);
		m_h = std::clamp(m_h, 0.0f, 360.0f);
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
} // namespace UI
