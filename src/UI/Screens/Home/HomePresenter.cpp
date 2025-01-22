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
		m_view->m_graph.setSeriesCount(sensorCount);
		for (size_t i = 0; i < sensorCount; i++)
		{
			OM::AnalogSensor* sensor = OM::GetAnalogSensorBySlot(i);
			if (!m_view->m_graph.getSeries(i))
			{
				m_view->m_graph.createSeries(lv_palette_main((lv_palette_t)m_view->m_graph.getSeriesCount()),
											 sensor->name.c_str());
			}
			m_view->m_graph.updateSeriesName(i, sensor->name.c_str());
			m_view->m_graph.addData(i, sensor->lastReading);
		}
	}

	void HomePresenter::refresh()
	{
		Lock lock;
		m_view->refresh();
	}
} // namespace UI
