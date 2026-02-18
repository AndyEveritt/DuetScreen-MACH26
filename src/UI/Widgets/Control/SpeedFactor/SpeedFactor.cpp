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
		ZoneScoped;
		UI_LOCK();

		m_speed.setSize(LV_PCT(100), LV_PCT(100));
		m_speed.setMinHeight(LV_SIZE_CONTENT);
		m_speed.setTitle(_("fine_tune.speed_factor"));

		m_speed.setItemCount(
			1,
			[&](size_t index, LvObj& parent)
			{
				auto slider = std::make_unique<Slider>(fmt::format("{:d}", index), parent);
				slider->setSize(LV_PCT(100), LV_SIZE_CONTENT);
				slider->setFocusedCallback(
					[this, slider = slider.get()](bool focused)
					{
						if (!focused || !m_numberPad)
							return;
						m_numberPad->setHeader(slider->getLabelText());
						m_numberPad->setValue(slider->getValue());
						m_numberPad->setMinValue(slider->getMin());
						m_numberPad->setMaxValue(slider->getMax());
						slider->getInput().sendEvent(LV_EVENT_DEFOCUSED, nullptr);
						m_numberPad->setConfirmCallback([this, slider](float value) { slider->setValue(value); });
					});
				slider->setOutOfRangeMode(Slider::OutOfRange::UPPER);
				slider->setRange(SPEED_FACTOR_MIN, 200);
				slider->setDefaultValue(100);
				slider->setValueChangedCallback(
					[this, slider = slider.get()](float value)
					{
						slider->setSendMode(Slider::SendMode::DISABLED); // prevent callback loop
						slider->setRange(std::max(SPEED_FACTOR_MIN, value - 50), std::max(150.0f, value + 50));
						slider->setOutOfRangeMode(slider->getMin() <= SPEED_FACTOR_MIN ? Slider::OutOfRange::UPPER
																					   : Slider::OutOfRange::BOTH);
						slider->setSendMode(Slider::SendMode::VALUE_CONFIRMED);

						m_presenter->setSpeedFactor(static_cast<uint32_t>(value));
					});
				slider->addEventCallback(
					[this, slider = slider.get()](lv_event_t*)
					{
						if (m_numberPad)
						{
							m_numberPad->setValue(slider->getValue());
						}
					},
					LV_EVENT_VALUE_CHANGED);
				return slider;
			});
	}

	void SpeedFactor::setSpeedValue(float value)
	{
		ZoneScoped;
		getSlider().setValue(value);
	}

	ModalSpeedFactor::ModalSpeedFactor(const std::string& name, LvObj& parent)
		: Modal(name, parent)
	{
		ZoneScoped;
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_speedFactor.setHeight(LV_PCT(100));
		m_speedFactor.setMinHeight(LV_SIZE_CONTENT);
		m_speedFactor.setFlexGrow(1);
		m_speedFactor.setNumberPad(&m_numberPad);
		m_speedFactor.getSlider().setFocusedCallback(
			[this](bool focused)
			{
				if (!focused)
					return;

				m_numberPad.setValue(m_speedFactor.getSlider().getValue());
				m_numberPad.setMinValue(m_speedFactor.getSlider().getMin());
				m_numberPad.setMaxValue(m_speedFactor.getSlider().getMax());
				m_speedFactor.getSlider().getInput().sendEvent(LV_EVENT_DEFOCUSED, nullptr);
				m_numberPad.setConfirmCallback(
					[this](float value)
					{
						m_speedFactor.getSlider().setValue(
							value); // This might be a bug if the slider is destroyed while the numberpad is open?
						closeModal(this);
					});
				m_numberPad.show();
			});
		// m_speedFactor.getSlider().getInput().hide();

		m_numberPad.setSize(LV_PCT(50), 400);
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
		ZoneScoped;
		Modal::onShow();

		auto& slider = m_speedFactor.getSlider();
		m_numberPad.setValue(slider.getValue());
		m_numberPad.hide();
	}
} // namespace UI
