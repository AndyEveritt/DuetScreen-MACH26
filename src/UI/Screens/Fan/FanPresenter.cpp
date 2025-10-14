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
		auto fan = OM::GetFanBySlot(slot);
		if (fan == nullptr || value == std::round(100 * fan->requestedValue))
		{
			return;
		}

		Comm::DUET.SendGcodef("M106 P{:d} S{:d}\n", fan->index, (uint32_t)std::round(2.55 * value));
	}

	void FanPresenter::newFanData()
	{
		m_view->setFanCount(OM::GetFanCount());

		OM::IterateFansWhile(
			[&](std::shared_ptr<OM::Fan> fan, size_t index)
			{
				m_view->setFanLabel(index, fmt::format(fmt::runtime(_("fine_tune.fan")), fan->index));
				m_view->setFanValue(index, std::round(100 * fan->requestedValue));
				return true;
			});
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
