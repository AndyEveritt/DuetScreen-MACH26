#include "Button.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	Button::Button(const std::string& name, lv_obj_t* parent, const std::string& text)
		: LvObj(lv_button_create, name, parent)
		, m_label(name + "_label", getCont())
		, m_icon(nullptr)
	{
		init(text);
	}

	Button::Button(const std::string& name, lv_obj_t* parent, const std::string& text, layout_t layout)
		: LvObj(lv_button_create, name, parent, layout)
		, m_label(name + "_label", getCont())
		, m_icon(nullptr)
	{
		init(text);
	}

	void Button::init(const std::string& text)
	{
		UI_LOCK();
		setUserData(this);
		lv_obj_set_user_data(getCont(), this);
		lv_obj_set_user_data(m_label, this);

		setMinHeight(30);
		setMinWidth(50);

		// Initialise the label obj
		m_label.setText(text);
		m_label.setPos(0, 0);
		m_label.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_align(m_label, LV_ALIGN_CENTER, 0);
		lv_obj_set_style_text_align(m_label, LV_TEXT_ALIGN_CENTER, 0);
		// lv_obj_update_layout(getCont());
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

	void Button::addClickedCallback(lv_event_cb_t event_cb, void* user_data)
	{
		UI_LOCK();
		lv_obj_add_event_cb(getCont(), event_cb, LV_EVENT_CLICKED, user_data);
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
			m_icon = lv_image_create(getCont());
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
		setFlag(LV_OBJ_FLAG_CHECKABLE, checkable);
	}

	void Button::setChecked(const bool checked)
	{
		UI_LOCK();
		setState(LV_STATE_CHECKED, checked);
	}

	const bool Button::getChecked() const
	{
		UI_LOCK();
		if (hasFlag(LV_OBJ_FLAG_CHECKABLE))
		{
			return hasState(LV_STATE_CHECKED);
		}
		return false;
	}

	void Button::setInvalid(bool invalid)
	{
		UI_LOCK();
		setState(LV_STATE_DISABLED, invalid);
	}
} // namespace UI
