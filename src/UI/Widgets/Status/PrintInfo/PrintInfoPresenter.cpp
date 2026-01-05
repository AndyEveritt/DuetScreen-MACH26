/*
 * PrintInfoPresenter.cpp
 *
 *  Created on: 2025-11-10
 *      Author: Andy Everitt
 */

#include "PrintInfoPresenter.h"
#include "Debug.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Job.h"
#include "ObjectModel/Tool.h"
#include "PrintInfo.h"
#include "UI/Screens/Home/HomeView.h"
#include "i18n/i18n.h"

namespace UI
{
	void PrintInfoPresenter::onInit()
	{
		registerEventListener<EventType::JobDuration>(this, &PrintInfoPresenter::newJobDuration);
		registerEventListener<EventType::JobTimeLeft>(this, &PrintInfoPresenter::newJobTimeLeft);
		registerEventListener<EventType::CurrentMoveRequestedSpeed>(this,
																	&PrintInfoPresenter::newCurrentMoveRequestedSpeed);
		registerEventListener<EventType::CurrentMoveTopSpeed>(this, &PrintInfoPresenter::newCurrentMoveTopSpeed);
		registerEventListener<EventType::CurrentMoveExtrusionSpeed>(this,
																	&PrintInfoPresenter::newCurrentMoveExtrusionSpeed);
		registerEventListener<EventType::AxesData>(this, &PrintInfoPresenter::newAxesData);
		registerEventListener<EventType::ExtruderData>(this, &PrintInfoPresenter::newExtruderData);
		registerEventListener<EventType::SpeedFactor>(this, &PrintInfoPresenter::newSpeedFactor);
	}

	void PrintInfoPresenter::onActivate()
	{
		newAxesData();
		newExtruderData();
		newSpeedFactor();
		newJobDuration();
		newJobTimeLeft();
		newCurrentMoveRequestedSpeed();
		newCurrentMoveTopSpeed();
		newCurrentMoveExtrusionSpeed();
	}

	void PrintInfoPresenter::openSpeedFactorModal()
	{
		auto& np = HomeView::instance().getNumberPad();
		np.setHeader(_("fine_tune.speed_factor"));
		np.setValue(OM::Move::GetSpeedFactor() * 100);
		np.setMinValue(1);
		np.setMaxValue(1000);
		np.setConfirmCallback(
			[](float value)
			{
				if (value == std::round(100 * OM::Move::GetSpeedFactor()))
				{
					return;
				}
				Comm::DUET.SendGcodef("M220 S{:g}\n", value);
			});
		openModal(&np);
	}

	void PrintInfoPresenter::openExtrusionFactorModal()
	{
		auto& modal = m_view->getModalExtrusionFactor();
		openModal(&modal);
	}

	void PrintInfoPresenter::newAxesData()
	{
		{
			MODEL_LOCK();
			size_t axis_count = OM::Move::GetAxisCount();
			m_view->setAxisCount(axis_count);
			for (size_t i = 0; i < axis_count; i++)
			{
				auto axis = OM::Move::GetAxisBySlot(i);
				if (axis == nullptr)
				{
					continue;
				}
				m_view->setPosition(i, axis->letter[0], axis->userPosition);
			}
		}
	}

	void PrintInfoPresenter::newExtruderData()
	{
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			m_view->updateFlowMultiplier(100);
			return;
		}
		// TODO show all extruder multipliers
		uint32_t extruderCount = 0;
		uint32_t flowMultiplier = 0;
		tool->IterateExtruders(
			[&](std::shared_ptr<OM::Move::ExtruderAxis> extruder, size_t /* index */)
			{
				flowMultiplier += static_cast<uint32_t>(100 * extruder->factor);
				extruderCount++;
			});

		if (extruderCount == 0)
		{
			m_view->updateFlowMultiplier(100);
			return;
		}

		m_view->updateFlowMultiplier(flowMultiplier / extruderCount);
	}

	void PrintInfoPresenter::newSpeedFactor()
	{
		m_view->updateSpeedMultiplier(static_cast<uint32_t>(100 * OM::Move::GetSpeedFactor()));
	}

	void PrintInfoPresenter::newJobDuration()
	{
		uint32_t elapsed = OM::GetPrintDuration();
		m_view->updateElapsedTime(elapsed);
	}

	void PrintInfoPresenter::newJobTimeLeft()
	{
		uint32_t timeRemaining = OM::GetPrintRemaining(OM::RemainingTimeType::AUTO);

		m_view->updateRemainingTime(timeRemaining);
	}

	void PrintInfoPresenter::newCurrentMoveRequestedSpeed()
	{
		m_view->updateSpeed(OM::Move::GetCurrentMoveTopSpeed(), OM::Move::GetCurrentMoveRequestedSpeed());
	}

	void PrintInfoPresenter::newCurrentMoveTopSpeed()
	{
		m_view->updateSpeed(OM::Move::GetCurrentMoveTopSpeed(), OM::Move::GetCurrentMoveRequestedSpeed());
	}

	void PrintInfoPresenter::newCurrentMoveExtrusionSpeed()
	{
		m_view->updateExtrusionRate(OM::Move::GetExtrusionRate(), OM::Move::GetVolumetricFlow());
	}
} // namespace UI
