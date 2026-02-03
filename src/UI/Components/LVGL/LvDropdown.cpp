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
	void LvDropdown::setText(const std::string& text)
	{
		LvDropdownGen::setText(text.c_str());
	}

	void LvDropdown::clearText()
	{
		LvDropdownGen::setText(nullptr);
	}

	void LvDropdown::setOptions(std::span<std::string> options)
	{
		UI_LOCK();
		std::string opt;
		for (const auto& option : options)
		{
			opt += option;
			if (&option != &options.back())
			{
				opt += "\n";
			}
		}
		LvDropdownGen::setOptions(opt.c_str());
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
		LvDropdownGen::setSelected(index);
		setSelectedHighlight(true);
		return true;
	}

	std::string LvDropdown::getSelectedString() const
	{
		UI_LOCK();
		char buf[64];
		LvDropdownGen::getSelectedStr(buf, sizeof(buf));
		return std::string(buf);
	}
} // namespace UI
