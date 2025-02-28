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
  		using Presenter::Presenter;

        virtual void newSpeedFactor() override;
        virtual void newExtruderData() override;
		virtual void newFanData() override;

		// Actions
		void babyStep(float change);
		void setSpeedFactor(uint32_t value);
		void setExtruderFactor(size_t slot, uint32_t value);
		void setFanValue(size_t slot, uint32_t value);

	  protected:
		virtual void onActivate() override;
	};
} // namespace UI
