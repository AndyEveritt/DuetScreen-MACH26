/*
 * SpeedFactorPresenter.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "SpeedFactorPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "SpeedFactor.h"

namespace UI
{
	void SpeedFactorPresenter::newSpeedFactor()
	{
		m_view->setSpeedValue(std::round(100 * OM::Move::GetSpeedFactor()));
	}

	void SpeedFactorPresenter::setSpeedFactor(uint32_t value)
	{
		if (value == std::round(100 * OM::Move::GetSpeedFactor()))
		{
			return;
		}
		Comm::DUET.SendGcodef("M220 S{:d}\n", value);
	}
} // namespace UI
