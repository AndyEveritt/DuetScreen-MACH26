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
		void retract(uint32_t distance, uint32_t feedrate);
		void extrude(uint32_t distance, uint32_t feedrate);
		void updateFilamentList();

		void toggleToolState(size_t index);
		void toggleHeaterState(size_t toolIndex, size_t heaterIndex);
		void loadFilament(size_t index, const char* filament);
		void unloadFilament(size_t index);
		bool configureNumberPad(const size_t toolIndex, const size_t heaterIndex, const bool active);

		// ExtrudeItem actions

		// Observers
		void newToolData();
		void disconnected();

	  protected:
		void onActivate() override;
		void onDeactivate() override;

		virtual void onInit() override
		{
			registerEventListener<EventType::ToolData>(this, &TemperaturePresenter::newToolData);
			registerEventListener<EventType::Disconnected>(this, &TemperaturePresenter::disconnected);
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
