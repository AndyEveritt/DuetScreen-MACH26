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
		registerEventListener<EventType::ToolData>(this, &MovePresenter::newToolData);
		registerEventListener<EventType::Status>(this, &MovePresenter::newStatus);
	}

	void MovePresenter::onActivate()
	{
		newAxesData();
		newToolData();

		OM::FileSystem::RequestFiles(OM::Directories::DirectoryType::FILAMENTS,
									 "",
									 [this](OM::FileSystem::ItemList files)
									 {
										 {
											 MODEL_LOCK();
											 this->m_filamentOptions.clear();
											 this->m_filamentOptions.reserve(files.size());
											 for (const auto& item : files)
											 {
												 if (!item)
												 {
													 continue;
												 }
												 this->m_filamentOptions.emplace_back(item->GetName());
											 }
										 }
										 this->updateFilamentList();
									 });
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

	void MovePresenter::extrude(float distance, float feedrate)
	{
		OM::Move::Extrude(distance, feedrate);
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

			m_view->setAxisData(m_axisData);
		}
	}

	void MovePresenter::newToolData()
	{
		m_view->setToolCount(OM::GetToolCount());
		auto currentTool = OM::GetCurrentTool();
		if (!currentTool)
		{
			m_view->setCurrentTool(-1);
			m_view->setExtrudeDisabled(true);
			m_view->setRetractDisabled(true);
		}
		for (size_t i = 0; i < OM::GetToolCount(); i++)
		{
			auto tool = OM::GetToolBySlot(i);
			if (!tool)
			{
				LOG_WARN("Tool {:d} not found", i);
				continue;
			}
			m_view->setToolName(i, tool->GetName());
			if (currentTool && currentTool == tool)
			{
				UI_LOCK();
				m_view->setCurrentTool(i);
				m_view->setFilamentDisabled(tool->filamentExtruder < 0);
				m_view->setLoadedFilament(tool->GetFilament().c_str());

				bool canExtrude = true;
				bool canRetract = true;

				const float coldExtrudeTemp = OM::Heat::GetColdExtrudeTemperature();
				const float coldRetractTemp = OM::Heat::GetColdRetractTemperature();
				tool->IterateHeaters(
					[this, &canExtrude, &canRetract, &coldExtrudeTemp, &coldRetractTemp](OM::ToolHeaterPtr heater,
																						 size_t /* index */)
					{
						if (!heater || !heater->heater)
						{
							return;
						}

						const float temperature = heater->heater->current;

						if (temperature < coldExtrudeTemp)
						{
							canExtrude = false;
						}

						if (temperature < coldRetractTemp)
						{
							canRetract = false;
						}
					});

				m_view->setExtrudeDisabled(!canExtrude);
				m_view->setRetractDisabled(!canRetract);
			}
		}
	}

	void MovePresenter::newStatus(const OM::PrinterStatus& /* status */)
	{
		newAxesData();
	}

	void MovePresenter::onDisconnect()
	{
		m_view->clear();
	}
} // namespace UI
