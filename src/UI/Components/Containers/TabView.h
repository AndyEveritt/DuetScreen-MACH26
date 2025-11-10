/*
 * TabView.h
 *
 *  Created on: 2025-11-06
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Components/Button/Button.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/List/List.h"
#include <vector>

namespace UI
{
	class TabView : public LvContainer
	{
	  public:
		using TabButton = Button;

		TabView(const std::string& name, LvObj& parent);

		LvContainer& addTab(std::string_view tab_name);
		bool renameTab(size_t index, std::string_view new_name);

		LvContainer& getActiveTab();
		TabButton* getTabButton(size_t index);
		LvContainer* getTab(size_t index);
		size_t getTabCount() const { return m_tabs.size(); };
		size_t getActiveTabIndex() const { return m_currentTabIndex; };

		void setActiveTab(size_t index);
		void setTabBarPosition(lv_dir_t dir, bool outer = false);

		bool disableTab(size_t index, bool disable);

	  private:
		List<TabButton> m_tabButtons{"tab_buttons", getRoot()};
		LvContainer m_tabContent{"tab_content", getRoot()};

		std::vector<std::unique_ptr<LvContainer>> m_tabs;
		size_t m_currentTabIndex = -1;
	};
} // namespace UI
