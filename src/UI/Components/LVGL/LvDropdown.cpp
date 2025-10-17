/*
 * LvDropdown.cpp
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#include "LvDropdown.h"
#include "Debug.h"

namespace UI
{
	LvDropdown::LvDropdown(const std::string& name, LvObj& parent)
		: LvObj(lv_dropdown_create, name, parent)
	{
	}

	void LvDropdown::setText(const std::string& text)
	{
		UI_LOCK();
		m_text = text;
		lv_dropdown_set_text(getRootPtr(), m_text.c_str());
	}

	const std::string& LvDropdown::getText() const
	{
		UI_LOCK();
		return m_text;
	}

	void LvDropdown::clearText()
	{
		UI_LOCK();
		m_text.clear();
		lv_dropdown_set_text(getRootPtr(), NULL);
	}

	void LvDropdown::setOptions(const std::string& options)
	{
		UI_LOCK();
		lv_dropdown_set_options(getRootPtr(), options.c_str());
	}

	void LvDropdown::setOptions(const std::vector<std::string>& options)
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
		lv_dropdown_set_options(getRootPtr(), opt.c_str());
	}

	void LvDropdown::addOption(const std::string& option, uint32_t pos)
	{
		UI_LOCK();
		lv_dropdown_add_option(getRootPtr(), option.c_str(), pos);
	}

	void LvDropdown::clearOptions()
	{
		UI_LOCK();
		lv_dropdown_clear_options(getRootPtr());
	}

	void LvDropdown::setSelected(uint32_t selected)
	{
		UI_LOCK();
		lv_dropdown_set_selected(getRootPtr(), selected);
	}

	bool LvDropdown::setSelected(const std::string& option)
	{
		UI_LOCK();
		int32_t index = getOptionIndex(option);
		if (index < 0)
		{
			if (!option.empty())
			{
				LOG_WARN("Failed to find option '{}' in dropdown", option);
			}
			setSelectedHighlight(false);
			return false;
		}
		setSelected(index);
		setSelectedHighlight(true);
		return true;
	}

	void LvDropdown::setDir(lv_dir_t dir)
	{
		UI_LOCK();
		lv_dropdown_set_dir(getRootPtr(), dir);
	}

	void LvDropdown::setSymbol(const void* symbol)
	{
		UI_LOCK();
		lv_dropdown_set_symbol(getRootPtr(), symbol);
	}

	void LvDropdown::setSelectedHighlight(bool en)
	{
		UI_LOCK();
		lv_dropdown_set_selected_highlight(getRootPtr(), en);
	}

	const char* LvDropdown::getOptions() const
	{
		UI_LOCK();
		return lv_dropdown_get_options(getRootPtr());
	}
	uint32_t LvDropdown::getSelected() const
	{
		UI_LOCK();
		return lv_dropdown_get_selected(getRootPtr());
	}

	uint32_t LvDropdown::getOptionCount() const
	{
		UI_LOCK();
		return lv_dropdown_get_option_count(getRootPtr());
	}

	std::string LvDropdown::getSelectedString() const
	{
		UI_LOCK();
		char buf[64];
		lv_dropdown_get_selected_str(getRootPtr(), buf, sizeof(buf));
		return std::string(buf);
	}

	int32_t LvDropdown::getOptionIndex(const std::string& option) const
	{
		UI_LOCK();
		return lv_dropdown_get_option_index(getRootPtr(), option.c_str());
	}

	const char* LvDropdown::getSymbol() const
	{
		UI_LOCK();
		return lv_dropdown_get_symbol(getRootPtr());
	}

	bool LvDropdown::getSelectedHighlight() const
	{
		UI_LOCK();
		return lv_dropdown_get_selected_highlight(getRootPtr());
	}

	lv_dir_t LvDropdown::getDir() const
	{
		UI_LOCK();
		return lv_dropdown_get_dir(getRootPtr());
	}

	void LvDropdown::open()
	{
		UI_LOCK();
		lv_dropdown_open(getRootPtr());
	}

	void LvDropdown::close()
	{
		UI_LOCK();
		lv_dropdown_close(getRootPtr());
	}

	bool LvDropdown::isOpen() const
	{
		UI_LOCK();
		return lv_dropdown_is_open(getRootPtr());
	}
} // namespace UI
