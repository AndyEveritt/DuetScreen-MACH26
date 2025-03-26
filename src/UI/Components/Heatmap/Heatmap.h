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

		Heatmap(const std::string& name, lv_obj_t* parent);
		Heatmap(const std::string& name, lv_obj_t* parent, layout_t layout);
		virtual ~Heatmap();

		range_t getXRange() const;
		range_t getYRange() const;
		void setXRange(range_t range);
		void setYRange(range_t range);

		void showScale(const bool show);

		void clear();

	  private:
		void init();

		int32_t m_columnDsc[4];
		int32_t m_rowDsc[3];

		Canvas m_canvas;
		Canvas m_colorBar;
	};
} // namespace UI
