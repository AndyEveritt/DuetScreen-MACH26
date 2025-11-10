/*
 * TabView.cpp
 *
 *  Created on: 2025-11-06
 *      Author: Andy Everitt
 */

#include "TabView.h"
#include "Debug.h"

namespace UI
{
	TabView::TabView(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		setTabBarPosition(LV_DIR_TOP, false);
		setStylePad(0);
		setStylePad(-5, LV_PART_MAIN, Padding::ROW);
		setStylePad(-5, LV_PART_MAIN, Padding::COLUMN);

		m_tabButtons.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_tabButtons.setStylePad(0, LV_PART_MAIN, Padding::VERTICAL);
		m_tabButtons.getListContainer().setStylePad(0);
		m_tabButtons.getListContainer().setFlexFlow(LV_FLEX_FLOW_ROW);
		m_tabButtons.getListContainer().setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		m_tabButtons.addStyle(Themes::getLvglStyles().bg_color_header, LV_PART_MAIN);

		m_tabContent.setSize(LV_PCT(100), LV_PCT(100));
		m_tabContent.setFlexGrow(1);
		m_tabContent.setStylePad(0);
	}

	LvContainer& TabView::addTab(std::string_view tab_name)
	{
		UI_LOCK();
		// Create new tab container
		m_tabs.emplace_back(std::make_unique<LvContainer>(fmt::format("tab_{}", m_tabs.size()), m_tabContent));
		LvContainer& new_tab = *m_tabs.back();

		new_tab.setSize(LV_PCT(100), LV_PCT(100));
		new_tab.setStylePad(0);

		// Create corresponding tab button
		TabButton& tab_button =
			m_tabButtons.addItem([&](size_t index, LvObj& parent)
								 { return std::make_unique<TabButton>(fmt::format("tab_button_{}", index), parent); });
		tab_button.setText(tab_name);
		tab_button.setFlexGrow(1);
		tab_button.setSize(LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		tab_button.setMinWidth(LV_SIZE_CONTENT);
		tab_button.addClickedCallback(
			[](lv_event_t* e)
			{
				TabView* tab_view = static_cast<TabView*>(lv_event_get_user_data(e));
				LvObj* target = LvObj::fromPtr(lv_event_get_target_obj(e));
				size_t index = 0;
				tab_view->m_tabButtons.iterateListItems(
					[&](size_t i, LvObj& obj)
					{
						if (&obj == target)
						{
							index = i;
						}
					});
				tab_view->setActiveTab(index);
			},
			this);

		// If this is the first tab, activate it
		if (m_tabs.size() == 1)
		{
			setActiveTab(0);
		}
		else
		{
			new_tab.hide();
		}

		return new_tab;
	}

	bool TabView::renameTab(size_t index, std::string_view new_name)
	{
		UI_LOCK();
		if (TabButton* tab_button = m_tabButtons.getItem(index))
		{
			tab_button->setText(new_name);
			return true;
		}

		LOG_ERROR("Tab index {} out of range", index);
		return false;
	}

	LvContainer& TabView::getActiveTab()
	{
		UI_LOCK();
		return *m_tabs.at(m_currentTabIndex);
	}

	TabView::TabButton* TabView::getTabButton(size_t index)
	{
		UI_LOCK();
		return m_tabButtons.getItem(index);
	}

	LvContainer* TabView::getTab(size_t index)
	{
		UI_LOCK();
		if (index >= m_tabs.size())
		{
			return nullptr;
		}
		return m_tabs.at(index).get();
	}

	void TabView::setActiveTab(size_t index)
	{
		UI_LOCK();
		if (index == m_currentTabIndex)
		{
			return;
		}

		if (index >= m_tabs.size())
		{
			LOG_ERROR("Tab index {} out of range", index);
			return;
		}

		// Hide current tab
		if (m_currentTabIndex < m_tabs.size())
		{
			m_tabs.at(m_currentTabIndex)->hide();
		}

		// Show new tab
		m_currentTabIndex = index;
		m_tabs.at(index)->show(true);

		m_tabButtons.iterateListItems([&](size_t i, TabButton& button) { button.setChecked(i == index); });
	}

	void TabView::setTabBarPosition(lv_dir_t dir, bool outer)
	{
		UI_LOCK();
		if (!outer)
		{
			m_tabButtons.setFlag(LV_OBJ_FLAG_FLOATING, false);
			switch (dir)
			{
			case LV_DIR_TOP:
				setFlexFlow(LV_FLEX_FLOW_COLUMN);
				break;
			case LV_DIR_BOTTOM:
				setFlexFlow(LV_FLEX_FLOW_COLUMN_REVERSE);
				break;
			case LV_DIR_LEFT:
				setFlexFlow(LV_FLEX_FLOW_ROW);
				break;
			case LV_DIR_RIGHT:
				setFlexFlow(LV_FLEX_FLOW_ROW_REVERSE);
				break;
			default:
				LOG_ERROR("Invalid tab bar direction");
				return;
			}
			return;
		}

		// Outer tab bar
		m_tabButtons.setFlag(LV_OBJ_FLAG_FLOATING, true);
		setExtDrawSize(100);

		switch (dir)
		{
		case LV_DIR_TOP:
			m_tabButtons.setAlign(LV_ALIGN_OUT_TOP_LEFT, 0, 0);
			break;
		default:
			LOG_ERROR("Invalid outer tab bar direction");
			return;
		}
	}

	bool TabView::disableTab(size_t index, bool disable)
	{
		UI_LOCK();
		TabButton* tab_button = m_tabButtons.getItem(index);
		if (!tab_button)
		{
			LOG_ERROR("Tab index {} out of range", index);
			return false;
		}

		if (tab_button->hasState(LV_STATE_DISABLED) == disable)
		{
			return true; // Already in desired state
		}

		tab_button->setDisabled(disable);

		if (disable && index == m_currentTabIndex)
		{
			// If disabling the active tab, switch to the first available tab
			bool switched = false;
			for (size_t i = 0; i < m_tabs.size(); i++)
			{
				size_t shifted_index = (index + i + 1) % m_tabs.size();
				TabButton* tab_button = m_tabButtons.getItem(shifted_index);
				if (shifted_index != index && tab_button && !tab_button->hasState(LV_STATE_DISABLED))
				{
					setActiveTab(shifted_index);
					switched = true;
					break;
				}
			}
			if (!switched)
			{
				m_currentTabIndex = -1; // No active tab
				m_tabs.at(index)->hide();
			}
		}

		return true;
	}
} // namespace UI
