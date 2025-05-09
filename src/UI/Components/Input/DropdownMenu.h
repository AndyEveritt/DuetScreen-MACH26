/*
 * TextBox.h
 *
 *  Created on: 2025-05-08
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/View.h"

namespace UI
{
	class DropdownMenu : public BaseView
	{
	  public:
		DropdownMenu(const std::string& name, lv_obj_t* parent, layout_t layout);

		void setLabel(const std::string& label);
		void setText(const std::string& text);
		const char* getText() const;
		lv_obj_t* getDropdownMenu() const { return m_dropdown; }

		void setOptions(const std::string& options);
		void setOptions(const std::vector<std::string>& options);
		void addOption(const std::string& option, uint32_t pos = LV_DROPDOWN_POS_LAST);
		void clearOptions();
		void setSelected(uint32_t selected, lv_anim_enable_t anim_en = LV_ANIM_OFF);
		void setDir(lv_dir_t dir);
		void setSymbol(const void* symbol);
		void setSelectedHighlight(bool en);

		const char* getOptions() const;
		uint32_t getSelected() const;
		uint32_t getOptionCount() const;
		std::string getSelectedString() const;
		int32_t getOptionIndex(const std::string& option) const;
		const char* getSymbol() const;
		bool getSelectedHighlight() const;
		lv_dir_t getDir() const;

		void open();
		void close();
		bool isOpen() const;

		void addEventCallback(lv_event_cb_t cb, lv_event_code_t code, void* userData);

	  private:
		lv_obj_t* m_label;
		lv_obj_t* m_dropdown;
	};
} // namespace UI
