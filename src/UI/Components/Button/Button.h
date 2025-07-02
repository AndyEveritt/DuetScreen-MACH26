#pragma once

#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Core/View.h"
#include "lvgl/lvgl.h"

namespace UI
{
	class Button : public LvObj
	{
	  public:
		Button(const std::string& name, lv_obj_t* parent);
		Button(const std::string& name, lv_obj_t* parent, const std::string& text);
		Button(const std::string& name, lv_obj_t* parent, const std::string& text, layout_t layout);

		void setText(const std::string& text);
		void addClickedCallback(lv_event_cb_t event_cb, void* user_data);
		void setIcon(lv_img_dsc_t* icon);
		void setCheckable(bool checkable);
		void setChecked(const bool checked);
		const bool getChecked() const;
		void setDisabled(bool disabled);

		LvLabel& getLabel() { return m_label; }
		lv_obj_t* getButton() const { return getCont(); }
		lv_obj_t* getIcon() const { return m_icon; }

	  private:
		void init(const std::string& text);

		LvLabel m_label;
		lv_obj_t* m_icon;
	};
} // namespace UI
