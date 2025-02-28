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

	  protected:
	};
} // namespace UI
