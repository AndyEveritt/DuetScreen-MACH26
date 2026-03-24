/*
 * MotionSystemPanel.h
 *
 *  Created on: 2026-03-24
 */

#pragma once

#include "UI/Components/Bar/CurrentTargetBar.h"
#include "UI/Components/LVGL/LvArc.h"
#include "UI/Components/LVGL/LvContainer.h"
#include "UI/Components/LVGL/LvLabel.h"
#include "UI/Components/LVGL/LvScale.h"

namespace UI
{
	class MotionSystemPanel : public LvContainer
	{
	  public:
		MotionSystemPanel(const std::string& name, LvObj& parent);

		void setTitle(std::string_view title);
		void setTool(std::string_view toolName);
		void setSpeedFactor(uint32_t speedFactorPercent);
		void setSpeeds(float currentSpeed, float targetSpeed);

	  private:
		static constexpr int32_t SpeedFactorArcMax = 100;

		static int32_t sanitizeBarValue(float value);

        LvContainer m_headerCont{"header_cont", getRoot()};
		LvLabel m_title{"title", m_headerCont};
		LvLabel m_tool{"tool", m_headerCont};

        LvContainer m_speedCont{"speed_cont", getRoot()};
        LvLabel m_speedFactorLabel{"speed_factor_label", m_speedCont};
		LvArc m_speedFactorArc{"speed_factor_arc", m_speedCont};
        LvScale m_scale{"scale", m_speedFactorArc};
		LvLabel m_speedFactorValue{"speed_factor_value", m_speedFactorArc};

        LvLabel m_speedBarLabel{"speed_bar_label", m_speedCont};
		CurrentTargetBar m_speedBar{"speed_bar", m_speedCont};
	};
} // namespace UI