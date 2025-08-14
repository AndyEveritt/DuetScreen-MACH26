/*
 * Color.h
 *
 *  Created on: 2025-08-06
 *      Author: Andy Everitt
 */

#pragma once

#include "lvgl/lvgl.h"

namespace UI
{
	class Color
	{
	  public:
		Color() = default;
		Color(float l, float c, float h);
		Color(lv_color_t rgb); // from RGB
		Color& operator=(lv_color_t rgb);

		operator lv_color_t() const; // implicit cast to RGB

		void setL(float);
		void setC(float);
		void setH(float);

		float getL() const { return m_l; }
		float getC() const { return m_c; }
		float getH() const { return m_h; }

	  private:
		float m_l; // [0.0, 1.0]
		float m_c; // chroma
		float m_h; // hue in radians [0.0, 2π)
	};
} // namespace UI
