/*
 * BabyStepPresenter.h
 *
 *  Created on: 2025-09-25
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class BabyStep;

	class BabyStepPresenter : public Presenter<BabyStep>
	{
	  public:
		PRESENTER_CONSTRUCTOR(BabyStepPresenter, BabyStep);

		// Setters

		// Getters

		// Actions
		void babystep(float change);
		void resetBabystep();

		// Observers
		void newAxesData();

	  protected:
		void onInit() override { registerEventListener<EventType::AxesData>(this, &BabyStepPresenter::newAxesData); }
		void onActivate() override { newAxesData(); }
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}
	};
} // namespace UI
