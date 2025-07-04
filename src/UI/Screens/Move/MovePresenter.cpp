#include "MovePresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "MoveView.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Tool.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void MovePresenter::onInit()
	{
		MODEL_LOCK();

		registerEventListener<EventType::AxesData>(this, &MovePresenter::newAxesData);
		registerEventListener<EventType::ToolData>(this, &MovePresenter::newToolData);
		registerEventListener<EventType::Disconnected>(this, &MovePresenter::disconnected);
	}

	void MovePresenter::onActivate()
	{
		newAxesData();
		newToolData();

		OM::FileSystem::RequestFiles(OM::Directories::DirectoryType::FILAMENTS,
									 "",
									 [this]()
									 {
										 {
											 MODEL_LOCK();
											 this->m_filamentOptions.clear();
											 for (size_t i = 0; i < OM::FileSystem::GetItemCount(); i++)
											 {
												 std::shared_ptr<OM::FileSystem::FileSystemItem> item =
													 OM::FileSystem::GetItem(i);
												 if (item == nullptr)
												 {
													 continue;
												 }
												 this->m_filamentOptions.push_back(item->GetName());
											 }
										 }
										 this->updateFilamentList();
									 });
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

	void MovePresenter::toggleToolState(size_t index)
	{
		MODEL_LOCK();
		auto tool = OM::GetToolBySlot(index);
		if (tool == nullptr)
		{
			LOG_WARN("Tool {:d} not found", index);
			return;
		}
		tool->ToggleState();
	}

	void MovePresenter::updateFilamentList()
	{
		m_view->setFilamentOptions(m_filamentOptions);
	}

	void MovePresenter::loadFilament(const std::string& filament)
	{
		MODEL_LOCK();
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			return;
		}
		tool->ChangeFilament(filament.c_str());
	}

	void MovePresenter::unloadFilament()
	{
		MODEL_LOCK();
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			return;
		}
		tool->UnloadFilament();
	}

	void MovePresenter::newAxesData()
	{
		size_t axisCount = OM::Move::GetAxisCount(false);
		auto x = OM::Move::GetAxisByLetter('X');
		auto y = OM::Move::GetAxisByLetter('Y');
		auto z = OM::Move::GetAxisByLetter('Z');

		std::vector<OM::Move::AxisPtr> axes = OM::Move::GetAxes(false);
		{
			/*
			The view keeps a pointer to the m_axisData so we need to lock to prevent the view using half complete data
			*/
			UI_LOCK();
			m_axisData.resize(axes.size());
			for (size_t i = 0; i < axes.size(); i++)
			{
				m_axisData[i] = {.letter = axes[i]->letter[0],
								 .homed = axes[i]->homed != 0,
								 .position = axes[i]->userPosition,
								 .min = axes[i]->minPosition,
								 .max = axes[i]->maxPosition};
			}

			m_view->setAxisData(m_axisData);
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

	void MovePresenter::newToolData()
	{
		m_view->setToolCount(OM::GetToolCount());
		auto currentTool = OM::GetCurrentTool();
		if (!currentTool)
		{
			m_view->setCurrentTool(-1);
		}
		for (size_t i = 0; i < OM::GetToolCount(); i++)
		{
			auto tool = OM::GetToolBySlot(i);
			if (!tool)
			{
				LOG_WARN("Tool {:d} not found", i);
				continue;
			}
			m_view->setToolName(
				i, tool->name.IsEmpty() ? fmt::format("{} {}", _("default_tool_name"), i) : tool->name.c_str());
			if (currentTool && currentTool == tool)
			{
				UI_LOCK();
				m_view->setCurrentTool(i);
				m_view->setFilamentDisabled(tool->filamentExtruder < 0);
				m_view->setLoadedFilament(tool->GetFilament().c_str());
			}
		}
	}

	void MovePresenter::disconnected()
	{
		m_view->clear();
	}
} // namespace UI
