/*
 * PrintInfoPresenter.h
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class PrintInfo;

	class PrintInfoPresenter : public Presenter<PrintInfo>
	{
	  public:
		PRESENTER_CONSTRUCTOR(PrintInfoPresenter, PrintInfo);

		// Setters

		// Getters

		// Actions

		// Observers
		void newAxesData();
		void newExtruderData();
		void newSpeedFactor();
		void newJobDuration();
		void newJobTimeLeft();
		void newCurrentMoveRequestedSpeed();
		void newCurrentMoveTopSpeed();
		void newCurrentMoveExtrusionSpeed();

	  protected:
		void onInit() override;
		void onActivate() override;
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}
	};
} // namespace UI
