/*
 * BabyStepPresenter.cpp
 *
 *  Created on: 2025-09-25
 *      Author: Andy Everitt
 */

#include "BabyStepPresenter.h"
#include "BabyStep.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "utils/UnitSystem.h"

namespace UI
{
	void BabyStepPresenter::babystep(float change)
	{
		Comm::DUET.SendGcodef("M290 S{:g}\n", change);
	}

	void BabyStepPresenter::resetBabystep()
	{
		Comm::DUET.SendGcode("M290 R0 S0\n");
	}

	void BabyStepPresenter::newAxesData()
	{
		auto axis = OM::Move::GetAxisByLetter('Z');
		if (axis == nullptr)
		{
			return;
		}

		m_view->setBabyStepValue(axis->babystep);
	}

} // namespace UI
