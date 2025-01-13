#pragma once

#include "component.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class Button : public Component
	{
	  public:
		Button(const char* name, lv_obj_t* parent, const char* text);

		void setText(const char* text);
		void setCallback(lv_event_cb_t event_cb, lv_event_code_t filter, void* user_data);
		virtual void setStyle(lv_style_t* style, lv_style_selector_t selector) override;
		void setIcon(lv_img_dsc_t* icon);

		/* Position control */

		void setWidth(int widthPct);
		void setHeight(int heightPct);
		void setX(int xPct);
		void setY(int yPct);

	  private:
		lv_obj_t* m_button;
		lv_obj_t* m_label;
		lv_obj_t* m_icon;
	};
} // namespace UI
