/*
 * CurrentTargetBar.cpp
 *
 *  Created on: 2026-03-24
 */

#include "CurrentTargetBar.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"

namespace UI
{
	CurrentTargetBar::CurrentTargetBar(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		ZoneScoped;
		UI_LOCK();

		setStylePad(0);
		setMinHeight(LV_SIZE_CONTENT);

		m_targetBar.setSize(LV_PCT(100), LV_PCT(100));
		m_targetBar.setMinHeight(LV_SIZE_CONTENT);
		m_targetBar.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_targetBar.addStyle(Themes::getLvglStyles().bg_color_primary_muted, LV_PART_INDICATOR);
		m_targetBar.addStyle(Themes::getLvglStyles().anim_fast);

		m_currentBar.setSize(LV_PCT(100), LV_PCT(100));
		m_currentBar.setMinHeight(LV_SIZE_CONTENT);
		m_currentBar.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_currentBar.setStyleBgOpa(LV_OPA_0);

		m_label.setAlign(LV_ALIGN_LEFT_MID, 10, 0);
		m_label.addStyle(Themes::getComponentStyles().bar_label_bg);
		m_label.addStyle(Themes::getComponentStyles().bar_label);
	}

	void CurrentTargetBar::setRange(int32_t min, int32_t max)
	{
		ZoneScoped;
		m_currentBar.setRange(min, max);
		m_targetBar.setRange(min, max);
	}

	void CurrentTargetBar::setValues(int32_t currentValue, int32_t targetValue, lv_anim_enable_t anim)
	{
		ZoneScoped;
		m_currentBar.setValue(currentValue, anim);
		m_targetBar.setValue(targetValue, anim);
	}

	void CurrentTargetBar::setLabel(std::string_view text)
	{
		ZoneScoped;
		m_label.setText(text);
	}
} // namespace UI