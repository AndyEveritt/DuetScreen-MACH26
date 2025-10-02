/*
 * TemperatureGraphPresenter.cpp
 *
 *  Created on: 2025-10-02
 *      Author: Andy Everitt
 */

#include "TemperatureGraphPresenter.h"
#include "Debug.h"
#include "ObjectModel/Heat.h"
#include "ObjectModel/Sensor.h"
#include "TemperatureGraph.h"

namespace UI
{
	void TemperatureGraphPresenter::tick()
	{
		// Update the graph with new temperature data
		UI_LOCK();
		const size_t sensorCount = OM::GetAnalogSensorCount();
		m_view->setSeriesCount(sensorCount);
		for (size_t i = 0; i < sensorCount; i++)
		{
			auto sensor = OM::GetAnalogSensorBySlot(i);
			if (!m_view->getSeries(i))
			{
				m_view->createSeries(lv_palette_main((lv_palette_t)m_view->getSeriesCount()), sensor->name.c_str());
			}
			m_view->updateSeriesName(i, sensor->name.c_str());
			m_view->addData(i, sensor->lastReading);
		}

		size_t heaterCount = OM::Heat::GetHeaterCount();
		if (heaterCount > 0)
		{
			int32_t maxTemperature = 300; // Default max temperature
			for (size_t i = 0; i < heaterCount; i++)
			{
				auto heater = OM::Heat::GetHeaterBySlot(i);
				if (heater == nullptr)
				{
					continue;
				}
				maxTemperature = std::max(maxTemperature, (int32_t)heater->max);
			}
			m_view->setYRange({0, maxTemperature});
		}
	}
} // namespace UI
