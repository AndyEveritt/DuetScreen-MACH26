#include "MovePresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "MoveView.h"
#include "ObjectModel/Axis.h"

namespace UI
{
	void MovePresenter::onInit()
	{
		MODEL_LOCK();

		registerEventListener<EventType::AxesData>(this, &MovePresenter::newAxesData);
		registerEventListener<EventType::Disconnected>(this, &MovePresenter::disconnected);
	}

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
		size_t axisCount = OM::Move::GetAxisCount(false);
		auto x = OM::Move::GetAxisByLetter('X');
		auto y = OM::Move::GetAxisByLetter('Y');
		auto z = OM::Move::GetAxisByLetter('Z');

		std::vector<OM::Move::AxisPtr> axes = OM::Move::GetAxes(false);
		m_axisLetters.resize(axes.size());
		for (size_t i = 0; i < axes.size(); i++)
		{
			m_axisLetters[i] = axes[i]->letter[0];
		}

		m_view->setAxisLetters(m_axisLetters);

		for (auto& axis : axes)
		{
			m_view->setAxisPosition(axis->letter[0], axis->userPosition);
			m_view->setAxisHomed(axis->letter[0], axis->homed);
		}

#if 0
		for (auto& axis : {x, y, z})
		{
			if (axis != nullptr && axis->visible)
			{
				axisCount--;
			}
		}
#endif

		if (x == nullptr || !x->visible)
		{
		}

#if 0
		m_view->setAxisCount(axisCount);
		for (size_t i = 0; i < axisCount; i++)
		{
			auto axis = OM::Move::GetAxis(i);
			if (axis == nullptr)
			{
				LOG_WARN("Axis {:d} not found", i);
				continue;
			}
			std::shared_ptr<AxisItem> item = m_view->getAxisItem(i);
			if (item == nullptr)
			{
				LOG_WARN("AxisItem {:d} not found", i);
				continue;
			}
			item->setAxisLetter(axis->letter);
			item->setHomed(axis->homed);
			item->setToolPosition(axis->userPosition);
			item->setMachinePosition(axis->machinePosition);
			item->disableHome(OM::Move::GetKinematics().IsDelta());
		}
#endif
	}

	void MovePresenter::disconnected()
	{
		m_view->clear();
	}
} // namespace UI
