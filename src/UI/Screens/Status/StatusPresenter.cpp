#include "StatusPresenter.h"
#include "Comm/FileInfo.h"
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
#include "lv_i18n/lv_i18n.h"

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
		newJobFileName(OM::GetJobName());
		newJobLastFileName(OM::GetLastJobName());
		newJobPrintTime();
		newJobDuration();
		newJobTimeLeft();
		newCurrentMoveRequestedSpeed();
		newCurrentMoveTopSpeed();
		newCurrentMoveExtrusionSpeed();
		updateLayerInfo();
		newExtruderData();
		newSpeedFactor();
		newFanData();
		newStatus(OM::GetStatus());

		std::string filename = OM::GetJobName();
		if (filename.empty())
		{
			LOG_DBG("No current job name");
			filename = OM::GetLastJobName();
		}
		if (filename.empty())
		{
			LOG_DBG("No job name");
			return;
		}

		setOrRequestThumbnail(filename);
	}

	void StatusPresenter::onDeactivate() {}

	void StatusPresenter::newJobFileName(const std::string& filename)
	{
		m_view->setFilename(filename.c_str());
		setOrRequestThumbnail(filename);
	}

	void StatusPresenter::newThumbnailData(const std::string& filename)
	{
		if (!filename.empty() && filename != OM::GetJobName() && filename != OM::GetLastJobName())
		{
			// Thumbnail is not for the current job
			return;
		}

		m_view->setThumbnail(GetThumbnailPath(filename.c_str()).c_str());
	}

	void StatusPresenter::newJobLastFileName(const std::string& filename)
	{
		if (filename.empty())
		{
			return;
		}
		m_view->setFilename(utils::format(_("status_printed_header"), filename.c_str()).c_str());
		setOrRequestThumbnail(filename);
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

	void StatusPresenter::updateLayerInfo()
	{
		auto axis = OM::Move::GetAxisByLetter('Z');
		if (axis == nullptr)
		{
			m_view->updateLayer(0, OM::GetPrintHeight());
			return;
		}

		m_view->updateLayer(axis->userPosition, OM::GetPrintHeight());
	}

	void StatusPresenter::newAxesData()
	{
		char axisNames[] = {'X', 'Y'};
		float positions[] = {0, 0, 0};
		float zOffset = 0;

		for (size_t i = 0; i < ARRAY_SIZE(axisNames); i++)
		{
			auto axis = OM::Move::GetAxisByLetter(axisNames[i]);
			if (axis == nullptr)
			{
				continue;
			}
			positions[i] = axis->userPosition;
		}

		auto axis = OM::Move::GetAxisByLetter('Z');
		if (axis != nullptr)
		{
			zOffset = axis->babystep;
		}

		m_view->updateAcceleration(OM::Move::GetPrintingAcceleration());
		m_view->updatePosition(positions[0], positions[1], positions[2]);
		m_view->updateZOffset(zOffset);
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

	void StatusPresenter::setOrRequestThumbnail(const std::string& filename)
	{
		if (filename.empty())
		{
			return;
		}

		if (IsThumbnailCached(filename.c_str()))
		{
			newThumbnailData(filename);
			return;
		}

		LOG_DBG("Requesting thumbnail for '{:s}'", filename.c_str());
		FILEINFO_CACHE->QueueThumbnailRequest(filename, true);
	}
} // namespace UI
