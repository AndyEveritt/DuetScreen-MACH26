#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class ExtrudeView;

	class ExtrudePresenter : public Presenter<ExtrudeView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		void retract(uint32_t distance, uint32_t feedrate);
		void extrude(uint32_t distance, uint32_t feedrate);
		void updateFilamentList();

		void toggleToolState(size_t index);
		void toggleHeaterState(size_t toolIndex, size_t heaterIndex);
		void loadFilament(size_t index, const char* filament);
		void unloadFilament(size_t index);

		// ExtrudeItem actions

		// Observers
		void newToolData() override;

	  protected:
		void onActivate() override;

	  private:
		std::vector<std::string> m_filamentOptions;
	};
} // namespace UI
