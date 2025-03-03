#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FanView;

	class FanPresenter : public Presenter<FanView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		void setFanSpeed(size_t slot, uint32_t value);

		// Observers
		virtual void newFanData() override;

	  private:
	};
} // namespace UI
