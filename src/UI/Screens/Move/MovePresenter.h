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
		void homeAxis(size_t axisSlot);
		void moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate);

		// Observers
		void newAxesData();
		void disconnected();

	  protected:
		void onActivate() override { newAxesData(); }

		virtual void onInit() override
		{
			registerEventListener<EventType::AxesData>(this, &MovePresenter::newAxesData);
			registerEventListener<EventType::Disconnected>(this, &MovePresenter::disconnected);
		}

	  private:
	};
} // namespace UI
