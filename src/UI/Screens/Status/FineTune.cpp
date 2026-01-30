/*
 * FineTune.cpp
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#include "FineTune.h"
#include "Debug.h"
#include "UI/Styles/Styles.h"
#include "i18n/i18n.h"
#include <algorithm>

namespace UI
{
	FineTune::FineTune(const std::string& name, LvObj& parent, LvObj* numberpad_parent)
		: View(name, parent, layout_t(0, 0, 100, 100))
		, m_babystep("babystep", getRoot())
		, m_sliderCont("sliders", getRoot())
		, m_speed("speed", m_sliderCont)
		, m_extruders("extruders", m_sliderCont)
		, m_fans("fans", m_sliderCont)
		, m_numberPad("numberpad", numberpad_parent ? *numberpad_parent : parent)
	{
		ZoneScoped;
		UI_LOCK();

		addStyle(Themes::getLvglStyles().bg_dark);
		m_babystep.addStyle(Themes::getLvglStyles().card);
		m_speed.addStyle(Themes::getLvglStyles().card);
		m_extruders.addStyle(Themes::getLvglStyles().card);
		m_fans.addStyle(Themes::getLvglStyles().card);

		setFlexFlow(LV_FLEX_FLOW_ROW);
		setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		iterateChildren([](size_t /* i */, LvObj& child) { child.setHeight(LV_PCT(100)); });
		m_babystep.setFlexGrow(2);
		m_sliderCont.setFlexGrow(5);

		m_babystep.setMaxWidth(200);

		m_sliderCont.setFlexFlow(LV_FLEX_FLOW_COLUMN);
		m_sliderCont.setFlexAlign(LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		m_speed.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_extruders.setSize(LV_PCT(100), LV_SIZE_CONTENT);
		m_fans.setSize(LV_PCT(100), LV_SIZE_CONTENT);

		m_extruders.setTitle(_("fine_tune.extruder_header"));
		m_fans.setTitle(_("fine_tune.fan_header"));

		m_speed.setLabel(_("fine_tune.speed_factor"));
		m_speed.setFocusedCallback(
			[this](bool focused)
			{
				if (!focused)
					return;
				configureNumberPad(&m_speed);
				showNumberPad();
			});
		m_speed.setOutOfRangeMode(Slider::OutOfRange::UPPER);
		m_speed.setRange(1, 200);
		m_speed.setDefaultValue(100);
		m_speed.setValueChangedCallback([this](float value)
										{ m_presenter->setSpeedFactor(static_cast<uint32_t>(value)); });
	}

	void FineTune::setSpeedValue(uint32_t value)
	{
		ZoneScoped;
		m_speed.setValue(static_cast<float>(value));
	}

	/**
	 * @brief
	 * @param count
	 */
	void FineTune::setExtruderCount(size_t count)
	{
		ZoneScoped;
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
												 showNumberPad();
											 }
										 });
									 slider->setRange(0, 200);
									 slider->setDefaultValue(100);
									 slider->setOutOfRangeMode(Slider::OutOfRange::UPPER);
									 slider->setValueChangedCallback(
										 [this, index](float value)
										 { m_presenter->setExtruderFactor(index, static_cast<uint32_t>(value)); });
									 return slider;
								 });
	}

	void FineTune::setFanCount(size_t count)
	{
		ZoneScoped;
		m_fans.setItemCount(count,
							[this](size_t index, LvObj& parent)
							{
								auto slider = std::make_unique<Slider>(fmt::format("{:d}", index), parent);

								slider->setSize(LV_PCT(100), LV_SIZE_CONTENT);
								slider->setFocusedCallback(
									[this, index](bool focused)
									{
										if (!focused)
											return;

										if (auto slider = m_fans.getItem(index))
										{
											configureNumberPad(slider);
											showNumberPad();
										}
									});
								slider->setRange(0, 100);
								slider->setDefaultValue(100);
								slider->setValueChangedCallback(
									[this, index](float value)
									{ m_presenter->setFanValue(index, static_cast<uint32_t>(value)); });
								return slider;
							});
	}

	void FineTune::setExtruderLabel(size_t index, std::string_view label)
	{
		ZoneScoped;
		UI_LOCK();
		if (auto extruder = m_extruders.getItem(index))
		{
			if (!extruder->isFocused())
			{
				extruder->setLabel(label);
			}
		}
	}

	void FineTune::setExtruderValue(size_t index, uint32_t value)
	{
		ZoneScoped;
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

	void FineTune::setFanLabel(size_t index, std::string_view label)
	{
		ZoneScoped;
		UI_LOCK();
		if (auto fan = m_fans.getItem(index))
		{
			if (!fan->isFocused())
			{
				fan->setLabel(label);
			}
		}
	}

	void FineTune::setFanValue(size_t index, uint32_t value)
	{
		ZoneScoped;
		UI_LOCK();
		if (auto fan = m_fans.getItem(index))
		{
			if (!fan->isFocused())
			{
				fan->setValue(static_cast<float>(value));
			}
		}
	}

	void FineTune::showNumberPad()
	{
		ZoneScoped;
		openModal(&m_numberPad);
	}

	void FineTune::configureNumberPad(Slider* slider)
	{
		ZoneScoped;
		m_numberPad.setHeader(slider->getLabelText());
		m_numberPad.setValue(slider->getValue());
		m_numberPad.setMinValue(slider->getMin());
		m_numberPad.setMaxValue(slider->getMax());
		slider->getInput().sendEvent(LV_EVENT_DEFOCUSED, nullptr);
		m_numberPad.setConfirmCallback(
			[slider](float value)
			{
				slider->setValue(value); // This might be a bug if the slider is destroyed while the numberpad is open?
			});
	}
} // namespace UI
