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
	LvDropdown::LvDropdown(const std::string& name, lv_obj_t* parent)
		: LvObj(lv_dropdown_create, name, parent)
	{
	}

	void LvDropdown::setText(const std::string& text)
	{
		UI_LOCK();
		lv_dropdown_set_text(getCont(), text.c_str());
	}

	const char* LvDropdown::getText() const
	{
		UI_LOCK();
		return lv_dropdown_get_text(getCont());
	}

	void LvDropdown::setOptions(const std::string& options)
	{
		UI_LOCK();
		lv_dropdown_set_options(getCont(), options.c_str());
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
		lv_dropdown_set_options(getCont(), opt.c_str());
	}

	void LvDropdown::addOption(const std::string& option, uint32_t pos)
	{
		UI_LOCK();
		lv_dropdown_add_option(getCont(), option.c_str(), pos);
	}

	void LvDropdown::clearOptions()
	{
		UI_LOCK();
		lv_dropdown_clear_options(getCont());
	}

	void LvDropdown::setSelected(uint32_t selected)
	{
		UI_LOCK();
		lv_dropdown_set_selected(getCont(), selected);
	}

	void LvDropdown::setDir(lv_dir_t dir)
	{
		UI_LOCK();
		lv_dropdown_set_dir(getCont(), dir);
	}

	void LvDropdown::setSymbol(const void* symbol)
	{
		UI_LOCK();
		lv_dropdown_set_symbol(getCont(), symbol);
	}

	void LvDropdown::setSelectedHighlight(bool en)
	{
		UI_LOCK();
		lv_dropdown_set_selected_highlight(getCont(), en);
	}

	const char* LvDropdown::getOptions() const
	{
		UI_LOCK();
		return lv_dropdown_get_options(getCont());
	}
	uint32_t LvDropdown::getSelected() const
	{
		UI_LOCK();
		return lv_dropdown_get_selected(getCont());
	}

	uint32_t LvDropdown::getOptionCount() const
	{
		UI_LOCK();
		return lv_dropdown_get_option_count(getCont());
	}

	std::string LvDropdown::getSelectedString() const
	{
		UI_LOCK();
		char buf[64];
		lv_dropdown_get_selected_str(getCont(), buf, sizeof(buf));
		return std::string(buf);
	}

	int32_t LvDropdown::getOptionIndex(const std::string& option) const
	{
		UI_LOCK();
		return lv_dropdown_get_option_index(getCont(), option.c_str());
	}

	const char* LvDropdown::getSymbol() const
	{
		UI_LOCK();
		return lv_dropdown_get_symbol(getCont());
	}

	bool LvDropdown::getSelectedHighlight() const
	{
		UI_LOCK();
		return lv_dropdown_get_selected_highlight(getCont());
	}

	lv_dir_t LvDropdown::getDir() const
	{
		UI_LOCK();
		return lv_dropdown_get_dir(getCont());
	}

	void LvDropdown::open()
	{
		UI_LOCK();
		lv_dropdown_open(getCont());
	}

	void LvDropdown::close()
	{
		UI_LOCK();
		lv_dropdown_close(getCont());
	}

	bool LvDropdown::isOpen() const
	{
		UI_LOCK();
		return lv_dropdown_is_open(getCont());
	}
} // namespace UI
