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
				m_numberPad->setHeader(m_speed.getLabel());
				m_numberPad->setValue(m_speed.getValue());
				m_numberPad->setMinValue(m_speed.getMin());
				m_numberPad->setMaxValue(m_speed.getMax());
				m_speed.getInput().sendEvent(LV_EVENT_DEFOCUSED, nullptr);
				m_numberPad->setConfirmCallback([this](float value) { m_speed.setValue(value); });
			});
		m_speed.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_speed.setRange(1, 200);
		m_speed.setDefaultValue(100);
		m_speed.setValueChangedCallback([this](int32_t value) { m_presenter->setSpeedFactor(value); });
	}

	void SpeedFactor::setSpeedValue(uint32_t value)
	{
		m_speed.setValue(value);
	}
} // namespace UI
