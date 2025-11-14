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
		virtual void onInit() override
		{
			registerEventListener<EventType::AxesData>(this, &BabyStepPresenter::newAxesData);
		}
		virtual void onActivate() override { newAxesData(); }
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override {}
	};
} // namespace UI
