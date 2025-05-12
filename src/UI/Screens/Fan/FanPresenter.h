#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class FanView;

	class FanPresenter : public Presenter<FanView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(FanPresenter, FanView)

		// Actions
		void setFanSpeed(size_t slot, uint32_t value);

		// Observers
		void newFanData();
		void disconnected();

	  private:
		virtual void onInit() override
		{
			registerEventListener<EventType::FanData>(this, &FanPresenter::newFanData);
			registerEventListener<EventType::Disconnected>(this, &FanPresenter::disconnected);
		}
	};
} // namespace UI
