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
		virtual void onInit() override {
            registerEventListener<EventType::Tick>(this, &TemperatureGraphPresenter::tick);
        }
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}

		virtual void onConnect() {}
		virtual void onDisconnect() {}
	};
} // namespace UI
