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
			bool home_disabled = true;
			bool jog_disabled = true;
		};

		using AxisDataList = std::vector<AxisData>;

		enum class PositionType
		{
			User,
			Machine
		};

		// Actions
		void homeAll();
		void disableMotors();

		// AxisItem actions
		void homeAxis(char axis_letter);
		void homeAxis(size_t axisSlot);
		void moveAxisAbsolute(char axis_letter, float position, uint32_t feedrate);
		void moveAxisRelative(char axis_letter, float distance, uint32_t feedrate);
		void moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate);

		// Observers
		void newAxesData();
		void newStatus(const OM::PrinterStatus& status);

	  protected:
		void onActivate() override;
		void onInit() override;
		void onDisconnect() override;

	  private:
		AxisDataList m_axisData;
	};
} // namespace UI
