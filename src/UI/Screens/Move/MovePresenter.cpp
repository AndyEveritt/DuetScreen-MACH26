#include "MovePresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "MoveView.h"
#include "ObjectModel/Axis.h"

namespace UI
{
	void MovePresenter::homeAll()
	{
		Comm::DUET.SendGcode("G28\n");
	}

	void MovePresenter::trueBedLevel()
	{
		Comm::DUET.SendGcode("G32\n");
	}

	void MovePresenter::meshBedLevel()
	{
		Comm::DUET.SendGcode("G29\n");
	}

	void MovePresenter::heightmap()
	{
		// TODO Open HeightmapView
	}

	void MovePresenter::disableMotors()
	{
		Comm::DUET.SendGcode("M18\n");
	}

	void MovePresenter::homeAxis(size_t axisSlot)
	{
		MODEL_LOCK();
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(axisSlot);
		if (axis == nullptr)
		{
			warn("Axis %u not found", axisSlot);
			return;
		}
		axis->Home();
	}

	void MovePresenter::moveAxisRelative(size_t axisSlot, float distance, uint32_t feedrate)
	{
		MODEL_LOCK();
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(axisSlot);
		if (axis == nullptr)
		{
			warn("Axis %u not found", axisSlot);
			return;
		}
		axis->MoveRelative(distance, feedrate);
	}

	void MovePresenter::newAxesData()
	{
		MODEL_LOCK();
		size_t axisCount = OM::Move::GetAxisCount(false);
		m_view->setAxisCount(axisCount);
		for (size_t i = 0; i < axisCount; i++)
		{
			OM::Move::Axis* axis = OM::Move::GetAxis(i);
			if (axis == nullptr)
			{
				warn("Axis %u not found", i);
				continue;
			}
			std::shared_ptr<AxisItem> item = m_view->getAxisItem(i);
			if (item == nullptr)
			{
				warn("AxisItem %u not found", i);
				continue;
			}
			item->setAxisLetter(axis->letter);
			item->setHomed(axis->homed);
			item->setToolPosition(axis->userPosition);
			item->setMachinePosition(axis->machinePosition);
		}
	}
} // namespace UI
