#include "Button.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	Button::Button(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_button_create, name, parent)
		, m_label("label", getRoot())
		, m_icon(nullptr)
	{
		init("");
	}

	Button::Button(const std::string& name, lv_obj_t* parent, std::string_view text)
		: LvObj(lv_button_create, name, parent)
		, m_label("label", getRoot())
		, m_icon(nullptr)
	{
		init(text);
	}

	Button::Button(const std::string& name, lv_obj_t* parent, std::string_view text, layout_t layout)
		: LvObj(lv_button_create, name, parent, layout)
		, m_label("label", getRoot())
		, m_icon(nullptr)
	{
		init(text);
	}

	void Button::init(std::string_view text)
	{
		UI_LOCK();
		setUserData(this);
		m_label.setUserData(this);

		setMinHeight(30);
		setMinWidth(50);

		// Initialise the label obj
		m_label.setText(text);
		m_label.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_label.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_label.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);
	}

	void Button::setText(std::string_view text)
	{
		UI_LOCK();
		m_label.setText(text);
		if (m_icon != nullptr)
		{
			lv_obj_set_y(m_icon, LV_PCT(-20));
			lv_obj_set_y(m_label, LV_PCT(30));
		}
	}

	void Button::addClickedCallback(lv_event_cb_t event_cb, void* user_data)
	{
		UI_LOCK();
		lv_obj_add_event_cb(getRoot(), event_cb, LV_EVENT_CLICKED, user_data);
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
			m_icon = lv_image_create(getRoot());
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

	void Button::setDisabled(bool disabled)
	{
		setState(LV_STATE_DISABLED, disabled);
	}
} // namespace UI
