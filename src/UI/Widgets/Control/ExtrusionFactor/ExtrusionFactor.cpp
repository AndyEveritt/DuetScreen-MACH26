/*
 * ExtrusionFactor.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "ExtrusionFactor.h"
#include "Debug.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"

namespace UI
{
	ExtrusionFactor::ExtrusionFactor(const std::string& name, LvObj& parent)
		: View(name, parent)
	{
		m_extruders.setSize(LV_PCT(100), LV_PCT(100));
		m_extruders.setMinHeight(LV_SIZE_CONTENT);

		m_extruders.setTitle(_("fine_tune.extruder_header"));
	}

	/**
	 * @brief Set the number of extruders shown in the extrusion factor list
	 * @param count
	 */
	void ExtrusionFactor::setExtruderCount(size_t count)
	{
		m_extruders.setItemCount(count,
								 [this](size_t index, LvObj& parent)
								 {
									 auto slider = std::make_unique<Slider>(fmt::format("{:d}", index), parent);
									 slider->setSize(LV_PCT(100), LV_SIZE_CONTENT);
									 slider->setFocusedCallback(
										 [this, index](bool focused)
										 {
											 if (!focused)
												 return;

											 if (auto slider = m_extruders.getItem(index))
											 {
												 configureNumberPad(slider);
												 if (m_inputFocusCb)
												 {
													 m_inputFocusCb(true, index, *slider);
												 }
											 }
										 });
									 slider->setRange(0, 200);
									 slider->setDefaultValue(100);
									 slider->setOutOfRangeMode(Slider::OutOfRange::UPPER);
									 slider->setValueChangedCallback(
										 [this, index](float value)
										 { m_presenter->setExtruderFactor(index, static_cast<uint32_t>(value)); });
									 slider->addEventCallback(
										 [this](lv_event_t* e)
										 {
											 float value = *static_cast<float*>(lv_event_get_param(e));
											 if (m_numberPad)
											 {
												 m_numberPad->setValue(value);
											 }
										 },
										 LV_EVENT_VALUE_CHANGED);
									 return slider;
								 });
	}

	void ExtrusionFactor::setExtruderLabel(size_t index, std::string_view label)
	{
		UI_LOCK();
		if (auto extruder = m_extruders.getItem(index))
		{
			if (!extruder->isFocused())
			{
				extruder->setLabel(label);
			}
		}
	}

	void ExtrusionFactor::setExtruderValue(size_t index, uint32_t value)
	{
		UI_LOCK();
		if (auto extruder = m_extruders.getItem(index))
		{
			if (!extruder->isFocused())
			{
				LOG_DBG("Setting extruder {:d} extrusion factor to {:d}", index, value);
				extruder->setValue(static_cast<float>(value));
			}
		}
	}

	void ExtrusionFactor::configureNumberPad(Slider* slider)
	{
		if (!m_numberPad)
		{
			LOG_WARN("Number pad not set");
			return;
		}
		m_numberPad->setHeader(slider->getLabelText());
		m_numberPad->setValue(slider->getValue());
		m_numberPad->setMinValue(slider->getMin());
		// m_numberPad->setMaxValue(slider->getMax());
		slider->getInput().sendEvent(LV_EVENT_DEFOCUSED, nullptr);
		m_numberPad->setConfirmCallback(
			[this, slider](float value)
			{
				slider->setValue(value); // This might be a bug if the slider is destroyed while the numberpad is open?
			});
	}

	ModalExtrusionFactor::ModalExtrusionFactor(const std::string& name, LvObj& parent)
		: Modal(name, parent)
	{
		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		setFlag(LV_OBJ_FLAG_SCROLLABLE, false);

		m_extrusionFactor.setHeight(LV_PCT(100));
		m_extrusionFactor.setFlexGrow(1);
		m_extrusionFactor.setNumberPad(&m_numberPad);
		m_extrusionFactor.setInputFocusCb(
			[this](bool focused, size_t index, Slider& /* slider */)
			{
				if (!focused)
					return;

				m_presenter->configureNumberPad(index);
			});

		m_numberPad.setSize(LV_PCT(50), LV_PCT(100));

		m_numberPad.setCloseOnConfirm(false);
		m_numberPad.addEventCallback([this](lv_event_t*) { closeModal(this); }, LV_EVENT_READY);
	}
} // namespace UI
