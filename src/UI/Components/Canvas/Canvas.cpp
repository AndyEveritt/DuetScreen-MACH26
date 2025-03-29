/*
 * Canvas.cpp
 *
 *  Created on: 2025-03-24
 *      Author: Andy Everitt
 */

#include "Canvas.h"
#include "Debug.h"

namespace UI
{
#define DEFAULT_CANVAS_WIDTH 100
#define DEFAULT_CANVAS_HEIGHT 100

	static constexpr lv_coord_t s_scaleSize = 30;

	Canvas::Canvas(const std::string& name, lv_obj_t* parent)
		: BaseView(name, parent)
		, m_columnDsc{s_scaleSize, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_rowDsc{LV_GRID_CONTENT, LV_GRID_FR(1), s_scaleSize, LV_GRID_TEMPLATE_LAST}
		, m_title(lv_label_create(getCont()))
		, m_canvas(lv_canvas_create(getCont()))
		, m_vScale(lv_scale_create(getCont()))
		, m_hScale(lv_scale_create(getCont()))
	{
		init();
	}

	Canvas::Canvas(const std::string& name, lv_obj_t* parent, layout_t layout)
		: BaseView(name, parent, layout)
		, m_columnDsc{s_scaleSize, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}
		, m_rowDsc{LV_GRID_CONTENT, LV_GRID_FR(1), s_scaleSize, LV_GRID_TEMPLATE_LAST}
		, m_title(lv_label_create(getCont()))
		, m_canvas(lv_canvas_create(getCont()))
		, m_vScale(lv_scale_create(getCont()))
		, m_hScale(lv_scale_create(getCont()))
	{
		init();
	}

	Canvas::~Canvas()
	{
		lv_draw_buf_destroy(m_buf);
	}

	void Canvas::init()
	{
		Lock lock;

		// Layout
		lv_obj_set_layout(getCont(), LV_LAYOUT_GRID);
		lv_obj_set_grid_dsc_array(getCont(), m_columnDsc, m_rowDsc);
		lv_obj_set_grid_cell(m_title, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_set_grid_cell(m_vScale, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_set_grid_cell(m_hScale, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
		lv_obj_set_grid_cell(m_canvas, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

		lv_obj_set_style_pad_top(getCont(), 10, LV_PART_MAIN);
		lv_obj_set_style_pad_right(getCont(), 20, LV_PART_MAIN);
		lv_obj_set_style_pad_left(getCont(), 10, LV_PART_MAIN);
		lv_obj_set_style_pad_bottom(getCont(), 5, LV_PART_MAIN);

		// Title
		lv_obj_set_size(m_title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		// Horizontal scale
		lv_obj_set_height(m_hScale, LV_SIZE_CONTENT);
		lv_scale_set_mode(m_hScale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
		lv_scale_set_label_show(m_hScale, true);

		// Vertical scale
		lv_obj_set_width(m_vScale, LV_SIZE_CONTENT);
		lv_scale_set_mode(m_vScale, LV_SCALE_MODE_VERTICAL_LEFT);
		lv_scale_set_label_show(m_vScale, true);
		lv_scale_set_total_tick_count(m_vScale, 21);
		lv_scale_set_major_tick_every(m_vScale, 4);

		// Canvas
		lv_image_set_inner_align(m_canvas, LV_IMAGE_ALIGN_STRETCH);
		lv_obj_set_style_border_width(m_canvas, 2, LV_PART_MAIN);
		lv_obj_set_style_border_color(m_canvas, lv_color_hex(0x000000), LV_PART_MAIN);
		lv_obj_set_style_border_opa(m_canvas, LV_OPA_100, LV_PART_MAIN);

		// lv_obj_set_style
	}

	void Canvas::setTitle(const char* title)
	{
		Lock lock;
		lv_label_set_text(m_title, title);
	}

	void Canvas::showTitle(const bool show)
	{
		Lock lock;
		lv_obj_set_flag(m_title, LV_OBJ_FLAG_HIDDEN, !show);
		m_rowDsc[0] = show ? LV_GRID_CONTENT : 0;
	}

	void Canvas::showXScale(const bool show)
	{
		Lock lock;
		lv_obj_set_flag(m_hScale, LV_OBJ_FLAG_HIDDEN, !show);
		m_rowDsc[2] = show ? s_scaleSize : 0;
	}

	void Canvas::showYScale(const bool show)
	{
		Lock lock;
		lv_obj_set_flag(m_vScale, LV_OBJ_FLAG_HIDDEN, !show);
		m_columnDsc[0] = show ? s_scaleSize : 0;
	}

	Canvas::range_t Canvas::getXRange() const
	{
		Lock lock;
		range_t range;
		range.min = lv_scale_get_range_min_value(m_hScale);
		range.max = lv_scale_get_range_max_value(m_hScale);
		return range;
	}

	Canvas::range_t Canvas::getYRange() const
	{
		Lock lock;
		range_t range;
		range.min = lv_scale_get_range_min_value(m_vScale);
		range.max = lv_scale_get_range_max_value(m_vScale);
		return range;
	}

	void Canvas::setXRange(Canvas::range_t range)
	{
		Lock lock;
		lv_scale_set_range(m_hScale, range.min, range.max);
	}

	void Canvas::setYRange(Canvas::range_t range)
	{
		Lock lock;
		lv_scale_set_range(m_vScale, range.min, range.max);
	}

	bool Canvas::getResolution(uint32_t& width, uint32_t& height) const
	{
		Lock lock;
		if (m_buf == nullptr)
		{
			width = 0;
			height = 0;
			return false;
		}

		width = m_buf->header.w;
		height = m_buf->header.h;
		return true;
	}

	void Canvas::setResolution(uint32_t width, uint32_t height)
	{
		Lock lock;
		if (m_buf != nullptr)
		{
			lv_draw_buf_destroy(m_buf);
		}
		m_buf = lv_draw_buf_create(width, height, LV_COLOR_FORMAT_RGB565, 0);
		lv_canvas_set_draw_buf(m_canvas, m_buf);
	}

	void Canvas::drawRect(const lv_area_t& area, lv_color_t color, lv_opa_t opa)
	{
		Lock lock;
		lv_draw_rect_dsc_t rect_dsc;
		lv_draw_rect_dsc_init(&rect_dsc);
		rect_dsc.bg_opa = opa;
		rect_dsc.bg_color = color;

		lv_layer_t layer;
		lv_canvas_init_layer(m_canvas, &layer);
		lv_draw_rect(&layer, &rect_dsc, &area);

		lv_draw_rect(&layer, &rect_dsc, &area);

		lv_canvas_finish_layer(m_canvas, &layer);
	}

	void Canvas::clear()
	{
		Lock lock;
		lv_canvas_fill_bg(m_canvas, lv_color_hex(0xFFFFFF), LV_OPA_COVER);
	}
} // namespace UI
