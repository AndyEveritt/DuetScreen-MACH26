/*
 * ExtruderControlPresenter.cpp
 *
 *  Created on: 2025-11-16
 *      Author: Andy Everitt
 */

#include "ExtruderControlPresenter.h"
#include "Debug.h"
#include "ExtruderControl.h"
#include "ObjectModel/Axis.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/Tool.h"
#include "UI/Core/Navigation.h"
#include "i18n/i18n.h"

namespace UI
{

	void ExtruderControlPresenter::extrude(float distance, float feedrate)
	{
		OM::Move::Extrude(distance, feedrate);
	}

	void ExtruderControlPresenter::toggleToolState(size_t index)
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

	void ExtruderControlPresenter::loadFilament(const std::string& filament)
	{
		MODEL_LOCK();
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			return;
		}
		tool->ChangeFilament(filament.c_str());
	}

	void ExtruderControlPresenter::unloadFilament()
	{
		MODEL_LOCK();
		auto tool = OM::GetCurrentTool();
		if (tool == nullptr)
		{
			return;
		}
		tool->UnloadFilament();
	}

	void ExtruderControlPresenter::newToolData()
	{
		auto currentTool = OM::GetCurrentTool();
		bool canExtrude = true;
		bool canRetract = true;

		if (currentTool)
		{
			UI_LOCK();

			const float coldExtrudeTemp = OM::Heat::GetColdExtrudeTemperature();
			const float coldRetractTemp = OM::Heat::GetColdRetractTemperature();
			currentTool->IterateHeaters(
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
		}

		m_view->setExtrudeDisabled(currentTool && !canExtrude);
		m_view->setRetractDisabled(currentTool && !canRetract);
	}

	void ExtruderControlPresenter::onInit()
	{
		m_view->setDistanceCallback(
			[this](size_t index, float distance)
			{
				NumberPad* np = m_view->getNumberPad();
				if (!np)
				{
					LOG_ERROR("NumberPad not available in ExtruderControl view");
					return;
				}
				openModal(np);
				np->setHeader(_("extrude.distance_header", Units::getDisplayedDistanceUnit()));
				np->setValue(distance);
				np->setMinValue(0);
				np->setMaxValue(1000);
				np->setConfirmCallback([this, index](float distance) { m_view->setDistanceValue(index, distance); });
			});
		m_view->setFeedrateCallback(
			[this](size_t index, float feedrate)
			{
				NumberPad* np = m_view->getNumberPad();
				if (!np)
				{
					LOG_ERROR("NumberPad not available in ExtruderControl view");
					return;
				}
				openModal(np);
				np->setHeader(_("extrude.feedrate_header", Units::getDisplayedSpeedUnit()));
				np->setValue(feedrate);
				np->setMinValue(0);
				np->setMaxValue(100); // mm/s
				np->setConfirmCallback([this, index](float feedrate) { m_view->setFeedrateValue(index, feedrate); });
			});
		m_view->setExtrudeCallback([this](float distance, float feedrate) { extrude(distance, feedrate); });

		registerEventListener<EventType::ToolData>(this, &ExtruderControlPresenter::newToolData);
	}

	void ExtruderControlPresenter::onActivate()
	{
		newToolData();
	}

	void ExtruderControlPresenter::onConnect() {}

	void ExtruderControlPresenter::onDisconnect()
	{
		m_view->clear();
	}
} // namespace UI
