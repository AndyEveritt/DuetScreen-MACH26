/*
 * MotionSystemPanel.cpp
 *
 *  Created on: 2026-03-24
 */

#include "MotionSystemPanel.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "UI/Styles/Themes/DefaultTheme.h"
#include "i18n/i18n.h"
#include <algorithm>
#include <cmath>

namespace UI
{
	MotionSystemPanel::MotionSystemPanel(const std::string& name, LvObj& parent)
		: LvContainer(name, parent)
	{
		ZoneScoped;
		UI_LOCK();

		setFlexFlow(LV_FLEX_FLOW_COLUMN);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_headerCont.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_speedCont.setWidth(LV_PCT(70));
		m_speedCont.setFlexGrow(1);

		m_headerCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_headerCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_speedCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_speedCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		static Themes::Style shadowStyle;

		lv_style_set_radius(shadowStyle, 3);
		lv_style_set_border_width(shadowStyle, 5);
		lv_style_set_border_color(shadowStyle, lv_color_white());
		lv_style_set_border_opa(shadowStyle, LV_OPA_30);
		lv_style_set_border_side(shadowStyle, LV_BORDER_SIDE_TOP);
		lv_style_set_shadow_color(shadowStyle, lv_color_black());
		lv_style_set_shadow_offset_y(shadowStyle, 5);
		lv_style_set_shadow_opa(shadowStyle, LV_OPA_30);
		lv_style_set_shadow_width(shadowStyle, 10);

		m_headerCont.addStyle(Themes::getLvglStyles().bg_light);
		m_speedCont.addStyle(Themes::getLvglStyles().bg_light);
		m_headerCont.addStyle(shadowStyle);
		m_speedCont.addStyle(shadowStyle);
		m_title.addStyle(Themes::getLvglStyles().text_emphasis);

		m_speedFactorLabel.setText("Speed Multiplier:");
		m_speedFactorArc.setWidth(LV_PCT(100));
		m_speedFactorArc.setFlexGrow(1);
		m_speedFactorArc.setRange(0, SpeedFactorArcMax);
		{
			constexpr int32_t arcGap = 60;
			constexpr int32_t arcEnd = 360 - arcGap;
			constexpr int32_t arcRotation = 90 + arcGap / 2;

			m_speedFactorArc.setBgAngles(0, arcEnd);
			m_speedFactorArc.setAngles(0, arcEnd);
			m_speedFactorArc.setRotation(arcRotation);

			m_scale.setAngleRange(arcEnd);
			m_scale.setRotation(arcRotation);
		}
		m_speedFactorArc.setStyleArcWidth(20);
		m_speedFactorArc.setStyleArcWidth(20, LV_PART_INDICATOR);
		m_speedFactorArc.setCenterAlign(LV_ALIGN_CENTER);
		m_speedFactorArc.setMode(LV_ARC_MODE_NORMAL);
		m_speedFactorArc.setValue(SpeedFactorArcMax);
		m_speedFactorArc.setFlag(LV_OBJ_FLAG_CLICKABLE, false);
		m_speedFactorArc.setFlag(LV_OBJ_FLAG_CLICK_FOCUSABLE, false);
		m_speedFactorArc.setStyleBgOpa(LV_OPA_0, LV_PART_KNOB);

		m_scale.setAlign(LV_ALIGN_CENTER);
		m_scale.setCenterAlign(LV_ALIGN_CENTER);
		m_scale.setMode(LV_SCALE_MODE_ROUND_INNER);
		m_speedFactorArc.addEventCallback(
			[this](lv_event_t*)
			{
				const lv_coord_t arcSize = std::min(m_speedFactorArc.getWidth(), m_speedFactorArc.getHeight());
				const lv_coord_t scaleSize = arcSize - m_speedFactorArc.getStyleProp(LV_STYLE_ARC_WIDTH).num * 2;
				m_scale.setSize(scaleSize, scaleSize);
			},
			LV_EVENT_SIZE_CHANGED);
		m_scale.setSize(LV_PCT(100), LV_PCT(100));

		m_speedFactorValue.setText("100%");
		m_speedFactorValue.setAlign(LV_ALIGN_CENTER, 0, 0);
		m_speedFactorValue.addStyle(Themes::getLvglStyles().text_emphasis);

		m_speedBarLabel.setText("Speed:");
		m_speedBar.setSize(LV_PCT(100), 50);
		m_speedBar.setRange(0, 600);
		m_speedBar.setValues(0, 0, LV_ANIM_OFF);
		m_speedBar.setLabel(_("status.speed_label", 0.0f, 0.0f));
		setSpeedFactor(100);
	}

	void MotionSystemPanel::setTitle(std::string_view title)
	{
		ZoneScoped;
		m_title.setText(title);
	}

	void MotionSystemPanel::setTool(std::string_view toolName)
	{
		ZoneScoped;
		m_tool.setText(toolName);
	}

	void MotionSystemPanel::setSpeedFactor(uint32_t speedFactorPercent)
	{
		ZoneScoped;
		const auto clamped =
			static_cast<int32_t>(std::min(speedFactorPercent, static_cast<uint32_t>(SpeedFactorArcMax)));
		m_speedFactorArc.setValue(clamped);
		m_speedFactorValue.setText(fmt::format("{:d}%", speedFactorPercent));
	}

	void MotionSystemPanel::setSpeeds(float currentSpeed, float targetSpeed)
	{
		ZoneScoped;
		const int32_t current = sanitizeBarValue(currentSpeed);
		const int32_t target = sanitizeBarValue(targetSpeed);

		m_speedBar.setValues(current, target);
		m_speedBar.setLabel(_("status.speed_label", currentSpeed, targetSpeed));
	}

	int32_t MotionSystemPanel::sanitizeBarValue(float value)
	{
		if (!std::isfinite(value))
		{
			return 0;
		}

		return std::max(0, static_cast<int32_t>(std::lround(value)));
	}
} // namespace UI