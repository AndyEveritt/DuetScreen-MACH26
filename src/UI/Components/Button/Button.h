#pragma once

#include "UI/Core/View.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class Button : public BaseView
	{
	  public:
		Button(const std::string& name, lv_obj_t* parent, const std::string& text);
		Button(const std::string& name, lv_obj_t* parent, const std::string& text, layout_t layout);

		void setText(const std::string& text);
		void setCallback(lv_event_cb_t event_cb, lv_event_code_t filter, void* user_data);
		void setUserData(void* user_data);
		void* getUserData() const;
		void setIcon(lv_img_dsc_t* icon);
		void setCheckable(bool checkable);
		void setChecked(const bool checked);
		const bool getChecked() const;
		void setInvalid(bool invalid);
		void setBgColor(lv_color_t, lv_style_selector_t selector);
		void addBtnStyle(const lv_style_t* style, lv_style_selector_t selector);

		lv_obj_t* getLabel() const { return m_label; }
		lv_obj_t* getButton() const { return m_button; }
		lv_obj_t* getIcon() const { return m_icon; }

	  private:
		void init(const std::string& text);

		lv_obj_t* m_button;
		lv_obj_t* m_label;
		lv_obj_t* m_icon;
	};
} // namespace UI
