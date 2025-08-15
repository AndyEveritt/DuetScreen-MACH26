#include "Button.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	Button::Button(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_button_create, name, parent)
		, m_icon("icon", getRoot())
		, m_label("label", getRoot())
	{
		init("");
	}

	Button::Button(const std::string& name, lv_obj_t* parent, std::string_view text)
		: LvObj(lv_button_create, name, parent)
		, m_icon("icon", getRoot())
		, m_label("label", getRoot())
	{
		init(text);
	}

	Button::Button(const std::string& name, lv_obj_t* parent, std::string_view text, layout_t layout)
		: LvObj(lv_button_create, name, parent, layout)
		, m_icon("icon", getRoot())
		, m_label("label", getRoot())
	{
		init(text);
	}

	void Button::init(std::string_view text)
	{
		UI_LOCK();
		setUserData(this);
		m_label.setUserData(this);

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		setMinHeight(25);
		setMinWidth(25);

		// Initialise the label obj
		setText(text);
		m_label.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_label.setStyleTextAlign(LV_TEXT_ALIGN_CENTER);

		// icon
		m_icon.hide();
		m_icon.setWidth(LV_PCT(100));
		m_icon.setFlexGrow(1);
		m_icon.setMinHeight(25);
		m_icon.setMinWidth(25);
	}

	void Button::setText(std::string_view text)
	{
		UI_LOCK();
		m_label.setText(text);
		m_label.setVisible(!text.empty());
	}

	void Button::addClickedCallback(lv_event_cb_t event_cb, void* user_data)
	{
		UI_LOCK();
		lv_obj_add_event_cb(getRoot(), event_cb, LV_EVENT_CLICKED, user_data);
	}

	void Button::setIcon(const char* icon_path)
	{
		UI_LOCK();
		// If the icon is null, remove the icon and center the label
		m_icon.setSrc(icon_path);

		// Check the icon has loaded correctly
		bool icon_loaded = m_icon.getSrc() != nullptr;
		if (icon_path && !icon_loaded)
		{
			LOG_ERROR("Failed to load icon from path: {}", icon_path);
		}
		m_icon.setVisible(icon_loaded);
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
