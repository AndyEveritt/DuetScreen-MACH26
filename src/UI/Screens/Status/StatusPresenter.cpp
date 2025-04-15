#include "StatusPresenter.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/BedOrChamber.h"
#include "ObjectModel/Fan.h"
#include "ObjectModel/Files.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/PrinterStatus.h"
#include "ObjectModel/Tool.h"
#include "StatusView.h"
#include "UI/Core/Navigation.h"

namespace UI
{
	void StatusPresenter::pausePrint()
	{
		OM::FileSystem::PausePrint(); // Pause print
	}

	void StatusPresenter::resumePrint()
	{
		OM::FileSystem::ResumePrint(); // Resume print
	}

	void StatusPresenter::printAgain()
	{
		OM::FileSystem::PrintAgain(); // Print again
	}

	void StatusPresenter::cancelPrint()
	{
		OM::FileSystem::StopPrint(); // Stop print and turn off heaters
		closeScreen(m_view);
	}

	void StatusPresenter::onActivate()
	{
		newJobFileName(OM::GetJobName().c_str());
		newJobLastFileName(OM::GetLastJobName().c_str());
		newJobPrintTime();
		newJobDuration();
		newJobTimeLeft();
		newCurrentMoveRequestedSpeed();
		newCurrentMoveTopSpeed();
		newCurrentMoveExtrusionSpeed();
		newAxesData();
		newExtruderData();
		newSpeedFactor();
		newToolData();
		newFanData();
		newStatus(OM::GetStatus());
	}

	void StatusPresenter::onDeactivate() {}

	void StatusPresenter::newJobFileName(const char* filename)
	{
		m_view->setFilename(filename);
	}

	void StatusPresenter::newJobLastFileName(const char* filename)
	{
		if (filename[0] == 0)
		{
			return;
		}
		m_view->setFilename(utils::format("Printed: %s", filename).c_str());
	}

	void StatusPresenter::newJobPrintTime() {}

	void StatusPresenter::newJobDuration()
	{
		uint32_t progress = 0;
		{
			MODEL_LOCK();
			uint32_t elapsed = OM::GetPrintDuration();
			m_view->updateElapsedTime(elapsed);

			// Progress
			uint32_t warmupTime = OM::GetWarmUpDuration();
			uint32_t totalDuration = std::max<uint32_t>(OM::GetPrintTime(), OM::GetSimulatedTime());
			progress =
				totalDuration == 0
					? 0
					: std::min<uint32_t>((100 * std::max<uint32_t>(0, elapsed - warmupTime)) / totalDuration, 100);
		}

		m_view->updateProgress(progress);
	}

	void StatusPresenter::newJobTimeLeft()
	{
		uint32_t timeRemaining = OM::GetPrintRemaining(OM::RemainingTimeType::AUTO);

		m_view->updateRemainingTime(timeRemaining);
	}

	void StatusPresenter::newCurrentMoveRequestedSpeed()
	{
		m_view->updateSpeed(OM::Move::GetCurrentMoveTopSpeed(), OM::Move::GetCurrentMoveRequestedSpeed());
	}

	void StatusPresenter::newCurrentMoveTopSpeed()
	{
		m_view->updateSpeed(OM::Move::GetCurrentMoveTopSpeed(), OM::Move::GetCurrentMoveRequestedSpeed());
	}

	void StatusPresenter::newCurrentMoveExtrusionSpeed()
	{
		m_view->updateExtrusionRate(OM::Move::GetExtrusionRate(), OM::Move::GetVolumetricFlow());
	}

	void StatusPresenter::newAxesData()
	{
		auto axis = OM::Move::GetAxisByLetter('Z');
		if (axis == nullptr)
		{
			m_view->updateLayer(0, 0);
			return;
		}

		// TODO get max height
		m_view->updateLayer(axis->userPosition, 0);
	}

	void StatusPresenter::newExtruderData()
	{
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			m_view->updateFlowMultiplier(100);
			return;
		}
		// TODO show all extruder multipliers
		size_t extruderCount = 0;
		uint32_t flowMultiplier = 0;
		tool->IterateExtruders(
			[&](std::shared_ptr<OM::Move::ExtruderAxis> extruder, size_t index)
			{
				flowMultiplier += 100 * extruder->factor;
				extruderCount++;
			});

		if (extruderCount == 0)
		{
			m_view->updateFlowMultiplier(100);
			return;
		}

		m_view->updateFlowMultiplier(flowMultiplier / extruderCount);
	}

	void StatusPresenter::newSpeedFactor()
	{
		m_view->updateSpeedMultiplier(100 * OM::Move::GetSpeedFactor());
	}

	void StatusPresenter::newHeaterData()
	{
		auto tool = OM::GetCurrentTool();

		if (tool == nullptr || tool->GetHeaterCount() == 0)
		{
			m_view->updateToolTemp(0, 0);
		}
		else
		{
			m_view->updateToolTemp(tool->GetHeater(0)->heater->current, tool->GetHeater(0)->activeTemp);
		}

		auto bed = OM::GetBedBySlot(0);
		if (bed == nullptr)
		{
			m_view->updateBedTemp(0, 0);
		}
		else
		{
			m_view->updateBedTemp(bed->GetCurrentTemp(), bed->GetCurrentTarget());
		}
	}

	void StatusPresenter::newFanData()
	{
		uint32_t fanSpeed = 0;
		{
			MODEL_LOCK();
			auto tool = OM::GetCurrentTool();

			if (tool != nullptr)
			{
				// TODO show all fan speeds
				tool->IterateFans([&](std::shared_ptr<OM::Fan> fan, size_t index) { fanSpeed = fan->requestedValue; });
			}
		}
		m_view->updateFanSpeed(fanSpeed);
	}

	void StatusPresenter::newStatus(OM::PrinterStatus status)
	{
		UI_LOCK();
		switch (status)
		{
		case OM::PrinterStatus::printing:
		case OM::PrinterStatus::simulating:
			m_view->setResume(StatusView::HIDDEN);
			m_view->setPause(StatusView::ENABLED);
			m_view->setPrintAgain(StatusView::HIDDEN);
			m_view->setCancel(StatusView::DISABLED);
			break;
		case OM::PrinterStatus::paused:
			m_view->setResume(StatusView::ENABLED);
			m_view->setPause(StatusView::HIDDEN);
			m_view->setPrintAgain(StatusView::HIDDEN);
			m_view->setCancel(StatusView::ENABLED);
			break;
		case OM::PrinterStatus::pausing:
			m_view->setResume(StatusView::DISABLED);
			m_view->setPause(StatusView::HIDDEN);
			m_view->setPrintAgain(StatusView::HIDDEN);
			m_view->setCancel(StatusView::DISABLED);
			break;
		case OM::PrinterStatus::resuming:
			m_view->setResume(StatusView::HIDDEN);
			m_view->setPause(StatusView::DISABLED);
			m_view->setPrintAgain(StatusView::HIDDEN);
			m_view->setCancel(StatusView::DISABLED);
			break;
		case OM::PrinterStatus::cancelling:
			m_view->setResume(StatusView::HIDDEN);
			m_view->setPause(StatusView::HIDDEN);
			m_view->setPrintAgain(StatusView::DISABLED);
			m_view->setCancel(StatusView::DISABLED);
			break;
		default:
			m_view->setResume(StatusView::HIDDEN);
			m_view->setPause(StatusView::HIDDEN);
			m_view->setPrintAgain(StatusView::ENABLED);
			m_view->setCancel(StatusView::DISABLED);
			break;
		}
	}
} // namespace UI
