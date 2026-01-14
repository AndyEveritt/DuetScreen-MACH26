#pragma once

#include "UI/Components/LVGL/generated/LvCanvas.gen.h"

namespace UI
{
	class LvCanvas : public LvCanvasGen
	{
	  public:
		using LvCanvasGen::LvCanvasGen;

		void setPxNoInvalidate(int32_t x, int32_t y, lv_color_t color, lv_opa_t opa)
		{
			ZoneScoped;
			lv_draw_buf_t* draw_buf = getDrawBuf();

			lv_color_format_t cf = static_cast<lv_color_format_t>(draw_buf->header.cf);
			uint8_t* data = static_cast<uint8_t*>(lv_draw_buf_goto_xy(draw_buf, x, y));

			if (LV_COLOR_FORMAT_IS_INDEXED(cf))
			{
				uint8_t shift;
				uint8_t c_int = color.blue;
				switch (cf)
				{
				case LV_COLOR_FORMAT_I1:
					shift = 7 - (x & 0x7);
					break;
				case LV_COLOR_FORMAT_I2:
					shift = 6 - 2 * (x & 0x3);
					break;
				case LV_COLOR_FORMAT_I4:
					shift = 4 - 4 * (x & 0x1);
					break;
				case LV_COLOR_FORMAT_I8:
					/*Indexed8 format is a easy case, process and return.*/
					shift = 0;
					*data = c_int;
				default:
					return;
				}

				uint8_t bpp = lv_color_format_get_bpp(cf);
				uint8_t mask = static_cast<uint8_t>((1 << bpp) - 1);
				c_int &= mask;
				*data = (*data & static_cast<uint8_t>(~(mask << shift))) | static_cast<uint8_t>(c_int << shift);
			}
			else if (cf == LV_COLOR_FORMAT_L8)
			{
				*data = lv_color_luminance(color);
			}
			else if (cf == LV_COLOR_FORMAT_A8)
			{
				*data = opa;
			}
			else if (cf == LV_COLOR_FORMAT_RGB565)
			{
				lv_color16_t* buf = (lv_color16_t*)data;
#pragma GCC diagnostic ignored "-Wconversion"
				buf->red = color.red >> 3;
				buf->green = color.green >> 2;
				buf->blue = color.blue >> 3;
#pragma GCC diagnostic pop
			}
			else if (cf == LV_COLOR_FORMAT_RGB888)
			{
				data[2] = color.red;
				data[1] = color.green;
				data[0] = color.blue;
			}
			else if (cf == LV_COLOR_FORMAT_XRGB8888)
			{
				data[2] = color.red;
				data[1] = color.green;
				data[0] = color.blue;
				data[3] = 0xFF;
			}
			else if (cf == LV_COLOR_FORMAT_ARGB8888)
			{
				lv_color32_t* buf = (lv_color32_t*)data;
				buf->red = color.red;
				buf->green = color.green;
				buf->blue = color.blue;
				buf->alpha = opa;
			}
			else if (cf == LV_COLOR_FORMAT_AL88)
			{
				lv_color16a_t* buf = (lv_color16a_t*)data;
				buf->lumi = lv_color_luminance(color);
				buf->alpha = 255;
			}
		}
	};
} // namespace UI
