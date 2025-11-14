/*
 * SpeedFactorPresenter.h
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class SpeedFactor;

	class SpeedFactorPresenter : public Presenter<SpeedFactor>
	{
	  public:
		PRESENTER_CONSTRUCTOR(SpeedFactorPresenter, SpeedFactor);

		// Setters

		// Getters

		// Actions
		void setSpeedFactor(uint32_t value);

		// Observers
		void newSpeedFactor();

	  protected:
		virtual void onInit() override
		{
			registerEventListener<EventType::SpeedFactor>(this, &SpeedFactorPresenter::newSpeedFactor);
		}
		virtual void onActivate() override { newSpeedFactor(); }
		virtual void onDeactivate() override {}

		virtual void onConnect() override {}
		virtual void onDisconnect() override {}
	};
} // namespace UI
