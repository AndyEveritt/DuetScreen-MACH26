#include "Button.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	Button::Button(const std::string& name, lv_obj_t* parent, const std::string& text)
		: LvObj(lv_obj_create, name, parent)
		, m_button(lv_button_create(getCont()))
		, m_label(lv_label_create(m_button))
		, m_icon(nullptr)
	{
		init(text);
	}

	Button::Button(const std::string& name, lv_obj_t* parent, const std::string& text, layout_t layout)
		: LvObj(lv_obj_create, name, parent, layout)
		, m_button(lv_button_create(getCont()))
		, m_label(lv_label_create(m_button))
		, m_icon(nullptr)
	{
		init(text);
	}

	void Button::init(const std::string& text)
	{
		UI_LOCK();
		lv_obj_set_user_data(m_button, this);
		lv_obj_set_user_data(m_label, this);

		lv_obj_set_style_bg_opa(getCont(), LV_OPA_TRANSP, 0);
		lv_obj_set_style_min_height(getCont(), 30, 0);
		lv_obj_set_style_min_width(getCont(), 50, 0);

		// Initialise the button obj
		lv_obj_set_pos(m_button, 0, 0);
		lv_obj_set_size(m_button, LV_PCT(100), LV_PCT(100));

		addStyle(Themes::getLvglStyles().no_border);

		// Initialise the label obj
		lv_label_set_text(m_label, text.c_str());
		lv_obj_set_pos(m_label, 0, 0);
		lv_obj_set_size(m_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_align(m_label, LV_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_label, LV_TEXT_ALIGN_CENTER, 0);
		lv_obj_update_layout(getCont());
	}

	void Button::setText(const std::string& text)
	{
		UI_LOCK();
		lv_label_set_text(m_label, text.c_str());
		lv_obj_set_size(m_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(m_label);
		if (m_icon != nullptr)
		{
			lv_obj_set_y(m_icon, LV_PCT(-20));
			lv_obj_set_y(m_label, LV_PCT(30));
		}
	}

	void Button::setCallback(lv_event_cb_t event_cb, lv_event_code_t filter, void* user_data)
	{
		UI_LOCK();
		lv_obj_add_event_cb(m_button, event_cb, filter, user_data);
	}

	void Button::setUserData(void* user_data)
	{
		UI_LOCK();
		lv_obj_set_user_data(m_button, user_data);
	}

	void* Button::getUserData() const
	{
		UI_LOCK();
		return lv_obj_get_user_data(m_button);
	}

	void Button::setIcon(lv_img_dsc_t* icon)
	{
		UI_LOCK();
		// If the icon is null, remove the icon and center the label
		if (icon == nullptr)
		{
			lv_obj_delete(m_icon);
			m_icon = nullptr;
			lv_obj_center(m_label);
			return;
		}

		// If the icon is not null, create the icon and set the icon
		if (m_icon == nullptr)
		{
			m_icon = lv_image_create(m_button);
			lv_obj_set_user_data(m_icon, this);
		}

		lv_image_set_src(m_icon, icon);
		lv_obj_center(m_icon);
		lv_obj_set_y(m_icon, lv_pct(-20));
		lv_obj_set_y(m_label, lv_pct(30));
	}

	void Button::setCheckable(bool checkable)
	{
		UI_LOCK();
		lv_obj_set_flag(m_button, LV_OBJ_FLAG_CHECKABLE, checkable);
	}

	void Button::setChecked(const bool checked)
	{
		UI_LOCK();
		lv_obj_set_state(m_button, LV_STATE_CHECKED, checked);
	}

	const bool Button::getChecked() const
	{
		UI_LOCK();
		if (lv_obj_has_flag(m_button, LV_OBJ_FLAG_CHECKABLE))
		{
			return lv_obj_has_state(m_button, LV_STATE_CHECKED);
		}
		return false;
	}

	void Button::setInvalid(bool invalid)
	{
		UI_LOCK();
		lv_obj_set_state(m_button, LV_STATE_DISABLED, invalid);
	}

	void Button::setBgColor(lv_color_t color, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_set_style_bg_color(m_button, color, selector);
	}

	void Button::addBtnStyle(const lv_style_t* style, lv_style_selector_t selector)
	{
		UI_LOCK();
		lv_obj_add_style(m_button, style, selector);
	}
} // namespace UI
