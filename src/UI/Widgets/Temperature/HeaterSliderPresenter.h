#pragma once

#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/Tool.h"
#include "UI/Core/Presenter.h"

namespace UI
{
	class HeaterSlider;

	class HeaterSliderPresenter : public Presenter<HeaterSlider>
	{
	  public:
		PRESENTER_CONSTRUCTOR(HeaterSliderPresenter, HeaterSlider)
		enum class SlotType
		{
			Tool,
			Bed,
			Chamber,
			Unknown
		};

		using heater_state_t = OM::Heat::HeaterStatus;

		// Setters
		void reset();
		void setToolHeaterIndex(size_t toolIndex, size_t toolHeaterIndex);
		void setBedIndex(size_t index);
		void setChamberIndex(size_t index);

		// Getters
		SlotType getSlotType() const { return m_slotType; }

		// Actions
		void cycleHeaterState();
		void sendTemperature(float value, bool active);

		// Observers
		void newHeaterData();

	  protected:
		void onInit() override
		{
			registerEventListener<EventType::HeaterData>(this, &HeaterSliderPresenter::newHeaterData);
		}
		void onActivate() override;
		void onDeactivate() override {}
		void onDisconnect() override { reset(); }

	  private:
		SlotType m_slotType = SlotType::Unknown;
		size_t m_slot = 0;
		OM::ToolPtr m_tool;
		OM::ToolHeaterPtr m_tHeater;
		OM::BedOrChamberPtr m_bedOrChamber;
	};
} // namespace UI
