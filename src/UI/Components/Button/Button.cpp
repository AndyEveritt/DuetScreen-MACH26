#include "Button.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	Button::Button(const std::string& name, LvObj& parent)
		: LvObj(lv_button_create, name, parent)
		, m_icon("icon", getRoot())
		, m_label("label", getRoot())
	{
		ZoneScoped;
		init("");
	}

	Button::Button(const std::string& name, LvObj& parent, std::string_view text)
		: LvObj(lv_button_create, name, parent)
		, m_icon("icon", getRoot())
		, m_label("label", getRoot())
	{
		ZoneScoped;
		init(text);
	}

	Button::Button(const std::string& name, LvObj& parent, std::string_view text, layout_t layout)
		: LvObj(lv_button_create, name, parent, layout)
		, m_icon("icon", getRoot())
		, m_label("label", getRoot())
	{
		ZoneScoped;
		init(text);
	}

	void Button::init(std::string_view text)
	{
		ZoneScoped;
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
		m_label.setLongMode(LV_LABEL_LONG_MODE_DOTS);

		// icon
		m_icon.hide();
		m_icon.setWidth(LV_PCT(100));
		m_icon.setFlexGrow(1);
		m_icon.setMinHeight(25);
		m_icon.setMinWidth(25);

		addEventCallback(
			[](lv_event_t* e)
			{
				// Update the label width
				auto& btn = *static_cast<Button*>(lv_event_get_user_data(e));
				LvObj* parent = btn.getParent();

				if (!parent)
				{
					LOG_DBG("Button has no parent, cannot adjust label size");
					return;
				}

				// while (parent != nullptr)
				// {

				// 	if (!(LV_COORD_IS_PCT(parent_width) && parent_min_width == LV_SIZE_CONTENT))
				// 	{
				// 		can_set_full_width = true;
				// 		break;
				// 	}

				// 	parent = lv_obj_get_parent(parent);
				// }

				lv_layout_t parent_layout =
					static_cast<lv_layout_t>(lv_obj_get_style_layout(parent->getRootPtr(), LV_PART_MAIN));
				lv_flex_flow_t parent_flex_flow = lv_obj_get_style_flex_flow(parent->getRootPtr(), LV_PART_MAIN);
#if 0
				int32_t parent_width = lv_obj_get_style_width(parent->getRootPtr(), LV_PART_MAIN);
				int32_t parent_height = lv_obj_get_style_height(parent->getRootPtr(), LV_PART_MAIN);
				int32_t parent_min_width = lv_obj_get_style_min_width(parent->getRootPtr(), LV_PART_MAIN);
				int32_t parent_min_height = lv_obj_get_style_min_height(parent->getRootPtr(), LV_PART_MAIN);
#endif

				uint8_t flex_grow = lv_obj_get_style_flex_grow(btn.getRootPtr(), LV_PART_MAIN);
				int32_t width = lv_obj_get_style_width(btn.getRootPtr(), LV_PART_MAIN);
				int32_t height = lv_obj_get_style_height(btn.getRootPtr(), LV_PART_MAIN);
				int32_t min_width = lv_obj_get_style_min_width(btn.getRootPtr(), LV_PART_MAIN);
				int32_t min_height = lv_obj_get_style_min_height(btn.getRootPtr(), LV_PART_MAIN);

				if ((width != LV_SIZE_CONTENT && min_width != LV_SIZE_CONTENT) ||
					(parent_layout == LV_LAYOUT_FLEX && parent_flex_flow == LV_FLEX_FLOW_ROW && flex_grow > 0))
				{
					btn.m_label.setWidth(LV_PCT(100));
				}
				else
				{
					btn.m_label.setWidth(LV_SIZE_CONTENT);
				}

				if ((height != LV_SIZE_CONTENT && min_height != LV_SIZE_CONTENT) ||
					(parent_layout == LV_LAYOUT_FLEX && parent_flex_flow == LV_FLEX_FLOW_COLUMN && flex_grow > 0))
				{
					btn.m_label.setMaxHeight(LV_PCT(100));
				}
				else
				{
					btn.m_label.setMaxHeight(LV_SIZE_CONTENT);
				}
			},
			static_cast<lv_event_code_t>(LV_EVENT_STYLE_CHANGED),
			this);

		m_icon.addEventCallback([this](lv_event_t*) { updateIconVisibility(); }, LV_EVENT_REFRESH);
	}

	void Button::setText(std::string_view text)
	{
		ZoneScoped;
		UI_LOCK();
		if (text == m_label.getText())
		{
			return;
		}
		m_label.setText(text);
		m_label.setVisible(!text.empty());
	}

	void Button::addClickedCallback(lv_event_cb_t event_cb, void* user_data)
	{
		ZoneScoped;
		addEventCallback(event_cb, LV_EVENT_CLICKED, user_data);
	}

	void Button::addClickedCallback(std::function<void(lv_event_t*)> event_cb)
	{
		ZoneScoped;
		addEventCallback(std::move(event_cb), LV_EVENT_CLICKED);
	}

	void Button::setIcon(std::string_view icon)
	{
		ZoneScoped;
		UI_LOCK();
		// If the icon is null, remove the icon and center the label
		m_icon.setIcon(icon);
		updateIconVisibility();
	}

	void Button::setFixedIcon(const std::string& icon_path)
	{
		ZoneScoped;
		UI_LOCK();
		m_icon.setFixedIcon(icon_path);
		updateIconVisibility();
	}

	void Button::clearIcon()
	{
		ZoneScoped;
		UI_LOCK();
		m_icon.clearIcon();
		updateIconVisibility();
	}

	void Button::updateIconVisibility()
	{
		ZoneScoped;
		UI_LOCK();
		// Check the icon has loaded correctly
		bool icon_loaded = m_icon.getSrc() != nullptr;
		if (!icon_loaded)
		{
			LOG_WARN("Failed to load icon from path: {}", m_icon.getIconName());
		}
		m_icon.setVisible(icon_loaded);
	}

	void Button::setCheckable(bool checkable)
	{
		ZoneScoped;
		UI_LOCK();
		setFlag(LV_OBJ_FLAG_CHECKABLE, checkable);
	}

	void Button::setChecked(const bool checked)
	{
		ZoneScoped;
		UI_LOCK();
		setState(LV_STATE_CHECKED, checked);
	}

	bool Button::getChecked() const
	{
		ZoneScoped;
		UI_LOCK();
		if (hasFlag(LV_OBJ_FLAG_CHECKABLE))
		{
			return hasState(LV_STATE_CHECKED);
		}
		return false;
	}

	void Button::setDisabled(bool disabled)
	{
		ZoneScoped;
		setState(LV_STATE_DISABLED, disabled);
	}
} // namespace UI
