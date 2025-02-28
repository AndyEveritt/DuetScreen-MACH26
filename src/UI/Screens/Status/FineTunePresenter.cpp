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
#include "lv_i18n/lv_i18n.h"
#include <cmath>
#include <ranges>

namespace UI
{
	static float s_babyStepValues[] = {0.01f, 0.05f};
	static float s_speedValues[] = {5.0f, 25.0f};
	static float s_flowValues[] = {1.0f, 2.0f};

	void FineTunePresenter::newSpeedFactor()
	{
		ModelLock lock;
		m_view->setSpeedValue(std::round(100 * OM::Move::GetSpeedFactor()));
	}

	void FineTunePresenter::newExtruderData()
	{
		ModelLock lock;

		m_view->setExtruderCount(OM::Move::GetExtruderAxisCount());

		OM::Move::IterateExtruderAxesWhile(
			[this](OM::Move::ExtruderAxis* extruder, size_t index)
			{
				m_view->setExtruderLabel(index, utils::format(_("fine_tune_extruder"), extruder->index).c_str());
				m_view->setExtruderValue(index, std::round(100 * extruder->factor));
				return true;
			});
	}

	void FineTunePresenter::newFanData()
	{
		ModelLock lock;

		m_view->setFanCount(OM::GetFanCount());

		OM::IterateFansWhile(
			[this](OM::Fan* fan, size_t index)
			{
				m_view->setFanLabel(index, utils::format(_("fine_tune_fan"), fan->index).c_str());
				m_view->setFanValue(index, std::round(100 * fan->requestedValue));
				return true;
			});
	}

	void FineTunePresenter::onActivate()
	{
		newSpeedFactor();
		newExtruderData();
		newFanData();
	}

	void FineTunePresenter::babyStep(float change)
	{
		Comm::DUET.SendGcodef("M290 S%.3f", change);
	}

	void FineTunePresenter::setSpeedFactor(uint32_t value)
	{
		if (value == std::round(100 * OM::Move::GetSpeedFactor()))
		{
			return;
		}
		Comm::DUET.SendGcodef("M220 S%d", value);
	}

	void FineTunePresenter::setExtruderFactor(size_t slot, uint32_t value)
	{
		OM::Move::ExtruderAxis* extruder = OM::Move::GetExtruderAxisBySlot(slot);
		if (extruder == nullptr || value == std::round(100 * extruder->factor))
		{
			return;
		}

		Comm::DUET.SendGcodef("M221 D%u S%u", extruder->index, value);
	}

	void FineTunePresenter::setFanValue(size_t slot, uint32_t value)
	{
		OM::Fan* fan = OM::GetFanBySlot(slot);
		if (fan == nullptr || value == std::round(100 * fan->requestedValue))
		{
			return;
		}

		Comm::DUET.SendGcodef("M106 P%u S%u", fan->index, (uint32_t)std::round(2.55 * value));
	}
} // namespace UI
