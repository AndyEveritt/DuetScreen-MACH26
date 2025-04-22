#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ExtrudeView;

	class ExtrudePresenter : public Presenter<ExtrudeView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(ExtrudePresenter, ExtrudeView)

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
		void newToolData() override;

	  protected:
		void onActivate() override;

	  private:
		static void numberPadConfirmCallback(lv_event_t* e);

		std::vector<std::string> m_filamentOptions;
		struct NumberPadData
		{
			size_t toolIndex;
			size_t heaterIndex;
			bool active;
		} m_numberPadData;
	};
} // namespace UI
