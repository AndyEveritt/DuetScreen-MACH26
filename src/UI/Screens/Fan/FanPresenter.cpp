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
		auto& fan = m_controllableFans.at(slot);
		if (fan == nullptr || value == std::round(100 * fan->requestedValue))
		{
			return;
		}

		Comm::DUET.SendGcodef("M106 P{:d} S{:d}\n", fan->index, (uint32_t)std::round(2.55 * value));
	}

	void FanPresenter::newFanData()
	{
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
		newFanData();
	}

	void FanPresenter::onDisconnect()
	{
		m_view->setFanCount(0);
	}
} // namespace UI
