#include "FanPresenter.h"
#include "Debug.h"
#include "FanView.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Fan.h"
#include "lv_i18n/lv_i18n.h"

namespace UI
{
	void FanPresenter::setFanSpeed(size_t slot, uint32_t value)
	{
		auto fan = OM::GetFanBySlot(slot);
		if (fan == nullptr || value == std::round(100 * fan->requestedValue))
		{
			return;
		}

		Comm::DUET.SendGcodef("M106 P%u S%u", fan->index, (uint32_t)std::round(2.55 * value));
	}

	void FanPresenter::newFanData()
	{
		MODEL_LOCK();

		m_view->setFanCount(OM::GetFanCount());

		OM::IterateFansWhile(
			[&](std::shared_ptr<OM::Fan> fan, size_t index)
			{
				m_view->setFanLabel(index, utils::format(_("fine_tune_fan"), fan->index).c_str());
				m_view->setFanValue(index, std::round(100 * fan->requestedValue));
				return true;
			});
	}
} // namespace UI
