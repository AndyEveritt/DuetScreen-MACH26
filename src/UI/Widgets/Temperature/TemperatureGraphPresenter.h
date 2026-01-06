/*
 * TemperatureGraphPresenter.h
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class TemperatureGraph;

	class TemperatureGraphPresenter : public Presenter<TemperatureGraph>
	{
	  public:
		PRESENTER_CONSTRUCTOR(TemperatureGraphPresenter, TemperatureGraph);

		// Setters

		// Getters

		// Actions

		// Observers
		void tick();

	  protected:
		void onInit() override { registerEventListener<EventType::Tick>(this, &TemperatureGraphPresenter::tick); }
		void onActivate() override {}
		void onDeactivate() override {}

		void onConnect() override {}
		void onDisconnect() override {}
	};
} // namespace UI
