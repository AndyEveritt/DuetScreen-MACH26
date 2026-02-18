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
#include "ObjectModel/PrinterStatus.h"
#include "utils/UnitSystem.h"

namespace UI
{
	void BabyStepPresenter::babystep(float change)
	{
		ZoneScoped;
		Comm::DUET.SendGcodef("M290 S{:g}\n", change);
	}

	void BabyStepPresenter::resetBabystep()
	{
		ZoneScoped;
		Comm::DUET.SendGcode("M290 R0 S0\n");
	}

	void BabyStepPresenter::newAxesData()
	{
		ZoneScoped;
		auto axis = OM::Move::GetAxisByLetter('Z');
		const auto val = axis ? axis->babystep : 0.0f;
		getView()->setDisabled(!axis || !OM::IsConnected());
		getView()->setBabyStepValue(val);
	}

	void BabyStepPresenter::onDisconnect()
	{
		ZoneScoped;
		getView()->setDisabled(true);
	}

} // namespace UI
