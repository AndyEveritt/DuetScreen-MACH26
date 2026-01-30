/*
 * DropdownMenu.cpp
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#include "DropdownMenu.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	DropdownMenu::DropdownMenu(const std::string& name, LvObj& parent)
		: LvObj(lv_obj_create, name, parent)
		, m_label("label", getRoot())
		, m_dropdown("dropdown", getRoot())
	{
		ZoneScoped;
		init();
	}

	DropdownMenu::DropdownMenu(const std::string& name, LvObj& parent, layout_t layout)
		: LvObj(lv_obj_create, name, parent, layout)
		, m_label("label", getRoot())
		, m_dropdown("dropdown", getRoot())
	{
		ZoneScoped;
		init();
	}

	void DropdownMenu::init()
	{
		ZoneScoped;
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Label
		m_label.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		// Dropdown
		m_dropdown.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		m_dropdown.setFlexGrow(1);
		m_dropdown.clearOptions();

		m_dropdown.addEventCallback(
			[](lv_event_t* e)
			{
				UI_LOCK();
				DropdownMenu* menu = static_cast<DropdownMenu*>(lv_event_get_user_data(e));
				if (menu->m_selectedCallback)
				{
					menu->m_selectedCallback(menu->m_dropdown.getSelected(), menu->m_dropdown.getSelectedString());
				}
			},
			LV_EVENT_VALUE_CHANGED,
			this);

		setLabel("");
	}

	void DropdownMenu::setLabel(const std::string& label)
	{
		ZoneScoped;
		m_label.setFlag(LV_OBJ_FLAG_HIDDEN, label.empty());
		m_label.setText(label);
	}
	void DropdownMenu::setText(const std::string& text)
	{
		ZoneScoped;
		m_dropdown.setText(text);
	}
	std::string_view DropdownMenu::getText() const
	{
		ZoneScoped;
		return m_dropdown.getText();
	}

	void DropdownMenu::clearText()
	{
		ZoneScoped;
		m_dropdown.clearText();
	}

	void DropdownMenu::setOptions(std::span<std::string> options)
	{
		ZoneScoped;
		m_dropdown.setOptions(options);
	}

	void DropdownMenu::addOption(const std::string& option, uint32_t pos)
	{
		ZoneScoped;
		m_dropdown.addOption(option, pos);
	}

	void DropdownMenu::clearOptions()
	{
		ZoneScoped;
		m_dropdown.clearOptions();
	}

	void DropdownMenu::setSelected(uint32_t selected)
	{
		ZoneScoped;
		m_dropdown.setSelected(selected);
	}

	bool DropdownMenu::setSelected(const std::string& option)
	{
		ZoneScoped;
		return m_dropdown.setSelected(option);
	}

	void DropdownMenu::setDir(lv_dir_t dir)
	{
		ZoneScoped;
		m_dropdown.setDir(dir);
	}

	void DropdownMenu::setSymbol(const void* symbol)
	{
		ZoneScoped;
		m_dropdown.setSymbol(symbol);
	}

	void DropdownMenu::setSelectedHighlight(bool en)
	{
		ZoneScoped;
		m_dropdown.setSelectedHighlight(en);
	}

	std::string_view DropdownMenu::getOptions() const
	{
		ZoneScoped;
		return m_dropdown.getOptions();
	}

	uint32_t DropdownMenu::getSelected() const
	{
		ZoneScoped;
		return m_dropdown.getSelected();
	}

	uint32_t DropdownMenu::getOptionCount() const
	{
		ZoneScoped;
		return m_dropdown.getOptionCount();
	}

	std::string DropdownMenu::getSelectedString() const
	{
		ZoneScoped;
		return m_dropdown.getSelectedString();
	}

	int32_t DropdownMenu::getOptionIndex(const std::string& option) const
	{
		ZoneScoped;
		return m_dropdown.getOptionIndex(option);
	}

	const char* DropdownMenu::getSymbol() const
	{
		ZoneScoped;
		return m_dropdown.getSymbol();
	}

	bool DropdownMenu::getSelectedHighlight() const
	{
		ZoneScoped;
		return m_dropdown.getSelectedHighlight();
	}

	lv_dir_t DropdownMenu::getDir() const
	{
		ZoneScoped;
		return m_dropdown.getDir();
	}

	void DropdownMenu::open()
	{
		ZoneScoped;
		m_dropdown.open();
	}

	void DropdownMenu::close()
	{
		ZoneScoped;
		m_dropdown.close();
	}

	bool DropdownMenu::isOpen() const
	{
		ZoneScoped;
		return m_dropdown.isOpen();
	}

	void DropdownMenu::setSelectedCallback(selected_cb_t cb)
	{
		ZoneScoped;
		m_selectedCallback = cb;
	}

	void DropdownMenu::addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData)
	{
		ZoneScoped;
		m_dropdown.addEventCallback(cb, code, userData);
	}
} // namespace UI
