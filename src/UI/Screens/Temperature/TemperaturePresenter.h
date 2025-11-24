#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class TemperatureView;

	class TemperaturePresenter : public Presenter<TemperatureView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(TemperaturePresenter, TemperatureView)

		// Actions

		// Observers
		void newToolData();
		void newBedHeaterData();
		void newChamberHeaterData();

	  protected:
		void onActivate() override;
		void onDeactivate() override;
		void onDisconnect() override;

		void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &TemperaturePresenter::newToolData);
			registerEventListener<EventType::BedHeaterData>(this, &TemperaturePresenter::newBedHeaterData);
			registerEventListener<EventType::ChamberHeaterData>(this, &TemperaturePresenter::newChamberHeaterData);
		}

	  private:
		void numberPadConfirmCallback(float value);

		std::vector<std::string> m_filamentOptions;
		struct NumberPadData
		{
			size_t toolIndex;
			size_t heaterIndex;
			bool active;
		} m_numberPadData;
	};
} // namespace UI
