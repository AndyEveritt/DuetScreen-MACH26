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

	void PrintInfoPresenter::newAxesData()
	{
		{
			MODEL_LOCK();
			int32_t max_speed = 1;
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
				max_speed = std::max(max_speed, axis->maxSpeed);
			}
			m_view->setMaxSpeed(max_speed);

			/* Update babystep */
			if (auto axis = OM::Move::GetAxisByLetter('Z'))
			{
				m_view->updateBabyStep(axis->babystep);
			}
			else
			{
				m_view->updateBabyStep(0.0f);
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
		int32_t extruderMaxSpeed = 1;
		uint32_t extruderCount = 0;
		uint32_t flowMultiplier = 0;
		tool->IterateExtruders(
			[&](std::shared_ptr<OM::Move::ExtruderAxis> extruder, size_t /* index */)
			{
				flowMultiplier += static_cast<uint32_t>(std::lround(100 * extruder->factor));
				extruderCount++;
				extruderMaxSpeed = std::max(extruderMaxSpeed, extruder->maxSpeed);
			});

		if (extruderCount == 0)
		{
			m_view->updateFlowMultiplier(100);
			return;
		}

		m_view->updateFlowMultiplier(flowMultiplier / extruderCount);
		m_view->setMaxExtrusionRate(extruderMaxSpeed);
	}

	void PrintInfoPresenter::newSpeedFactor()
	{
		m_view->updateSpeedMultiplier(static_cast<uint32_t>(std::lround(100 * OM::Move::GetSpeedFactor())));
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
