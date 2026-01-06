/*
 * LvDropdown.h
 *
 *  Created on: 2025-06-13
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/LVGL/generated/LvDropdown.gen.h"
#include <span>
#include <string>

namespace UI
{
	class LvDropdown : public LvDropdownGen
	{
	  public:
		using LvDropdownGen::LvDropdownGen;

		/* Override LvDropdownGen text methods so text is never NULL */

		void setText(const std::string& text);
		const std::string& getText() const;
		void clearText();

		void setOptions(std::span<std::string> options);
		void addOption(const std::string& option, uint32_t pos) { LvDropdownGen::addOption(option.c_str(), pos); }
		using LvDropdownGen::setSelected;
		bool setSelected(const std::string& option);

		std::string getSelectedString() const;
		int32_t getOptionIndex(const std::string& option) const
		{
			return LvDropdownGen::getOptionIndex(option.c_str());
		}

	  private:
		std::string m_text;
	};
} // namespace UI
