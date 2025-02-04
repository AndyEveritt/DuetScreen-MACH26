#include "HomePresenter.h"
#include "HomeView.h"

#include "ObjectModel/Heat.h"
#include "ObjectModel/Sensor.h"
#include "UI/Core/Navigation.h"
#include "lv_i18n/lv_i18n.h"
#include "utils/StorageHelper.h"

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

	void HomePresenter::newResponse(const char* resp)
	{
		Lock lock;
		if (m_view->m_consoleView.isVisible())
		{
			return;
		}

		if (resp)
		{
			std::shared_ptr<MessageBox> msgBox = m_view->createMessageBox();
			msgBox->setTitle("Response");
			msgBox->setText(resp);
			msgBox->setCloseCallback(
				[this]()
				{
					m_view->popMessageBox();
					if (m_view->getMessageBoxCount() > 0)
					{
						m_view->getMessageBox(0)->show();
					}
				});

			msgBox->setCancelBtnText(_("close"));
			msgBox->setOkBtnText(_("open_console"));
			msgBox->setOkCallback(
				[this]()
				{
					m_view->clearMessageBoxes();
					openScreen(&m_view->m_consoleView);
				});
			msgBox->okVisible(true);
			msgBox->setTimeout(StorageHelper::getData(ID_INFO_TIMEOUT, 5000));
		}
	}
} // namespace UI
