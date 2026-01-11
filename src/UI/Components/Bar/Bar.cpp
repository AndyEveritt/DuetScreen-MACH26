/*
 * Bar.cpp
 *
 *  Created on: 2026-01-09
 *      Author: Andy Everitt
 */

#include "Bar.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	Bar::Bar(const std::string& name, LvObj& parent)
		: LvBar(name, parent)
	{
		UI_LOCK();

		// Update label text and position
		addEventCallback(drawLabel, LV_EVENT_DRAW_MAIN_END, this);
	}

	void Bar::drawLabel(lv_event_t* e)
	{
		Bar& self = *static_cast<Bar*>(lv_event_get_user_data(e));

		const int32_t pad = 10;

		lv_draw_label_dsc_t label_dsc;
		lv_draw_label_dsc_init(&label_dsc);
		label_dsc.font = &Themes::getFonts().normal;

		char buf[32];
		auto result = fmt::format_to_n(buf, sizeof(buf) - 1, fmt::runtime(self.m_labelFmt), self.getValue());
		buf[result.size] = '\0';

		lv_point_t txt_size;
		lv_text_get_size(&txt_size,
						 buf,
						 label_dsc.font,
						 label_dsc.letter_space,
						 label_dsc.line_space,
						 self.getWidth() - (2 * pad),
						 label_dsc.flag);

		lv_area_t txt_area{.x1 = 0, .y1 = 0, .x2 = txt_size.x - 1, .y2 = txt_size.y - 1};

		lv_area_t indic_area = self.getCoords();
		const int32_t filled_width =
			static_cast<int32_t>((lv_area_get_width(&indic_area) * (self.getValue() - self.getMinValue())) /
								 (self.getMaxValue() - self.getMinValue()));
		lv_area_set_width(&indic_area, filled_width);

		/*If the indicator is long enough put the text inside on the right*/
		if (lv_area_get_width(&indic_area) > txt_size.x + 2 * pad)
		{
			lv_area_align(&indic_area, &txt_area, LV_ALIGN_RIGHT_MID, -pad, 0);
			label_dsc.color = lv_color_white();
		}
		/*If the indicator is still short put the text out of it on the right*/
		else
		{
			lv_area_align(&indic_area, &txt_area, LV_ALIGN_OUT_RIGHT_MID, pad, 0);
			label_dsc.color = lv_color_white();
		}
		label_dsc.text = buf;
		label_dsc.text_local = true;
		lv_layer_t* layer = lv_event_get_layer(e);
		lv_draw_label(layer, &label_dsc, &txt_area);
	}
} // namespace UI
