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
		void openSpeedFactorModal();
		void openExtrusionFactorModal();

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
		virtual void onInit() override;
		virtual void onActivate() override;
		virtual void onDeactivate() override {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}
	};
} // namespace UI
