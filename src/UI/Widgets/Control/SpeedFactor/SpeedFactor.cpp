/*
 * SpeedFactor.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "SpeedFactor.h"
#include "Debug.h"
#include "i18n/i18n.h"

namespace UI
{
	static constexpr float SPEED_FACTOR_MIN = 1.0f;

	SpeedFactor::SpeedFactor(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		UI_LOCK();

		m_speed.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_speed.setLabel(_("fine_tune.speed_factor"));
		m_speed.setFocusedCallback(
			[this](bool focused)
			{
				if (!focused || !m_numberPad)
					return;
				m_numberPad->setHeader(m_speed.getLabelText());
				m_numberPad->setValue(m_speed.getValue());
				m_numberPad->setMinValue(m_speed.getMin());
				m_numberPad->setMaxValue(m_speed.getMax());
				m_speed.getInput().sendEvent(LV_EVENT_DEFOCUSED, nullptr);
				m_numberPad->setConfirmCallback([this](float value) { m_speed.setValue(value); });
			});
		m_speed.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_speed.setRange(SPEED_FACTOR_MIN, 200);
		m_speed.setDefaultValue(100);
		m_speed.setValueChangedCallback(
			[this](float value)
			{
				m_speed.setSendMode(Slider::SendMode::DISABLED); // prevent callback loop
				m_speed.setRange(std::max(SPEED_FACTOR_MIN, value - 50), std::max(150.0f, value + 50));
				m_speed.setOutOfRangeMode(m_speed.getMin() <= SPEED_FACTOR_MIN ? Slider::OutOfRange::UPPER
																			   : Slider::OutOfRange::BOTH);
				m_speed.setSendMode(Slider::SendMode::VALUE_CONFIRMED);

				m_presenter->setSpeedFactor(static_cast<uint32_t>(value));
			});
		m_speed.addEventCallback(
			[this](lv_event_t*)
			{
				if (m_numberPad)
				{
					m_numberPad->setValue(m_speed.getValue());
				}
			},
			LV_EVENT_VALUE_CHANGED);
	}

	void SpeedFactor::setSpeedValue(float value)
	{
		m_speed.setValue(value);
	}

	ModalSpeedFactor::ModalSpeedFactor(const std::string& name, LvObj& parent)
		: Modal(name, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_speedFactor.setHeight(LV_PCT(100));
		m_speedFactor.setFlexGrow(1);
		m_speedFactor.setNumberPad(&m_numberPad);

		m_numberPad.setSize(LV_PCT(50), LV_PCT(100));
		m_numberPad.setCloseOnConfirm(false);

		auto& slider = m_speedFactor.getSlider();
		m_numberPad.setHeader("");
		m_numberPad.setMinValue(SPEED_FACTOR_MIN);
		m_numberPad.setConfirmCallback(
			[this, &slider](float value)
			{
				slider.setValue(value); // This might be a bug if the slider is destroyed while the numberpad is open?
				closeModal(this);
			});
	}

	void ModalSpeedFactor::onShow()
	{
		Modal::onShow();

		auto& slider = m_speedFactor.getSlider();
		m_numberPad.setValue(slider.getValue());
	}
} // namespace UI
