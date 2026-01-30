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
		ZoneScoped;
		OM::Move::Extrude(distance, feedrate);
	}

	void ExtruderControlPresenter::newToolData()
	{
		ZoneScoped;
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
		else
		{
			canExtrude = false;
			canRetract = false;
		}

		m_view->setExtrudeDisabled(!canExtrude);
		m_view->setRetractDisabled(!canRetract);
	}

	void ExtruderControlPresenter::onInit()
	{
		ZoneScoped;
		registerEventListener<EventType::ToolData>(this, &ExtruderControlPresenter::newToolData);
	}

	void ExtruderControlPresenter::onActivate()
	{
		ZoneScoped;
		newToolData();
	}

	void ExtruderControlPresenter::onConnect() {}

	void ExtruderControlPresenter::onDisconnect()
	{
		ZoneScoped;
		m_view->clear();
	}
} // namespace UI
