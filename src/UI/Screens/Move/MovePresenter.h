#pragma once

#include "UI/Core/Presenter.h"

namespace UI
{
	class MoveView;

	class MovePresenter : public Presenter<MoveView>
	{
	  public:
		PRESENTER_CONSTRUCTOR(MovePresenter, MoveView)

		struct AxisData
		{
			char letter;
			bool homed = false;
			float position = 0.0f;
			float min = std::numeric_limits<float>::min();
			float max = std::numeric_limits<float>::max();
		};

		using AxisDataList = std::vector<AxisData>;

		enum class PositionType
		{
			User,
			Machine
		};

		// Actions
		void homeAll();
		void meshBedLevel();
		void trueBedLevel();
		void heightmap();
		void disableMotors();

		// AxisItem actions
		void homeAxis(char axis_letter);
		void homeAxis(size_t axisSlot);
		void moveAxisAbsolute(char axis_letter, float position, uint32_t feedrate);
		void moveAxisRelative(char axis_letter, float distance, uint32_t feedrate);
		void moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate);

		// Observers
		void newAxesData();
		void disconnected();

	  protected:
		void onActivate() override { newAxesData(); }

		virtual void onInit() override;

	  private:
		AxisDataList m_axisData;
	};
} // namespace UI
