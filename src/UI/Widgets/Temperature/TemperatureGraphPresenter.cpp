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
#include "i18n/i18n.h"

namespace UI
{
	void TemperatureGraphPresenter::tick()
	{
		ZoneScoped;
		// Update the graph with new temperature data
		UI_LOCK();
		const size_t heaterCount = OM::Heat::GetHeaterCount();

		std::array<std::optional<size_t>, MAX_SENSORS> seenSensors{
			std::nullopt}; // so we don't show multiple heaters with same sensor

		m_view->setSeriesCount(heaterCount);
		int32_t maxTemperature = 300; // Default max temperature
		for (size_t i = 0; i < heaterCount; i++)
		{
			const auto heater = OM::Heat::GetHeaterBySlot(i);
			if (heater == nullptr || heater->sensor == nullptr ||
				std::find(seenSensors.begin(), seenSensors.end(), heater->sensor->index) != seenSensors.end())
			{
				continue;
			}

			seenSensors[i] = heater->sensor->index;

			const auto& sensor = heater->sensor;
			std::string name(heater->GetName());
			if (name.empty())
			{
				name = _("temperature.heater", heater->index);
			}

			if (!m_view->getSeries(i))
			{
				m_view->createSeries(lv_palette_main((lv_palette_t)m_view->getSeriesCount()), name);
			}

			m_view->updateSeriesName(i, name);
			m_view->addData(i, static_cast<int32_t>(sensor->lastReading));
			maxTemperature = std::max(maxTemperature, (int32_t)heater->max);
		}

		m_view->setYRange({0, maxTemperature});
	}
} // namespace UI
