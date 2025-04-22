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

		virtual void newSpeedFactor() override;
		virtual void newExtruderData() override;
		virtual void newFanData() override;
		virtual void newAxesData() override;

		// Actions
		void babystep(float change);
		void resetBabystep();
		void setSpeedFactor(uint32_t value);
		void setExtruderFactor(size_t slot, uint32_t value);
		void setFanValue(size_t slot, uint32_t value);

	  protected:
		virtual void onActivate() override;
	};
} // namespace UI
