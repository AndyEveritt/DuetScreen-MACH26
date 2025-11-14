/*
 * FineTunePresenter.cpp
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#include "FineTunePresenter.h"
#include "Debug.h"
#include "FineTune.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Fan.h"
#include "ObjectModel/Tool.h"
#include "i18n/i18n.h"
#include <cmath>
#include <ranges>

namespace UI
{
	void FineTunePresenter::newSpeedFactor()
	{
		m_view->setSpeedValue(static_cast<uint32_t>(std::round(100 * OM::Move::GetSpeedFactor())));
	}

	void FineTunePresenter::newExtruderData()
	{
		m_view->setExtruderCount(OM::Move::GetExtruderAxisCount());

		OM::Move::IterateExtruderAxesWhile(
			[this](std::shared_ptr<OM::Move::ExtruderAxis> extruder, size_t index)
			{
				m_view->setExtruderLabel(index, _("fine_tune.extruder", extruder->index));
				m_view->setExtruderValue(index, static_cast<uint32_t>(std::round(100 * extruder->factor)));
				return true;
			});
	}

	void FineTunePresenter::newFanData()
	{
		m_view->setFanCount(OM::GetFanCount());

		OM::IterateFansWhile(
			[this](std::shared_ptr<OM::Fan> fan, size_t index)
			{
				m_view->setFanLabel(index, _("fine_tune.fan", fan->index));
				m_view->setFanValue(index, static_cast<uint32_t>(std::round(100 * fan->requestedValue)));
				return true;
			});
	}

	void FineTunePresenter::onActivate()
	{
		newSpeedFactor();
		newExtruderData();
		newFanData();
	}

	void FineTunePresenter::setSpeedFactor(uint32_t value)
	{
		if (value == std::round(100 * OM::Move::GetSpeedFactor()))
		{
			return;
		}
		Comm::DUET.SendGcodef("M220 S{:d}\n", value);
	}

	void FineTunePresenter::setExtruderFactor(size_t slot, uint32_t value)
	{
		auto extruder = OM::Move::GetExtruderAxisBySlot(slot);
		if (extruder == nullptr || value == std::round(100 * extruder->factor))
		{
			return;
		}

		LOG_DBG("Setting extruder {:d} slider factor to {:d}", extruder->index, value);
		Comm::DUET.SendGcodef("M221 D{:d} S{:d}\n", extruder->index, value);
	}

	void FineTunePresenter::setFanValue(size_t slot, uint32_t value)
	{
		auto fan = OM::GetFanBySlot(slot);
		if (fan == nullptr || value == std::round(100 * fan->requestedValue))
		{
			return;
		}

		Comm::DUET.SendGcodef("M106 P{:d} S{:d}\n", fan->index, (uint32_t)std::round(2.55 * value));
	}
} // namespace UI
