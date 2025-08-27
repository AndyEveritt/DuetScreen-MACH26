/*
 * EStopPresenter.h
 *
 *  Created on: 2025-08-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class EStop;

	class EStopPresenter : public Presenter<EStop>
	{
	  public:
		PRESENTER_CONSTRUCTOR(EStopPresenter, EStop);

		// Setters

		// Getters

		// Actions
		void eStop();

		// Observers

	  protected:
		virtual void onInit() override {}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}
	};
} // namespace UI
