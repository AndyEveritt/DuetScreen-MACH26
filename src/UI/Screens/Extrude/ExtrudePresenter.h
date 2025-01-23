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

		// ExtrudeItem actions

		// Observers
		void newToolData() override;

	  protected:
		void onActivate() override { newToolData(); }
	};
} // namespace UI
