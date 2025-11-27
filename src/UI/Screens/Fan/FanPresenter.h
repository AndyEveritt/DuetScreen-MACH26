#pragma once

#include "ObjectModel/Fan.h"
#include "UI/Core/Presenter.h"
#include <vector>

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

	  private:
		void onInit() override { registerEventListener<EventType::FanData>(this, &FanPresenter::newFanData); }
		void onActivate() override;
		void onDisconnect() override;

		std::vector<OM::FanPtr> m_controllableFans;
	};
} // namespace UI
