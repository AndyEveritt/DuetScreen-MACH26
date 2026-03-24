/*
 * MotionSystemsPresenter.cpp
 *
 *  Created on: 2026-03-24
 */

#include "MotionSystemsPresenter.h"
#include "Debug.h"
#include "MotionSystemsView.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Tool.h"
#include "i18n/i18n.h"
#include <cmath>

namespace UI
{
	static std::array<uint32_t, 2> speedFactors{100, 100};
	static std::array<float, 2> currentSpeeds{0.0f, 0.0f};

	void MotionSystemsPresenter::tick()
	{
		/* Populate with random data for testing */
		for (auto& panel : m_view->getMotionSystemPanels())
		{
			const size_t idx = &panel - m_view->getMotionSystemPanels().data();
			auto& sf = speedFactors[idx];
			sf += rand() % 5 - 2;
			sf = std::clamp(sf, 1u, 100u);
			panel.setSpeedFactor(sf);

			float targetSpeed = static_cast<float>(rand() % 500);
			float& currentSpeed = currentSpeeds[idx];
			currentSpeed += std::min(targetSpeed - currentSpeed, 50.0f);
			panel.setSpeeds(currentSpeed, targetSpeed);
		}
	}

	void MotionSystemsPresenter::onInit()
	{
		ZoneScoped;
		registerEventListener<EventType::Tick>(this, &MotionSystemsPresenter::tick);
		registerEventListener<EventType::CurrentTool>(this, &MotionSystemsPresenter::newCurrentTool);
		registerEventListener<EventType::SpeedFactor>(this, &MotionSystemsPresenter::newSpeedFactor);
		registerEventListener<EventType::CurrentMoveRequestedSpeed>(
			this, &MotionSystemsPresenter::newCurrentMoveRequestedSpeed);
		registerEventListener<EventType::CurrentMoveTopSpeed>(this, &MotionSystemsPresenter::newCurrentMoveTopSpeed);
	}

	void MotionSystemsPresenter::onActivate()
	{
		ZoneScoped;
		refreshTool();
		refreshSpeedFactor();
		refreshSpeeds();
	}

	void MotionSystemsPresenter::onDisconnect()
	{
		ZoneScoped;
	}

	void MotionSystemsPresenter::newCurrentTool()
	{
		ZoneScoped;
		refreshTool();
	}

	void MotionSystemsPresenter::newSpeedFactor()
	{
		ZoneScoped;
		refreshSpeedFactor();
	}

	void MotionSystemsPresenter::newCurrentMoveRequestedSpeed()
	{
		ZoneScoped;
		refreshSpeeds();
	}

	void MotionSystemsPresenter::newCurrentMoveTopSpeed()
	{
		ZoneScoped;
		refreshSpeeds();
	}

	void MotionSystemsPresenter::refreshTool()
	{
		ZoneScoped;
	}

	void MotionSystemsPresenter::refreshSpeedFactor()
	{
		ZoneScoped;
	}

	void MotionSystemsPresenter::refreshSpeeds()
	{
		ZoneScoped;
	}
} // namespace UI