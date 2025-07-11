#include "HeaterSliderPresenter.h"
#include "Debug.h"
#include "HeaterSlider.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void HeaterSliderPresenter::setHeaterIndex(size_t index)
	{
		UI_LOCK();
		m_heaterIndex = index;
		m_heater = OM::Heat::GetHeater(index);
		LOG_DBG("Set heater index to {} for presenter '{}'", index, getName());
	}

	void HeaterSliderPresenter::newHeaterData()
	{
		if (m_heater == nullptr)
		{
			// Try to get the heater if it hasn't been set yet
			m_heater = OM::Heat::GetHeater(m_heaterIndex);
			if (m_heater == nullptr)
			{
				LOG_WARN("No heater set for presenter '{}'", getName());
				return;
			}
		}

		m_view->setHeaterName(m_heater->GetName());
		m_view->setHeaterState(_(m_heater->GetHeaterStatusStr()));
		m_view->setHeaterMinTemperature(m_heater->min);
		m_view->setHeaterMaxTemperature(m_heater->max);
		m_view->setCurrentTemperature(m_heater->current);
		m_view->setActiveTemperature(m_heater->activeTemp);
		m_view->setStandbyTemperature(m_heater->standbyTemp);
	}
} // namespace UI
