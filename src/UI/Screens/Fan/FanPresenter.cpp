#include "FanPresenter.h"
#include "Debug.h"
#include "FanView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Fan.h"
#include "i18n/i18n.h"

namespace UI
{
	void FanPresenter::setFanSpeed(size_t slot, uint32_t value)
	{
		ZoneScoped;
		auto& fan = m_controllableFans.at(slot);
		if (fan == nullptr)
			return;

		const uint32_t currentValue = static_cast<uint32_t>(std::round(100 * fan->requestedValue));
		if (value == currentValue)
		{
			return;
		}

		Comm::DUET.SendGcodef("M106 P{:d} S{:d}\n", fan->index, (255 * value / 100));
	}

	void FanPresenter::newFanData()
	{
		ZoneScoped;
		m_controllableFans.resize(OM::GetFanCount());

		size_t count = 0;
		OM::IterateFansWhile(
			[&](OM::FanPtr fan, size_t /* index */)
			{
				if (!fan->thermostatic)
				{
					m_controllableFans.at(count) = std::move(fan);
					count++;
				}
				return true;
			});

		m_view->setFanCount(count);

		for (size_t i = 0; i < count; i++)
		{
			auto& fan = m_controllableFans.at(i);
			m_view->setFanLabel(i, _("fine_tune.fan", fan->index));
			m_view->setFanValue(i, static_cast<uint32_t>(std::round(100 * fan->requestedValue)));
		}
	}

	void FanPresenter::onActivate()
	{
		ZoneScoped;
		newFanData();
	}

	void FanPresenter::onDisconnect()
	{
		ZoneScoped;
		m_view->setFanCount(0);
	}
} // namespace UI
