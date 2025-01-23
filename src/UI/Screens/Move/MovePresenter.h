#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class MoveView;

	class MovePresenter : public Presenter<MoveView>
	{
	  public:
		using Presenter::Presenter;

		// Actions
		void homeAll();
		void meshBedLevel();
		void trueBedLevel();
		void heightmap();
		void disableMotors();

		// AxisItem actions
		void homeAxis(size_t axisSlot);
		void moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate);

		// Observers
		void newAxesData() override;

	  protected:
		void onActivate() override { newAxesData(); }
	};
} // namespace UI
