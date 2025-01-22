#include "MovePresenter.h"
#include "Debug.h"
#include "MoveView.h"
#include "ObjectModel/Axis.h"

namespace UI
{
	void MovePresenter::homeAxis(size_t axisSlot)
	{
		Model::lock();
		OM::Move::Axis* axis = OM::Move::GetAxisBySlot(axisSlot);
		if (axis == nullptr)
		{
			warn("Axis %u not found", axisSlot);
			return;
		}
		axis->Home();
	}

	void MovePresenter::newAxesData()
	{
		Model::lock();
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
