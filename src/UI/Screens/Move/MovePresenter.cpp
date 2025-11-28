#include "MovePresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "MoveView.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "UI/Core/Navigation.h"
#include "UI/Screens/Home/HomeView.h"
#include "i18n/i18n.h"

namespace UI
{
	static bool canHome()
	{
		switch (OM::GetStatus())
		{
		case OM::PrinterStatus::connecting:
		case OM::PrinterStatus::pausing:
		case OM::PrinterStatus::resuming:
		case OM::PrinterStatus::processing:
		case OM::PrinterStatus::off:
			return false;
		default:
			return true;
		}
	}

	static bool canMove(const OM::Move::AxisPtr& axis)
	{
		return (axis->homed || !OM::Move::GetNoMovesBeforeHoming()) && canHome();
	}

	void MovePresenter::onInit()
	{
		MODEL_LOCK();

		registerEventListener<EventType::AxesData>(this, &MovePresenter::newAxesData);
		registerEventListener<EventType::Status>(this, &MovePresenter::newStatus);
	}

	void MovePresenter::onActivate()
	{
		getView()->setDisabled(!OM::IsConnected());
		newAxesData();
	}

	void MovePresenter::homeAll()
	{
		Comm::DUET.SendGcode("G28\n");
	}

	void MovePresenter::disableMotors()
	{
		Comm::DUET.SendGcode("M18\n");
	}

	void MovePresenter::homeAxis(char axis_letter)
	{
		MODEL_LOCK();
		auto axis = OM::Move::GetAxisByLetter(axis_letter);
		if (axis == nullptr)
		{
			LOG_WARN("Axis '{}' not found", axis_letter);
			return;
		}
		axis->Home();
	}

	void MovePresenter::homeAxis(size_t axisSlot)
	{
		MODEL_LOCK();
		auto axis = OM::Move::GetAxisBySlot(axisSlot);
		if (axis == nullptr)
		{
			LOG_WARN("Axis {:d} not found", axisSlot);
			return;
		}
		axis->Home();
	}

	void MovePresenter::moveAxisAbsolute(char axis_letter, float position, uint32_t feedrate)
	{
		MODEL_LOCK();
		auto axis = OM::Move::GetAxisByLetter(axis_letter);
		if (axis == nullptr)
		{
			LOG_WARN("Axis '{}' not found", axis_letter);
			return;
		}
		axis->MoveAbsolute(position, feedrate);
	}

	void MovePresenter::moveAxisRelative(char axis_letter, float distance, uint32_t feedrate)
	{
		MODEL_LOCK();
		auto axis = OM::Move::GetAxisByLetter(axis_letter);
		if (axis == nullptr)
		{
			LOG_WARN("Axis '{}' not found", axis_letter);
			return;
		}
		axis->MoveRelative(distance, feedrate);
	}

	void MovePresenter::moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate)
	{
		MODEL_LOCK();
		auto axis = OM::Move::GetAxisBySlot(axisSlot);
		if (axis == nullptr)
		{
			LOG_WARN("Axis {:d} not found", axisSlot);
			return;
		}
		axis->MoveRelative(distance, feedrate);
	}

	void MovePresenter::newAxesData()
	{
		std::vector<OM::Move::AxisPtr> axes = OM::Move::GetAxes(false);
		{
			/*
			The view keeps a pointer to the m_axisData so we need to lock to prevent the view using half complete data
			*/
			UI_LOCK();
			m_axisData.resize(axes.size());
			for (size_t i = 0; i < axes.size(); i++)
			{
				auto& axis = axes[i];
				m_axisData[i] = {.letter = axis->letter[0],
								 .homed = axis->homed != 0,
								 .position = axis->userPosition,
								 .min = axis->minPosition,
								 .max = axis->maxPosition,
								 .home_disabled = !canHome(),
								 .jog_disabled = !canMove(axis)};
			}

			getView()->setAxisData(m_axisData);
			getView()->setHomeAllDisabled(!canHome());
			getView()->setDisableMotorsDisabled(!canHome());
		}
	}

	void MovePresenter::newStatus(const OM::PrinterStatus& /* status */)
	{
		getView()->setDisabled(!OM::IsConnected());
		newAxesData();
	}

	void MovePresenter::onConnect()
	{
		getView()->setDisabled(false);
	}

	void MovePresenter::onDisconnect()
	{
		getView()->setDisabled(true);
		getView()->clear();
	}
} // namespace UI
