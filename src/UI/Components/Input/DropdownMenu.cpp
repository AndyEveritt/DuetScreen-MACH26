/*
 * DropdownMenu.cpp
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#include "DropdownMenu.h"
#include "Debug.h"

namespace UI
{
	DropdownMenu::DropdownMenu(const std::string& name, lv_obj_t* parent, layout_t layout)
		: LvObj(lv_obj_create, name, parent, layout)
		, m_label(lv_label_create(getCont()))
		, m_dropdown(lv_dropdown_create(getCont()))
	{
		UI_LOCK();
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		// Label
		lv_obj_set_size(m_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

		// Dropdown
		lv_obj_set_size(m_dropdown, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_flex_grow(m_dropdown, 1);
		lv_dropdown_clear_options(m_dropdown);
	}

	void DropdownMenu::setLabel(const std::string& label)
	{
		UI_LOCK();
		lv_label_set_text(m_label, label.c_str());
	}
	void DropdownMenu::setText(const std::string& text)
	{
		UI_LOCK();
		lv_dropdown_set_text(m_dropdown, text.c_str());
	}
	const char* DropdownMenu::getText() const
	{
		UI_LOCK();
		return lv_dropdown_get_text(m_dropdown);
	}

	void DropdownMenu::setOptions(const std::string& options)
	{
		UI_LOCK();
		lv_dropdown_set_options(m_dropdown, options.c_str());
	}

	void DropdownMenu::setOptions(const std::vector<std::string>& options)
	{
		UI_LOCK();
		std::string opt;
		for (size_t i = 0; i < options.size(); ++i)
		{
			opt += options[i];
			if (i < options.size() - 1)
			{
				opt += "\n";
			}
		}
		lv_dropdown_set_options(m_dropdown, opt.c_str());
	}

	void DropdownMenu::addOption(const std::string& option, uint32_t pos)
	{
		UI_LOCK();
		lv_dropdown_add_option(m_dropdown, option.c_str(), pos);
	}

	void DropdownMenu::clearOptions()
	{
		UI_LOCK();
		lv_dropdown_clear_options(m_dropdown);
	}

	void DropdownMenu::setSelected(uint32_t selected)
	{
		UI_LOCK();
		lv_dropdown_set_selected(m_dropdown, selected);
	}

	void DropdownMenu::setDir(lv_dir_t dir)
	{
		UI_LOCK();
		lv_dropdown_set_dir(m_dropdown, dir);
	}

	void DropdownMenu::setSymbol(const void* symbol)
	{
		UI_LOCK();
		lv_dropdown_set_symbol(m_dropdown, symbol);
	}

	void DropdownMenu::setSelectedHighlight(bool en)
	{
		UI_LOCK();
		lv_dropdown_set_selected_highlight(m_dropdown, en);
	}

	const char* DropdownMenu::getOptions() const
	{
		UI_LOCK();
		return lv_dropdown_get_options(m_dropdown);
	}
	uint32_t DropdownMenu::getSelected() const
	{
		UI_LOCK();
		return lv_dropdown_get_selected(m_dropdown);
	}

	uint32_t DropdownMenu::getOptionCount() const
	{
		UI_LOCK();
		return lv_dropdown_get_option_count(m_dropdown);
	}

	std::string DropdownMenu::getSelectedString() const
	{
		UI_LOCK();
		char buf[64];
		lv_dropdown_get_selected_str(m_dropdown, buf, sizeof(buf));
		return std::string(buf);
	}

	int32_t DropdownMenu::getOptionIndex(const std::string& option) const
	{
		UI_LOCK();
		return lv_dropdown_get_option_index(m_dropdown, option.c_str());
	}

	const char* DropdownMenu::getSymbol() const
	{
		UI_LOCK();
		return lv_dropdown_get_symbol(m_dropdown);
	}

	bool DropdownMenu::getSelectedHighlight() const
	{
		UI_LOCK();
		return lv_dropdown_get_selected_highlight(m_dropdown);
	}

	lv_dir_t DropdownMenu::getDir() const
	{
		UI_LOCK();
		return lv_dropdown_get_dir(m_dropdown);
	}

	void DropdownMenu::open()
	{
		UI_LOCK();
		lv_dropdown_open(m_dropdown);
	}

	void DropdownMenu::close()
	{
		UI_LOCK();
		lv_dropdown_close(m_dropdown);
	}

	bool DropdownMenu::isOpen() const
	{
		UI_LOCK();
		return lv_dropdown_is_open(m_dropdown);
	}

	void DropdownMenu::addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData)
	{
		UI_LOCK();
		lv_obj_add_event_cb(m_dropdown, cb, code, userData);
	}
} // namespace UI
