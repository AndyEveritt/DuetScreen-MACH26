/*
 * Canvas.h
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

	class Canvas : public BaseView
	{
	  public:
		struct range_t
		{
			int32_t min;
			int32_t max;
		};
		struct range_float_t
		{
			float min;
			float max;
		};

		Canvas(const std::string& name, lv_obj_t* parent);
		Canvas(const std::string& name, lv_obj_t* parent, layout_t layout);
		virtual ~Canvas();

		range_t getXRange() const;
		range_t getYRange() const;
		void setXRange(range_t range);
		void setYRange(range_t range);
		void setXRange(range_float_t range);
		void setYRange(range_float_t range);

		void setTitle(const char* title);

		void showTitle(const bool show);
		void showXScale(const bool show);
		void showYScale(const bool show);

		bool getResolution(uint32_t& width, uint32_t& height) const;
		void setResolution(uint32_t width, uint32_t height);

		void drawRect(lv_area_t area, lv_color_t color, lv_opa_t opa);
		void drawRectPx(lv_area_t area, lv_color_t color, lv_opa_t opa);

		lv_obj_t* getCanvas() const { return m_canvas; }

		void clear();

	  private:
		void init();

		void createLabels(range_float_t range, uint32_t ticks, std::vector<std::string>& vec, const char**& labels);

		int32_t m_columnDsc[3];
		int32_t m_rowDsc[4];

		lv_draw_buf_t* m_buf = nullptr;

		lv_obj_t* m_title;
		lv_obj_t* m_canvas;
		lv_obj_t* m_vScale;
		lv_obj_t* m_hScale;

		std::vector<std::string> m_xLabels;
		std::vector<std::string> m_yLabels;
		const char** m_xLabelPtr = nullptr;
		const char** m_yLabelPtr = nullptr;
	};
} // namespace UI
