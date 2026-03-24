/*
 * CurrentTargetBar.h
 *
 *  Created on: 2026-03-24
 */

#pragma once

#include "UI/Components/LVGL/LvBar.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"

namespace UI
{
	class CurrentTargetBar : public LvContainer
	{
	  public:
		CurrentTargetBar(const std::string& name, LvObj& parent);

		void setRange(int32_t min, int32_t max);
		void setValues(int32_t currentValue, int32_t targetValue, lv_anim_enable_t anim = LV_ANIM_ON);
		void setLabel(std::string_view text);

	  private:
		LvBar m_targetBar{"target", getRoot()};
		LvBar m_currentBar{"current", getRoot()};
		LvLabel m_label{"label", m_currentBar};
	};
} // namespace UI