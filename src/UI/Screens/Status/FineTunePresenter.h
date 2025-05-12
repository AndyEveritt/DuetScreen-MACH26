/*
 * FineTunePresenter.h
 *
 *  Created on: 2025-02-27
 *      Author: Andy Everitt
 */

#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FineTune;

	class FineTunePresenter : public Presenter<FineTune>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FineTunePresenter, FineTune)

		void newSpeedFactor();
		void newExtruderData();
		void newFanData();
		void newAxesData();

		// Actions
		void babystep(float change);
		void resetBabystep();
		void setSpeedFactor(uint32_t value);
		void setExtruderFactor(size_t slot, uint32_t value);
		void setFanValue(size_t slot, uint32_t value);

	  protected:
		virtual void onActivate() override;

		virtual void onInit() override
		{
			registerEventListener<EventType::SpeedFactor>(this, &FineTunePresenter::newSpeedFactor);
			registerEventListener<EventType::ExtruderData>(this, &FineTunePresenter::newExtruderData);
			registerEventListener<EventType::FanData>(this, &FineTunePresenter::newFanData);
			registerEventListener<EventType::AxesData>(this, &FineTunePresenter::newAxesData);
		}

	  private:
	};
} // namespace UI
