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
		void updateFilamentList();

		void toggleToolState(size_t index);
		void toggleHeaterState(size_t toolIndex, size_t heaterIndex);
		void loadFilament(size_t index, const char* filament);
		void unloadFilament(size_t index);
		bool configureNumberPad(const size_t toolIndex, const size_t heaterIndex, const bool active);

		// ExtrudeItem actions

		// Observers
		void newToolData();
		void newBedHeaterData();
		void newChamberHeaterData();

	  protected:
		void onActivate() override;
		void onDeactivate() override;
		void onDisconnect() override;

		virtual void onInit() override
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
