#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class MoveView;

	class MovePresenter : public Presenter<MoveView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(MovePresenter, MoveView)

		// Actions
		void homeAll();
		void meshBedLevel();
		void trueBedLevel();
		void heightmap();
		void disableMotors();

		// AxisItem actions
		void homeAxis(char axis_letter);
		void homeAxis(size_t axisSlot);
		void moveAxisRelative(char axis_letter, float distance, uint32_t feedrate);
		void moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate);

		// Observers
		void newAxesData();
		void disconnected();

	  protected:
		void onActivate() override { newAxesData(); }

		virtual void onInit() override;

	  private:
		std::vector<char> m_axisLetters;
	};
} // namespace UI
