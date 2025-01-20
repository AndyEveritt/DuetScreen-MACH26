#include "HomePresenter.h"
#include "HomeView.h"

#include "ObjectModel/Heat.h"
#include "ObjectModel/Sensor.h"

namespace UI
{
	void HomePresenter::tick()
	{
		Lock lock;
		const size_t sensorCount = OM::GetAnalogSensorCount();
		for (size_t i = 0; i < sensorCount; i++)
		{
			OM::AnalogSensor* sensor = OM::GetAnalogSensorBySlot(i);
			if (!m_view->m_graph.getSeries(sensor->name.c_str()))
			{
				m_view->m_graph.createSeries(sensor->name.c_str(),
											 lv_palette_main((lv_palette_t)m_view->m_graph.getSeriesCount()),
											 sensor->name.c_str());
			}
			m_view->m_graph.addData(sensor->name.c_str(), sensor->lastReading);
		}
	}
} // namespace UI
