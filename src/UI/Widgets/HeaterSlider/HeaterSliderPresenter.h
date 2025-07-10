#pragma once

#include "UI/Core/Presenter.h"
#include "ObjectModel/Heat.h"

namespace UI
{
	class HeaterSlider;

	class HeaterSliderPresenter : public Presenter<HeaterSlider>
	{
	  public:
		PRESENTER_CONSTRUCTOR(HeaterSliderPresenter, HeaterSlider)

		// Actions
		void setHeaterIndex(size_t index);

		// Observers
		void newHeaterData();

	  protected:
		virtual void onInit() override {
			registerEventListener<EventType::HeaterData>(this, &HeaterSliderPresenter::newHeaterData);
		}
		virtual void onActivate() override {}
		virtual void onDeactivate() override {}

	  private:
		size_t m_heaterIndex = 0;
		std::shared_ptr<OM::Heat::Heater> m_heater;
	};
} // namespace UI
